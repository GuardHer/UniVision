#include "UniOnnxModel.h"

#include <iostream>
#include <mutex>

#include "core/UniLog.h"
#include "UniAlgorithmUtils.h"

UniOnnxModel::UniOnnxModel(const std::string &modelPath) :
    _modelPath(modelPath),
    _OrtMemoryInfo(Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtDeviceAllocator, OrtMemType::OrtMemTypeCPUOutput))
{
    _OrtEnv = Ort::Env(OrtLoggingLevel::ORT_LOGGING_LEVEL_ERROR, _modelPath.c_str());
}

UniOnnxModel::~UniOnnxModel()
{
}
bool UniOnnxModel::loadModel(bool isCuda, int cudaID, bool warmUp)
{
    try {
        if (0 != _access(_modelPath.c_str(), 0)) {
            LOG_INFO("Model path does not exist, please check {}", _modelPath);
            return false;
        }

        std::vector<std::string> available_providers = Ort::GetAvailableProviders();
        auto cuda_available =
        std::find(available_providers.begin(), available_providers.end(), "CUDAExecutionProvider");

        if (isCuda && (cuda_available == available_providers.end())) {
            LOG_INFO("Your ORT build without GPU. Change to CPU.");
            LOG_INFO("Infer model on CPU!");
        }
        else if (isCuda && (cuda_available != available_providers.end())) {
            LOG_INFO("Infer model on GPU!");
#if ORT_API_VERSION < ORT_OLD_VISON
            OrtCUDAProviderOptions cudaOption;
            cudaOption.device_id = cudaID;
            _OrtSessionOptions.AppendExecutionProvider_CUDA(cudaOption);
#else
            OrtStatus *status = OrtSessionOptionsAppendExecutionProvider_CUDA(_OrtSessionOptions, cudaID);
#endif
        }
        else {
            std::cout << "************* Infer model on CPU! *************" << std::endl;
        }

        _OrtSessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

#ifdef _WIN32
        std::wstring model_path(_modelPath.begin(), _modelPath.end());
        _OrtSession = new Ort::Session(_OrtEnv, model_path.c_str(), _OrtSessionOptions);
#else
        _OrtSession = new Ort::Session(_OrtEnv, _modelPath.c_str(), _OrtSessionOptions);
#endif

        Ort::AllocatorWithDefaultOptions allocator;

        // 初始化输入
        _inputNodesNum = _OrtSession->GetInputCount();
#if ORT_API_VERSION < ORT_OLD_VISON
        _inputName = _OrtSession->GetInputName(0, allocator);
        _inputNodeNames.push_back(_inputName);
#else
        _inputName  = std::move(_OrtSession->GetInputNameAllocated(0, allocator));
        _inputNodeNames.push_back(_inputName.get());
        LOG_INFO("Onnx Input Node name: {}", _inputName.get());
#endif

        Ort::TypeInfo inputTypeInfo = _OrtSession->GetInputTypeInfo(0);
        auto input_tensor_info      = inputTypeInfo.GetTensorTypeAndShapeInfo();
        _inputNodeDataType          = input_tensor_info.GetElementType();
        _inputTensorShape           = input_tensor_info.GetShape();

        if (_inputTensorShape[0] == -1) {
            _isDynamicShape      = true;
            _inputTensorShape[0] = _batchSize;
        }
        if (_inputTensorShape[2] == -1 || _inputTensorShape[3] == -1) {
            _isDynamicShape      = true;
            _inputTensorShape[2] = _netHeight;
            _inputTensorShape[3] = _netWidth;
        }

        // 初始化输出
        _outputNodesNum = _OrtSession->GetOutputCount();

        if (_outputNodesNum == 1) {
            // 单输出情况（如 UNet）
            _output_name0 = std::move(_OrtSession->GetOutputNameAllocated(0, allocator));
            _outputNodeNames.push_back(_output_name0.get());
            Ort::TypeInfo type_info_output0 = _OrtSession->GetOutputTypeInfo(0);
            auto tensor_info_output0        = type_info_output0.GetTensorTypeAndShapeInfo();
            _outputNodeDataType             = tensor_info_output0.GetElementType();
            _outputTensorShape              = tensor_info_output0.GetShape();
        }
        else if (_outputNodesNum == 2) {
            // 双输出情况（如 YOLO）
            _output_name0 = std::move(_OrtSession->GetOutputNameAllocated(0, allocator));
            _output_name1 = std::move(_OrtSession->GetOutputNameAllocated(1, allocator));
            Ort::TypeInfo type_info_output0(nullptr);
            Ort::TypeInfo type_info_output1(nullptr);
            if (strcmp(_output_name0.get(), _output_name1.get()) < 0) {
                _outputNodeNames.push_back(_output_name0.get());
                _outputNodeNames.push_back(_output_name1.get());
                type_info_output0 = _OrtSession->GetOutputTypeInfo(0);
                type_info_output1 = _OrtSession->GetOutputTypeInfo(1);
            }
            else {
                _outputNodeNames.push_back(_output_name1.get());
                _outputNodeNames.push_back(_output_name0.get());
                type_info_output0 = _OrtSession->GetOutputTypeInfo(1);
                type_info_output1 = _OrtSession->GetOutputTypeInfo(0);
            }
            auto tensor_info_output0 = type_info_output0.GetTensorTypeAndShapeInfo();
            _outputNodeDataType      = tensor_info_output0.GetElementType();
            _outputTensorShape       = tensor_info_output0.GetShape();
            // 可选：如果需要处理第二个输出的形状，可以使用 type_info_output1
        }
        else {
            std::cout << "Error: Unsupported number of outputs: " << _outputNodesNum << std::endl;
            return false;
        }

        // 预热
        if (isCuda && warmUp) {
            std::cout << "Start warming up" << std::endl;
            size_t input_tensor_length = VectorProduct(_inputTensorShape);
            float *temp                = new float[input_tensor_length];
            std::vector<Ort::Value> input_tensors;
            std::vector<Ort::Value> output_tensors;
            input_tensors.push_back(Ort::Value::CreateTensor<float>(
            _OrtMemoryInfo, temp, input_tensor_length, _inputTensorShape.data(), _inputTensorShape.size()));
            for (int i = 0; i < 3; ++i) {
                output_tensors =
                _OrtSession->Run(Ort::RunOptions{nullptr}, _inputNodeNames.data(), input_tensors.data(),
                                 _inputNodeNames.size(), _outputNodeNames.data(), _outputNodeNames.size());
            }
            delete[] temp;
        }
    }
    catch (const std::exception &e) {
        std::cout << "YoloSegOnnxDetect Error:" << e.what() << std::endl;
        return false;
    }
    return true;
}

bool UniOnnxModel::infer(const std::vector<Ort::Value> &input_tensors, std::vector<Ort::Value> &output_tensors)
{
    try
    {
        std::lock_guard<std::mutex> lock(_mutex);

        output_tensors = _OrtSession->Run(Ort::RunOptions{nullptr}, _inputNodeNames.data(), input_tensors.data(),
                                          _inputNodeNames.size(), _outputNodeNames.data(), _outputNodeNames.size());

        return true;
    }
    catch (...)
    {
        LOG_ERROR("UniOnnxModel::infer: inference failed.");
        return false;
    }
    
}
