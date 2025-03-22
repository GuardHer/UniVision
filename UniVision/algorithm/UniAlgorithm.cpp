#include "UniAlgorithm.h"

#include <iostream>
#include <fstream>
#include <locale>
#include <codecvt>
#include <QFileInfo>
#include "core/UniThreadPool.h"
#include "core/UniLog.h"
#include "UniYoloTRTDetect.h"
#include "UniYoloOnnxDetect.h"
#include "UniPureOnnxDetect.h"

UniAlgorithm *UniAlgorithm::_instance = nullptr;
std::mutex UniAlgorithm::_mutex;

UniAlgorithm *UniAlgorithm::instance()
{
    if (_instance == nullptr) {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_instance == nullptr) {
            _instance = new UniAlgorithm();
        }
    }
    return _instance;
}

UniAlgorithm::UniAlgorithm()
{
}

UniAlgorithm::~UniAlgorithm()
{
}


void UniAlgorithm::setAlgorithmResultReadyCallback(const AlgorithmResultReadyCallback &callback)
{
    _resultReadyCallback = callback;
}

void UniAlgorithm::detect(const AlgorithmInput &input, AlgorithmOutput &output)
{
    output._cameraIndex = input._cameraIndex;
    output._imageIndex  = input._imageIndex;
    output._imageMark   = input._imageMark;
    std::vector<OutputParams> onnx_output;

    if (_config._detectType == AlgorithmDetectType::YOLO_TRT_SEG && _config._enableCuda) {
        YoloTRTPara para;
        para._labels              = generate_labels(_config._labelPath);
        para._colors              = _colors;
        para._confidenceThreshold = _config._param._confidenceThreshold;

        std::lock_guard<std::mutex> lock(_mutexDetect);
        std::unique_ptr<YoloTRTDetect> detect = std::make_unique<YoloTRTDetect>(para);

        cv::Mat dstImage;
        YoloTRTOutput result = detect->process_single_image(_modelTRT.get(), input._srcImage, dstImage);

        for (size_t i = 0; i < result._result.num; ++i) {
            auto &box   = result._result.boxes[i];   // 当前目标的边界框
            int cls     = result._result.classes[i]; // 当前目标的类别
            float score = result._result.scores[i];  // 当前目标的置信度

            AlgorithmOutput::Result res;
            res._score = score;
            try {
                res._class = para._labels[cls];
                res._rect  = cv::Rect(cv::Point(box.left, box.top), cv::Point(box.right, box.bottom));
            }
            catch (const std::exception &e) {
                res._class = "Unknown";
                LOG_WARN("exception: {}", e.what());
            }
            catch (...) {
                res._class = "Unknown";
                LOG_WARN("Unknown exception.");
            }

            output._results.push_back(std::move(res));
        }
        output._bResult  = !output._results.empty();
        output._dstImage = std::move(dstImage);
    }
    else if (_config._detectType == AlgorithmDetectType::YOLO_ONNX_SEG) {
        cv::Mat dstImage = input._srcImage.clone();
        _modelOnnx->detect(input._srcImage, onnx_output, dstImage);
        auto labels      = generate_labels(_config._labelPath);
        std::vector<cv::Scalar> color;
        GenerateColor(color, labels.size());

        output._dstImage = std::move(dstImage);
        output._bResult  = !onnx_output.empty();
    }
    else {
        std::cout << "UniAlgorithm has not been initialized." << std::endl;
        return;
    }

    if (_resultReadyCallback) {
        _resultReadyCallback(input, output);
    }
}

void UniAlgorithm::init(const AlgorithmConfig &config)
{
    if (_isInit) {
        LOG_WARN("UniAlgorithm has been initialized.");
        return;
    }

    _config         = config;
    auto modelFiles = _config.getVecModelName();

    if (modelFiles.empty()) {
        LOG_WARN("Model file is empty.");
        return;
    }
    GenerateColor(_colors, generate_labels(_config._labelPath).size());

    if (_config._detectType == AlgorithmDetectType::YOLO_TRT_SEG && _config._enableCuda) {
        deploy::InferOption option;
        option.enableSwapRB();
        option.enablePerformanceReport();

        _modelTRT = std::make_unique<deploy::SegmentModel>(modelFiles[0], option);

        LOG_INFO("YoloTRT {} loaded.", modelFiles[0]);
    }
    else if (_config._detectType == AlgorithmDetectType::YOLO_ONNX_SEG) {
        _modelOnnx = std::make_unique<YoloSegOnnxDetect>(generate_labels(_config._labelPath), _colors, _config._param);
        _modelOnnx->init(modelFiles[0], _config._enableCuda, _config._gpuIndex);
        LOG_INFO("YoloONNX {} loaded.", modelFiles[0]);
    }
    else if (_config._detectType == AlgorithmDetectType::PURE_ONNX_SEG) {
        _modelOnnx = std::make_unique<UniPureOnnxDetect>(generate_labels(_config._labelPath), _colors, _config._param);
        _modelOnnx->init(modelFiles[0], _config._enableCuda, _config._gpuIndex);
        LOG_INFO("PureSegONNX {} loaded.", modelFiles[0]);
    }
    else {
        LOG_WARN("Unsupported algorithm type or CUDA is disabled.");
    }

    _isInit = true;
}


std::vector<std::string> UniAlgorithm::generate_labels(const std::string &label_file)
{
    std::ifstream file(label_file);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open labels file: " + label_file);
    }

    std::vector<std::string> labels;
    std::string label;
    while (std::getline(file, label)) {
        labels.emplace_back(label);
    }
    return labels;
}
