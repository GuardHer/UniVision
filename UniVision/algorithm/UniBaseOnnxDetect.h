#pragma once

#include <vector>
#include <string>
#include <memory>
#include <mutex>

#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>

#include "UniAlgorithmUtils.h"
#include "config/UniAlgorithmConfig.h"

class UniOnnxModel;

class UniBaseOnnxDetect
{
public:
    UniBaseOnnxDetect(const AlgotithmParam &param);
    virtual ~UniBaseOnnxDetect() = default;

    bool init(const std::string &modelPath, bool isCuda = false, int cudaID = 0, bool warmUp = true);

public:
    virtual void detect(const cv::Mat &src, std::vector<OutputParams> &results, cv::Mat &dstImg) const = 0;

private:
    virtual bool preprocess(const cv::Mat &src, cv::Mat &dst) const = 0;
    virtual void postprocess(std::vector<Ort::Value> &output_tensors, std::vector<OutputParams> &results, cv::Mat &dstImg) const = 0;

protected:
    std::unique_ptr<UniOnnxModel> _model;

    AlgotithmParam _param;
};
