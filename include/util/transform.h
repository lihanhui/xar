#ifndef XAR_UTIL_TRANSFORM
#define XAR_UTIL_TRANSFORM

#include <opencv2/opencv.hpp>

namespace xar
{
struct transform
{
    static cv::Mat rectangle(cv::Mat &img, cv::Point point1, cv::Point point2){
        cv::Rect rect(point1.x, point1.y, point2.x - point1.x, point2.y - point1.y);
        cv::Mat roi;
        img(rect).copyTo(roi);
        return roi;
    }
    static cv::Mat rectangle(cv::Mat &img, cv::Rect &rect){
        cv::Mat roi;
        img(rect).copyTo(roi);
        return roi;
    }
};
}; // namespace xar

#endif