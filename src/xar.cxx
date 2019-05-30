#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>
#include <simple-web-server/client_http.hpp>

#include <doraemon/base64/base64.h>

#include "xar.h"

#define DEFINE_CASE(ns, s) return {#ns "::" #s, #s};
#define FORALL_NS_SYMBOLS(_) _(std, cout) 

using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

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
    //ORB detector with pyramid but AGAST NO!! cv::AgastFeatureDetector::create(20);//
    cv::Ptr<cv::FeatureDetector> detector = cv::ORB::create(500);

    std::vector<cv::KeyPoint> keypoints_object;
    detector->detect(frame, keypoints_object);
    for(int i = 0; i < keypoints_object.size(); i++){
        //std::cout<<keypoints_object[i].size<<" - "<<keypoints_object[i].octave<<std::endl;
    }
    cv::drawKeypoints(frame, keypoints_object, out_frame);
    //std::cout<<keypoints_object.size()<<std::endl;

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
void request(HttpClient & client, const std::string & frame){
  std::string json_string = "{\"frameId\": \"John\",\"frame\": \"\"$$$$\"\"}";
  std::string::size_type pos = json_string.find("\"$$$$\"");
  json_string.replace(pos, std::string("\"$$$$\"").length(), frame);
  // Synchronous request examples
  try {
     
    auto r2 = client.request("POST", "/api1/mysky/cloud/ar/xxxx", json_string);
    std::cout << r2->content.rdbuf() << std::endl;
  }
  catch(const SimpleWeb::system_error &e) {
    std::cerr << "Client request error: " << e.what() << std::endl;
}
}
std::vector<cv::Point> caculateHexagon(int width, int height){
    std::vector<cv::Point> points;
    int roi_edge = width > height ? height/2: width/2;
    cv::Point center(width / 2, height / 2);

    points.push_back(cv::Point(center.x - roi_edge / 2, center.y - roi_edge / 2));
    points.push_back(cv::Point(center.x - roi_edge / 2, center.y + roi_edge / 2));
    points.push_back(cv::Point(center.x, center.y + roi_edge*3/4));

    points.push_back(cv::Point(center.x + roi_edge / 2, center.y + roi_edge / 2));
    points.push_back(cv::Point(center.x + roi_edge / 2, center.y - roi_edge / 2));
    points.push_back(cv::Point(center.x, center.y - roi_edge * 3 / 4));
    return points;
}
int main(){
    //test();
    HttpClient client("localhost:8080");

    cv::VideoCapture capture(0);
    if(!capture.isOpened()){
        std::cout<<"Error opening webcam. Exiting."<<std::endl;
        return -1;
    }
    cv::namedWindow("xar", cv::WINDOW_AUTOSIZE);
    
    int key = -1;
    int count = 0;
    while(key == -1){
        cv::Mat frame;
        
        if(!capture.read(frame)){
            break;
        }
        std::cout<<frame.rows<<" - "<<frame.cols<<std::endl;
        ++count;
        if(count % 10 == 0){
            std::string frame_en = mat_base64_encode(frame);
            //std::cout<<frame_en<<std::endl;
            request(client, frame_en);
            count = 0;
        }
        //break;
        cv::Mat out_frame;
        freak(frame, out_frame);
        //break;
        cv::Point start(10,10);
        cv::Point end(out_frame.cols - 10, out_frame.rows - 10);
        cv::Scalar color(0, 255, 0);
        auto pts = caculateHexagon(out_frame.cols, out_frame.rows);
        xar::draw::polylines(out_frame, pts, color);
        //xar::draw::rectangle(out_frame, start, end, color);
        cv::imshow("xar", out_frame);
        if( cv::waitKey(50) == 27 ){
            break;
        }
    }
    return 0;
}
