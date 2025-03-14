#pragma once

#include <vector>
#include <string>
#include <memory>

#include <opencv2/opencv.hpp>
#include <deploy/model.hpp>
#include <deploy/option.hpp>
#include <deploy/result.hpp>

struct YoloTRTPara
{
	std::vector<std::string> _labels; // 标签
	std::string _enginePath;          // 模型路径
	float _confidenceThreshold{ 0.5f }; // 置信度阈值
};

struct YoloTRTOutput
{
	deploy::SegmentRes _result; // 检测结果
	std::tuple<std::string, std::string, std::string> _performance; // 性能信息
};

class YoloTRTDetect
{
public:
	YoloTRTDetect(const YoloTRTPara& para, std::unique_ptr<deploy::SegmentModel> _model);
	~YoloTRTDetect();

	YoloTRTOutput process_single_image(const cv::Mat& src_img, cv::Mat& dst_img) const;

	std::vector<YoloTRTOutput> process_batch_images(const std::vector<cv::Mat>& src_imgs, std::vector<cv::Mat>& dst_imgs) const;

private:
	void visualize(cv::Mat& image, deploy::SegmentRes& result) const;

private:
	YoloTRTPara _para;

	std::unique_ptr<deploy::SegmentModel> _model;
};

using YoloTRTDetectPtr = std::shared_ptr<YoloTRTDetect>;

