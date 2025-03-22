#include "UniYoloOnnxDetect.h"
#include "UniOnnxModel.h"

using namespace Ort;


YoloSegOnnxDetect::YoloSegOnnxDetect(const std::vector<std::string> &classes,
                             std::vector<cv::Scalar> colors,
                             const AlgotithmParam &param) :
    UniBaseOnnxDetect(param), _classes(classes), _colors(colors)
{
}

YoloSegOnnxDetect::~YoloSegOnnxDetect()
{
}

bool YoloSegOnnxDetect::preprocess(const cv::Mat &src, cv::Mat &dst) const
{
    cv::Size input_size = cv::Size(_model->_netWidth, _model->_netHeight);
    cv::Mat temp_img    = src;
    _params             = cv::Vec4d{1.0, 1.0, 0.0, 0.0}; // 初始化参数

    if (temp_img.size() != input_size) {
        cv::Mat borderImg;
        LetterBox(temp_img, borderImg, _params, input_size, false, false, true, 32);
        dst = borderImg; // 直接赋值给输出图像
    }
    else {
        dst = temp_img; // 如果大小已匹配，直接使用原图
    }

    return 0;
}


void YoloSegOnnxDetect::detect(const cv::Mat &src, std::vector<OutputParams> &results, cv::Mat &dstImg) const
{
    cv::Size input_size = cv::Size(_model->_netWidth, _model->_netHeight);
    //preprocessing
    cv::Mat input_image;
    preprocess(src, input_image);
    cv::Mat blob = cv::dnn::blobFromImage(input_image, 1 / 255.0, input_size, cv::Scalar(0, 0, 0), true, false);
    int64_t input_tensor_length =
    std::accumulate(_model->_inputTensorShape.begin(), _model->_inputTensorShape.end(), 1, std::multiplies<int64_t>());
    std::vector<Ort::Value> input_tensors;
    std::vector<Ort::Value> output_tensors;

    input_tensors.emplace_back(Ort::Value::CreateTensor<float>(_model->_OrtMemoryInfo, (float *) blob.data,
                                                               input_tensor_length, _model->_inputTensorShape.data(),
                                                               _model->_inputTensorShape.size()));

    bool res = _model->infer(input_tensors, output_tensors);
    if (!res) {
        LOG_ERROR("YoloSegOnnxDetect::detect: inference failed.");
        return;
    }
    dstImg = src.clone();
    postprocess(output_tensors, results, dstImg);
    drawPred(dstImg, results);
}

