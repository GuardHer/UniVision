#pragma once

#include "UniBaseOnnxDetect.h"
#include <algorithm>
#include <stdexcept>
#include <vector>

class UniPureOnnxDetect : public UniBaseOnnxDetect
{
public:
    UniPureOnnxDetect(const std::vector<std::string> &classes, std::vector<cv::Scalar> colors, const AlgotithmParam& param);
    virtual ~UniPureOnnxDetect();

public:
    void detect(const cv::Mat &src, std::vector<OutputParams> &results, cv::Mat &dstImg) const override;

private:
    bool preprocess(const cv::Mat &src, cv::Mat &dst) const override;
    void postprocess(std::vector<Ort::Value> &output_tensors,
                     std::vector<OutputParams> &results,
                     cv::Mat &dstImg) const override;

private:


private:
    std::vector<std::string> _classes; //class name
    std::vector<cv::Scalar> _colors;   //class color
};
