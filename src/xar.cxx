#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>

#include <doraemon/base64/base64.h>

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

std::string mat_base64_encode(const cv::Mat& m)
{
	int params[3] = {0};
	params[0] = CV_IMWRITE_JPEG_QUALITY;
	params[1] = 100;

	std::vector<uchar> buf;
	bool code = cv::imencode(".jpg", m, buf, std::vector<int>(params, params+2));
	uchar* result = reinterpret_cast<uchar*> (&buf[0]);

	return doraemon::base64::encode(result, buf.size());

}



cv::Mat mat_base64_decode(const std::string& s)
{
	// Decode data
	std::string decoded_string = doraemon::base64::decode(s);
	std::vector<uchar> data(decoded_string.begin(), decoded_string.end());

	cv::Mat img = imdecode(data, cv::IMREAD_UNCHANGED);
	return img;
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
        std::cout<<mat_base64_encode(frame)<<std::endl;
        break;
        cv::Mat out_frame;
        freak(frame, out_frame);
        cv::imshow("xar", out_frame);
        if( cv::waitKey(50) == 27 ){
            break;
        }
    }
    return 0;
}
