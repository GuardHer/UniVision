#pragma once
#include <iostream>
#include <memory>
#include <opencv2/opencv.hpp>
#include "UniAlgorithmUtils.h"
#include <onnxruntime_cxx_api.h>

#include "UniBaseOnnxDetect.h"

class YoloSegOnnxDetect : public UniBaseOnnxDetect
{
public:
    YoloSegOnnxDetect(const std::vector<std::string> &classes,
                      std::vector<cv::Scalar> colors,
                      const AlgotithmParam &param);
    virtual ~YoloSegOnnxDetect();

public:
    void detect(const cv::Mat &src, std::vector<OutputParams> &results, cv::Mat &dstImg) const override;

private:
    bool preprocess(const cv::Mat &src, cv::Mat &dst) const override;
    void postprocess(std::vector<Ort::Value> &output_tensors,
                     std::vector<OutputParams> &results,
                     cv::Mat &dstImg) const override;

private:
    void drawPred(cv::Mat &img, std::vector<OutputParams> result) const;

private:
    std::vector<std::string> _classes; //class name
    std::vector<cv::Scalar> _colors;   //class color

    float _maskThreshold = 0.5f;

    mutable cv::Vec4d _params;
};