void YoloSegOnnxDetect::postprocess(std::vector<Ort::Value> &output_tensors,
                                std::vector<OutputParams> &results,
                                cv::Mat &dstImg) const
{
    //post-process
    float *all_data                    = output_tensors[0].GetTensorMutableData<float>();
    _model->_outputTensorShape         = output_tensors[0].GetTensorTypeAndShapeInfo().GetShape();
    _model->_outputMaskTensorShape     = output_tensors[1].GetTensorTypeAndShapeInfo().GetShape();
    std::vector<int> mask_protos_shape = {1, (int) _model->_outputMaskTensorShape[1],
                                          (int) _model->_outputMaskTensorShape[2],
                                          (int) _model->_outputMaskTensorShape[3]};
    int mask_protos_length             = VectorProduct(mask_protos_shape);
    int64_t one_output_length          = VectorProduct(_model->_outputTensorShape) / _model->_outputTensorShape[0];
    int net_width                      = (int) _model->_outputTensorShape[1];
    int socre_array_length             = net_width - 4 - (int) _model->_outputMaskTensorShape[1];

    cv::Mat output0 =
    cv::Mat(cv::Size((int) _model->_outputTensorShape[2], (int) _model->_outputTensorShape[1]), CV_32F, all_data)
    .t(); //[bs,116,8400]=>[bs,8400,116]
    all_data += one_output_length;
    float *pdata = (float *) output0.data;
    int rows     = output0.rows;
    std::vector<int> class_ids;                       //结果id数组
    std::vector<float> confidences;                   //结果每个id对应置信度数组
    std::vector<cv::Rect> boxes;                      //每个id矩形框
    std::vector<std::vector<float>> picked_proposals; //output0[:,:, 5 + _className.size():net_width]===> for mask
    for (int r = 0; r < rows; ++r) {                  //stride
        cv::Mat scores(1, socre_array_length, CV_32F, pdata + 4);
        cv::Point classIdPoint;
        double max_class_socre;
        minMaxLoc(scores, 0, &max_class_socre, 0, &classIdPoint);
        max_class_socre = (float) max_class_socre;
        if (max_class_socre >= _param._confidenceThreshold) {
            std::vector<float> temp_proto(pdata + 4 + socre_array_length, pdata + net_width);
            picked_proposals.push_back(temp_proto);
            //rect [x,y,w,h]
            float x  = (float) ((pdata[0] - _params[2]) / _params[0]); //x
            float y  = (float) ((pdata[1] - _params[3]) / _params[1]); //y
            float w  = (float) (pdata[2] / _params[0]);                          //w
            float h  = (float) (pdata[3] / _params[1]);                          //h
            int left = MAX(int(x - 0.5 * w + 0.5), 0);
            int top  = MAX(int(y - 0.5 * h + 0.5), 0);
            class_ids.push_back(classIdPoint.x);
            confidences.push_back(max_class_socre);
            boxes.push_back(cv::Rect(left, top, int(w + 0.5), int(h + 0.5)));
        }
        pdata += net_width; //下一行
    }

    std::vector<int> nms_result;
    cv::dnn::NMSBoxes(boxes, confidences, _param._confidenceThreshold, _param._nmsThreshold, nms_result);
    std::vector<std::vector<float>> temp_mask_proposals;
    cv::Rect holeImgRect(0, 0, dstImg.cols, dstImg.rows);
    std::vector<OutputParams> temp_output;
    for (int i = 0; i < nms_result.size(); ++i) {
        int idx = nms_result[i];
        OutputParams result;
        result.id         = class_ids[idx];
        result.confidence = confidences[idx];
        result.box        = boxes[idx] & holeImgRect;
        if (result.box.area() < 1)
            continue;
        temp_mask_proposals.push_back(picked_proposals[idx]);
        temp_output.push_back(result);
    }

    MaskParams mask_params;
    mask_params.params        = _params;
    mask_params.srcImgShape   = dstImg.size();
    mask_params.netHeight     = _model->_netHeight;
    mask_params.netWidth      = _model->_netWidth;
    mask_params.maskThreshold = _maskThreshold;
    cv::Mat mask_protos       = cv::Mat(mask_protos_shape, CV_32F,
                                        output_tensors[1].GetTensorMutableData<float>() + 0 * mask_protos_length);
    for (int i = 0; i < temp_mask_proposals.size(); ++i) {
        GetMask2(cv::Mat(temp_mask_proposals[i]).t(), mask_protos, temp_output[i], mask_params);
    }

    results = temp_output;
}

void YoloSegOnnxDetect::drawPred(cv::Mat &img, std::vector<OutputParams> result) const
{
    cv::Mat mask = img.clone();
    for (int i = 0; i < result.size(); i++) {
        float left = 0, top = 0;

        int color_num = i;
        if (result[i].box.area() > 0) {
            cv::rectangle(img, result[i].box, _colors[result[i].id], 2, 8);
            left = result[i].box.x;
            top  = result[i].box.y;
        }
        if (result[i].rotatedBox.size.width * result[i].rotatedBox.size.height > 0) {
            DrawRotatedBox(img, result[i].rotatedBox, _colors[result[i].id], 2);
            left = result[i].rotatedBox.center.x;
            top  = result[i].rotatedBox.center.y;
        }
        if (result[i].boxMask.rows && result[i].boxMask.cols > 0)
            mask(result[i].box).setTo(_colors[result[i].id], result[i].boxMask);
        std::string label = _classes[result[i].id] + ":" + std::to_string(result[i].confidence);
        int baseLine;
        cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
        top                = MAX(top, labelSize.height);
        //rectangle(frame, cv::Point(left, top - int(1.5 * labelSize.height)), cv::Point(left + int(1.5 * labelSize.width), top + baseLine), cv::Scalar(0, 255, 0), FILLED);
        putText(img, label, cv::Point2f(left, top), cv::FONT_HERSHEY_SIMPLEX, 1, _colors[result[i].id], 2);
    }
    cv::addWeighted(img, 0.5, mask, 0.5, 0, img); //add mask to src
}
