#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>

#define DEFINE_CASE(ns, s) return {#ns "::" #s, #s};
#define FORALL_NS_SYMBOLS(_) _(std, cout) 

std::pair<const char*, const char*> experimental(){
    FORALL_NS_SYMBOLS(DEFINE_CASE)
}
void test(){
    auto pair = experimental();
    std::cout<<pair.first<<":"<<pair.second<<std::endl;
}
int freak(const cv::Mat & frame, cv::Mat &out_frame)
{
    cv::cvtColor(frame, out_frame, cv::COLOR_BGR2GRAY);
    cv::Ptr<cv::FeatureDetector> detector = cv::ORB::create(5000);

    std::vector<cv::KeyPoint> keypoints_object;
    detector->detect(frame, keypoints_object);
    
    cv::drawKeypoints(frame, keypoints_object, out_frame);
    

    return 0;
}

int main(){
    test();
    cv::VideoCapture capture(0);
    if(!capture.isOpened()){
        std::cout<<"Error opening webcam. Exiting."<<std::endl;
        return -1;
    }
    cv::namedWindow("xar", cv::WINDOW_AUTOSIZE);
    
    int key = -1;
    while(key == -1){
        cv::Mat frame;
        
        if(!capture.read(frame)){
            break;
        }
        cv::Mat out_frame;
        freak(frame, out_frame);
        cv::imshow("xar", out_frame);
        if( cv::waitKey(50) == 27 ){
            break;
        }
    }
    return 0;
}