#include <opencv2/opencv.hpp>

#include "recognition/feature.h"

using namespace xar;
void feature::detect(cv::Mat &image, std::vector<cv::KeyPoint > &kps){
    //cv::cvtColor(frame, out_frame, cv::COLOR_BGR2GRAY);
    cv::Ptr<cv::FeatureDetector> detector = cv::ORB::create(500);
    detector->detect(image, kps);
}

void feature::detect_origin(cv::Mat &image, std::vector<cv::KeyPoint> &kps){
    //cv::cvtColor(frame, out_frame, cv::COLOR_BGR2GRAY);
    cv::Ptr<cv::FeatureDetector> detector = cv::ORB::create(500, 1.0f, 0);
    detector->detect(image, kps);
}

void feature::compute(cv::Mat &image, std::vector<cv::KeyPoint> &kps, cv::Mat &descriptors){
    cv::Ptr<cv::FeatureDetector> orb = cv::ORB::create(500);
    orb->compute(image, kps, descriptors);
}
void feature::scale_keypoints(std::vector<cv::KeyPoint> &kps, std::unordered_map<int, std::vector<cv::KeyPoint>> &kp_map){
    auto scale = [&](const cv::KeyPoint &kp) {
        if (kp_map.find(kp.octave) == kp_map.end())
        {
            kp_map.insert({kp.octave, std::vector<cv::KeyPoint>()});
        }
        kp_map[kp.octave].push_back(kp);
    };
    std::for_each(kps.begin(), kps.end(), scale);
}
