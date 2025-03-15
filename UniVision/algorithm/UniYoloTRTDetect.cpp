#include "Template.h"
#include "UniYoloTRTDetect.h"

#include "core/UniLog.h"

YoloTRTDetect::YoloTRTDetect(const YoloTRTPara& para, std::unique_ptr<deploy::SegmentModel> _model)
	: _para(para), _model(std::move(_model))
{
}

YoloTRTDetect::~YoloTRTDetect()
{
	_model.reset();
}

YoloTRTOutput YoloTRTDetect::process_single_image(const cv::Mat& src_img, cv::Mat& dst_img) const
{
    YoloTRTOutput output;

	deploy::Image img(src_img.data, src_img.cols, src_img.rows);
	deploy::SegmentRes result = _model->predict(img);
	dst_img = src_img.clone();
	visualize(dst_img, result);

    output._result = std::move(result);
	output._performance = _model->performanceReport();
	auto [throughput_str, gpu_latency_str, cpu_latency_str] = output._performance;
	
	LOG_DEBUG("YoloTRTDetect::process_single_image: {}, {}, {}", throughput_str, gpu_latency_str, cpu_latency_str);

	return output;
}

std::vector<YoloTRTOutput> YoloTRTDetect::process_batch_images(const std::vector<cv::Mat>& src_imgs, std::vector<cv::Mat>& dst_imgs) const
{
	std::vector<YoloTRTOutput> outputs;
    const int batch_size = _model->batch_size();

	for (size_t i = 0; i < src_imgs.size(); i += batch_size) {
		std::vector<cv::Mat>       images;
		std::vector<deploy::Image> img_batch;
		std::vector<std::string>   img_name_batch;
		for (size_t j = i; j < i + batch_size && j < src_imgs.size(); ++j) {
			cv::Mat image = src_imgs[j];
			images.push_back(image);
			img_batch.emplace_back(image.data, image.cols, image.rows);
		}
		auto results = _model->predict(img_batch);
		for (size_t j = 0; j < images.size(); ++j) {
			cv::Mat dst_img = images[j].clone();
			visualize(dst_img, results[j]);
			dst_imgs.push_back(dst_img);
		}
		for (size_t j = 0; j < images.size(); ++j) {
			YoloTRTOutput output;
			output._result = std::move(results[j]);
			output._performance = _model->performanceReport();
			outputs.push_back(std::move(output));

			//auto [throughput_str, gpu_latency_str, cpu_latency_str] = output._performance;

			//LOG_DEBUG("YoloTRTDetect::process_single_image {}: {}, {}, {}", j, throughput_str, gpu_latency_str, cpu_latency_str);
		}
	}

    return outputs;
}

void YoloTRTDetect::visualize(cv::Mat& image, deploy::SegmentRes& result) const
{
    // 遍历每个检测到的目标
    for (size_t i = 0; i < result.num; ++i) {
        auto& box = result.boxes[i];           // 当前目标的边界框
        int         cls = result.classes[i];   // 当前目标的类别
        float       score = result.scores[i];  // 当前目标的置信度
		if (score < _para._confidenceThreshold) {
			continue;
		}
		if (_para._labels.size() <= cls) {
			continue;
		}
        auto label = _para._labels[cls];      // 获取类别对应的标签
        std::string label_text = label + " " + cv::format("%.3f", score);  // 构造显示的标签文本

        // 绘制边界框和标签
        int      base_line;
        cv::Size label_size = cv::getTextSize(label_text, cv::FONT_HERSHEY_SIMPLEX, 0.6, 1, &base_line);
        cv::rectangle(image, cv::Point(box.left, box.top), cv::Point(box.right, box.bottom), cv::Scalar(251, 81, 163), 2, cv::LINE_AA);
        cv::rectangle(image, cv::Point(box.left, box.top - label_size.height), cv::Point(box.left + label_size.width, box.top), cv::Scalar(125, 40, 81), -1);
        cv::putText(image, label_text, cv::Point(box.left, box.top), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(253, 168, 208), 1);

        // 创建分割掩码
        cv::Mat mask(result.masks[i].height, result.masks[i].width, CV_8UC1, result.masks[i].data.data());
        cv::resize(mask, mask, image.size());  // 将掩码调整到图像大小

        // 创建边界框区域的二值掩码
        cv::Mat box_mask = cv::Mat::zeros(image.size(), CV_8UC1);
        cv::rectangle(box_mask, cv::Point(box.left, box.top), cv::Point(box.right, box.bottom), cv::Scalar(1), cv::FILLED);

        // 将分割掩码与边界框掩码结合
        mask &= box_mask;

        // 在掩码区域内将分割颜色与图像混合
        for (int y = 0; y < mask.rows; ++y) {
            cv::Vec3b* image_row = image.ptr<cv::Vec3b>(y);  // 当前行的图像指针
            const uchar* mask_row = mask.ptr<uchar>(y);       // 当前行的掩码指针
            for (int x = 0; x < mask.cols; ++x) {
                if (mask_row[x]) {                             // 如果掩码值为1
                    // 混合颜色（半透明效果）
                    image_row[x][0] = static_cast<uchar>(image_row[x][0] * 0.5 + 253 * 0.5);
                    image_row[x][1] = static_cast<uchar>(image_row[x][1] * 0.5 + 168 * 0.5);
                    image_row[x][2] = static_cast<uchar>(image_row[x][2] * 0.5 + 208 * 0.5);
                }
            }
        }
    }

	// draw text "OK" or "NG", right top, ok is green, ng is red
	std::string str_res = result.num ? "NG" : "OK";
	cv::Scalar color = result.num ? cv::Scalar(0, 0, 255) : cv::Scalar(0, 255, 0);
	cv::putText(image, str_res, cv::Point(image.cols - 100, 50), cv::FONT_HERSHEY_SIMPLEX, 10, color, 2);
}

