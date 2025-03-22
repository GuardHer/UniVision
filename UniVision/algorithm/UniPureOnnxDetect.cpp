#include "UniPureOnnxDetect.h"

#include "core/UniLog.h"
#include "UniOnnxModel.h"
#include "UniAlgorithmUtils.h"

#include <opencv2/dnn.hpp>
#include <onnxruntime_cxx_api.h>

UniPureOnnxDetect::UniPureOnnxDetect(const std::vector<std::string> &classes,
                                     std::vector<cv::Scalar> colors,
                                     const AlgotithmParam &param) :
    UniBaseOnnxDetect(param)
{
    _classes = classes;
    _colors  = colors;
}

UniPureOnnxDetect::~UniPureOnnxDetect()
{
}


bool UniPureOnnxDetect::preprocess(const cv::Mat &src, cv::Mat &dst) const
{
    if (src.empty()) {
        LOG_ERROR("Input image is empty.");
        return false;
    }

    cv::cvtColor(src, src, cv::COLOR_BGR2RGB); // RGB


    std::vector<cv::Mat> channels;
    cv::split(src, channels); // 分离通道

    // 提取第一个通道（B 通道，索引 0），并保持三维形状 (H, W, 1)
    cv::Mat single_channel;
    channels[0].convertTo(single_channel, CV_8U); // 确保类型为无符号 8 位整数

    int channel = single_channel.channels();

    single_channel.convertTo(single_channel, CV_32FC(channel));

    // std::vector<float> mean -> cv::Scalar mean
    cv::Scalar mean;
    for (int i = 0; i < channel; i++) {
        mean[i] = _param._meanValues[i];
    }
    // std::vector<float> std -> cv::Scalar std
    cv::Scalar std;
    for (int i = 0; i < channel; i++) {
        mean[i] = _param._stdValues[i];
    }
    cv::subtract(single_channel, mean * 255.0f, single_channel, cv::noArray(), CV_32FC(channel));
    cv::divide(single_channel, std * 255.0f, single_channel, 1.0, CV_32FC(channel));
    // image = normalize(image, vmean, vstd);
    dst = cv::dnn::blobFromImage(single_channel,
                                 1.0,                                                // 缩放到 [0, 1]
                                 cv::Size(single_channel.cols, single_channel.rows), // 原始尺寸
                                 cv::Scalar(0),                                      // 减去均值
                                 false, // 不交换 RB（单通道无需考虑）
                                 false, // 不裁剪
                                 CV_32F // 输出为 float32
    );

    return true;
}


void UniPureOnnxDetect::detect(const cv::Mat &src, std::vector<OutputParams> &results, cv::Mat &dstImg) const
{
    cv::Mat blob;
    if (!preprocess(src, blob)) {
        LOG_ERROR("UniPureOnnxDetect::preprocess: failed.");
        return;
    }

    int64_t input_tensor_length =
    std::accumulate(_model->_inputTensorShape.begin(), _model->_inputTensorShape.end(), 1, std::multiplies<int64_t>());

    std::vector<Ort::Value> input_tensors;
    input_tensors.emplace_back(Ort::Value::CreateTensor<float>(_model->_OrtMemoryInfo, (float *) blob.data,
                                                               input_tensor_length, _model->_inputTensorShape.data(),
                                                               _model->_inputTensorShape.size()));

    std::vector<Ort::Value> output_tensors;
    bool res = _model->infer(input_tensors, output_tensors);
    if (!res) {
        LOG_ERROR("UniPureOnnxDetect::detect: inference failed.");
        return;
    }
    dstImg = src.clone();
    postprocess(output_tensors, results, dstImg);
}

void UniPureOnnxDetect::postprocess(std::vector<Ort::Value> &output_tensors,
                                    std::vector<OutputParams> &results,
                                    cv::Mat &dstImg) const
{
    if (output_tensors.empty()) {
        LOG_ERROR("UniPureOnnxDetect::postprocess: invalid output tensor size.");
        return;
    }
    Ort::Value &output_tensor = output_tensors[0];


    float *tensor_data         = output_tensor.GetTensorMutableData<float>();
    auto out_shape             = output_tensor.GetTensorTypeAndShapeInfo().GetShape(); // b n h w
    _model->_outputTensorShape = out_shape;

    std::vector<float> p_thresh;
    for (int i = 0; i < _model->_outputTensorShape[1]; i++) {
        p_thresh.push_back(_param._confidenceThreshold);
    }

    PurePostProcessor postProcessor(out_shape[1], p_thresh, _param._minArea);

    postProcessor.sigmoid(tensor_data, out_shape[0], out_shape[1], out_shape[2], out_shape[3]);

    std::vector<cv::Mat> preds(out_shape[1]);

    // 遍历每个通道
    for (int c = 0; c < out_shape[1]; ++c) {
        // 计算当前通道的数据偏移量
        const size_t channel_offset = out_shape[1] * c * out_shape[2] * out_shape[3];
        const float *channel_data   = tensor_data + channel_offset;

        cv::Mat channel_mat(out_shape[2], out_shape[3], CV_32FC1, (void *) channel_data);

        preds[c] = channel_mat;
    }

    std::vector<PurePostProcessor::Component> infos;
    cv::Mat mask = postProcessor.postprocess(preds, infos);

    // 遍历掩码的每个像素
    for (int y = 0; y < mask.rows; ++y) {
        for (int x = 0; x < mask.cols; ++x) {
            // 获取类别索引
            int classIdx = mask.type() == CV_8UC1 ? mask.at<uchar>(y, x) : mask.at<int>(y, x);

            // 如果是背景（0），跳过
            if (classIdx == 0) {
                continue;
            }

            if (classIdx >= _colors.size()) {
                std::cerr << "Warning: Class index " << classIdx << " exceeds color vector size!" << std::endl;
                continue;
            }

            // 获取当前像素的颜色
            cv::Scalar maskColor = _colors[classIdx];

            // 获取原图当前像素值
            cv::Vec3b &pixel = dstImg.at<cv::Vec3b>(y, x);

            pixel[0] = cv::saturate_cast<uchar>((1.0) * pixel[0] * maskColor[0]); // B
            pixel[1] = cv::saturate_cast<uchar>((1.0) * pixel[1] * maskColor[1]); // G
            pixel[2] = cv::saturate_cast<uchar>((1.0) * pixel[2] * maskColor[2]); // R
        }
    }

    for (const auto &info : infos) {
        OutputParams output;
        output.id   = info.channel;
        output.area = info.size;
        results.push_back(output);
    }
}
