#pragma once
#include <iostream>
#include <numeric>
#include <vector>
#include <opencv2/opencv.hpp>
#include <io.h>

#define ORT_OLD_VISON 13 //ort1.12.0 之前的版本为旧版本API

struct PoseKeyPoint
{
    float x          = 0;
    float y          = 0;
    float confidence = 0;
};

struct OutputParams
{
    int id;                              //结果类别id
    int area;                            //结果面积
    float confidence;                    //结果置信度
    cv::Rect box;                        //矩形框
    cv::RotatedRect rotatedBox;          //obb结果矩形框
    cv::Mat boxMask;                     //矩形框内mask，节省内存空间和加快速度
    std::vector<PoseKeyPoint> keyPoints; //pose key points
};
struct MaskParams
{
    //int segChannels = 32;
    //int segWidth = 160;
    //int segHeight = 160;
    int netWidth        = 640;
    int netHeight       = 640;
    float maskThreshold = 0.5;
    cv::Size srcImgShape;
    cv::Vec4d params;
};

/// <summary>
/// 检查模型路径是否存在
/// </summary>
/// <param name="modelPath"></param>
/// <returns></returns>
bool CheckModelPath(std::string modelPath);

/// <summary>
/// 检查参数是否合法
/// </summary>
/// <param name="netHeight"></param>
/// <param name="netWidth"></param>
/// <param name="netStride"></param>
/// <param name="strideSize"></param>
/// <returns></returns>
bool CheckParams(int netHeight, int netWidth, const int *netStride, int strideSize);

/// <summary>
/// 绘制检测结果
/// </summary>
/// <param name="img"></param>
/// <param name="result"></param>
/// <param name="classNames"></param>
/// <param name="color"></param>
/// <param name="isVideo"></param>
void DrawPred(cv::Mat &img,
              std::vector<OutputParams> result,
              std::vector<std::string> classNames,
              std::vector<cv::Scalar> color,
              bool isVideo = false);

void DrawRotatedBox(cv::Mat &srcImg, cv::RotatedRect box, cv::Scalar color, int thinkness);
void LetterBox(const cv::Mat &image,
               cv::Mat &outImage,
               cv::Vec4d &params, //[ratio_x,ratio_y,dw,dh]
               const cv::Size &newShape = cv::Size(640, 640),
               bool autoShape           = false,
               bool scaleFill           = false,
               bool scaleUp             = true,
               int stride               = 32,
               const cv::Scalar &color  = cv::Scalar(114, 114, 114));
void GetMask(const cv::Mat &maskProposals,
             const cv::Mat &maskProtos,
             std::vector<OutputParams> &output,
             const MaskParams &maskParams);
void GetMask2(const cv::Mat &maskProposals,
              const cv::Mat &maskProtos,
              OutputParams &output,
              const MaskParams &maskParams);
int BBox2Obb(float centerX, float centerY, float boxW, float boxH, float angle, cv::RotatedRect &rotatedRect);

void GenerateColor(std::vector<cv::Scalar> &color, int num);


template<typename T> inline T VectorProduct(const std::vector<T> &v)
{
    return std::accumulate(v.begin(), v.end(), 1, std::multiplies<T>());
};


class PurePostProcessor
{
public:
    struct Component
    {
        int global_label;
        int channel;
        int original_label;
        int size;
    };

    int N_CLASSES                       = 4;
    static const int MIN_COMPONENT_SIZE = 20;

    PurePostProcessor(int n_classes, const std::vector<float> &p_thresh, const std::vector<int> &min_class_sizes);

    std::vector<cv::Mat> process(const std::vector<cv::Mat> &probabilities);

    // 处理概率图，生成单一掩码
    cv::Mat postprocess(const std::vector<cv::Mat> &probabilities, std::vector<Component>& infos);

    void sigmoid(float *data, int batch, int channels, int height, int width);

private:
    

    std::vector<float> p_thresh;
    std::vector<int> min_class_sizes;

    std::vector<cv::Mat> componentDomination(const std::vector<cv::Mat> &preds);

    std::pair<cv::Mat, std::vector<Component>> postcomponentDomination(const std::vector<cv::Mat> &preds);

    cv::Mat postwritePreds(const std::vector<cv::Mat> &channel_components,
                           const std::vector<Component> &components_info);

    std::pair<std::vector<cv::Mat>, std::vector<Component>> findComponents(const std::vector<cv::Mat> &preds);

    std::vector<cv::Mat> writePreds(const std::vector<cv::Mat> &channel_components,
                                    const std::vector<Component> &components_info);

    std::vector<float> setupPThresh(const std::vector<float> &p_thresh);

    std::vector<int> setupMinClassSizes(const std::vector<int> &min_sizes);
};