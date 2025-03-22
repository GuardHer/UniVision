#pragma once

#include <string>
#include <mutex>
#include <onnxruntime_cxx_api.h>

class UniOnnxModel
{
    friend class UniBaseOnnxDetect;
    friend class UniPureOnnxDetect;

public:
    UniOnnxModel(const std::string &modelPath);
    ~UniOnnxModel();

    bool loadModel(bool isCuda = false, int cudaID = 0, bool warmUp = true);

    bool infer(const std::vector<Ort::Value> &input_tensors, std::vector<Ort::Value> &output_tensors);

private:
    std::string _modelPath;

public:
    Ort::Env _OrtEnv;
    Ort::SessionOptions _OrtSessionOptions = Ort::SessionOptions();
    Ort::Session *_OrtSession              = nullptr;
    Ort::MemoryInfo _OrtMemoryInfo;

#if ORT_API_VERSION < ORT_OLD_VISON
    char *_inputName, *_output_name0, *_output_name1;
#else
    std::shared_ptr<char> _inputName, _output_name0, _output_name1;
#endif
    int _batchSize       = 1;                      //if multi-batch,set this
    bool _isDynamicShape = false;                  //onnx support dynamic shape
    const int _netWidth  = 640;                    //ONNX-net-input-width
    const int _netHeight = 640;                    //ONNX-net-input-height
    std::vector<char *> _inputNodeNames;           //输入节点名
    std::vector<char *> _outputNodeNames;          //输出节点名
    size_t _inputNodesNum  = 0;                    //输入节点数
    size_t _outputNodesNum = 0;                    //输出节点数
    ONNXTensorElementDataType _inputNodeDataType;  //输入数据类型
    ONNXTensorElementDataType _outputNodeDataType; //输出数据类型
    std::vector<int64_t> _inputTensorShape;        //输入张量shape
    std::vector<int64_t> _outputTensorShape;       //输出张量shape
    std::vector<int64_t> _outputMaskTensorShape;   //输出张量shape for mask


    std::mutex _mutex;
};
