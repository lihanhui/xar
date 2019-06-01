#ifndef XAR_RECOGNITION_FEATURE
#define XAR_RECOGNITION_FEATURE

#include <opencv2/opencv.hpp>

namespace xar{
    struct feature{
       void detect(cv::Mat &image, std::vector<>cv::KeyPoint> & kps);
       void detect_origin(cv::Mat &image, std::vector<>cv::KeyPoint> & kps); // scale
              
       void compute(cv::Mat &image, std::vector<cv::KeyPoint> & kps, cv::Mat & descriptors); 

};

};
#endif
