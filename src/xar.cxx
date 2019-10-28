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
    detector->detectAndCompute(frame, cv::Mat(),  keypoints_object, out_frame);
    for(int i = 0; i < keypoints_object.size(); i++){
        //std::cout<<keypoints_object[i].size<<" - "<<keypoints_object[i].octave<<std::endl;
    }
    //cv::drawKeypoints(frame, keypoints_object, out_frame);
    std::cout<<keypoints_object.size()<<std::endl;

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
    int roi_edge = width > height ? 2*height/3: 2*width/3;
    cv::Point center(width / 2, height / 2);

    points.push_back(cv::Point(center.x - roi_edge / 2, center.y - 1.732 * roi_edge / 6));
    points.push_back(cv::Point(center.x - roi_edge / 2, center.y + 1.732 * roi_edge / 6));
    points.push_back(cv::Point(center.x, center.y + 1.732 * roi_edge / 3));

    points.push_back(cv::Point(center.x + roi_edge / 2, center.y + 1.732 * roi_edge / 6));
    points.push_back(cv::Point(center.x + roi_edge / 2, center.y - 1.732 * roi_edge / 6));
    points.push_back(cv::Point(center.x, center.y - 1.732 * roi_edge / 3));
    return points;
}

int main2(){
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
    int seconds = 0;
    while(key == -1){
        cv::Mat frame;
        
        if(!capture.read(frame)){
            break;
        }
        double fps = capture.get(CV_CAP_PROP_FPS);

        cv::Point center(frame.cols / 2, frame.rows / 2);
        int edge = frame.cols > frame.rows ? 2*frame.rows / 3 : 2*frame.cols / 3;

        //std::cout << frame.rows << " - " << frame.cols << " - " << fps <<" - "<<edge<< std::endl;

        cv::Scalar color(0, 255, 0);
        ++count;

        if(count % 12 == 0){
            cv::Mat target = xar::transform::rectangle(frame,
                                                       cv::Point(center.x - edge / 2, center.y - edge / 2),
                                                       cv::Point(center.x + edge / 2, center.y + edge / 2));
            cv::Mat imageGray; 
            cv::cvtColor(target, imageGray, CV_RGB2GRAY);
            cv::Mat imageSobel;
            cv::Sobel(imageGray, imageSobel, CV_16U, 1, 1);
            double meanValue = 0.0;
            meanValue = cv::mean(imageSobel)[0];
            std::cout<<meanValue<<std::endl;
            std::string frame_en = mat_base64_encode(target);
            //std::cout<<frame_en<<std::endl;
            xar::draw::text(frame, std::to_string(++seconds), cv::Point(frame.cols/ 2, frame.rows / 2), cv::FONT_HERSHEY_SIMPLEX, 2, color);
            request(client, frame_en);
            count = 0;
            if(seconds % 10 == 0){
                ;cv::imwrite(std::to_string(seconds++) + ".jpg", target);
            }
        }

        //break;
        cv::Mat out_frame;
        freak(frame, out_frame);
        //break;
        cv::Point start(10,10);
        cv::Point end(out_frame.cols - 10, out_frame.rows - 10);
        
        auto pts = caculateHexagon(out_frame.cols, out_frame.rows);
        xar::draw::polylines(out_frame, pts, color);
        //xar::draw::rectangle(out_frame, start, end, color);
        //
        cv::Mat target = xar::transform::rectangle(out_frame,
                                                       cv::Point(center.x - edge / 2, center.y - edge / 2),
                                                       cv::Point(center.x + edge / 2, center.y + edge / 2));
        cv::imshow("xar", target);
        if( cv::waitKey(50) == 27 ){
            break;
        }
    }
    return 0;
}
using namespace std; 
using namespace cv; 
int main() { 
	String path = "~/Downloads/2019-10-2.mov"; 
	Mat frame; VideoCapture capture(0); // path 为视频文件的路径 
	while (1) { 
		capture >> frame;
		cv::Mat out_frame;
		freak(frame, out_frame); 
		std::cout<<out_frame.rows<<":"<<out_frame.cols<<std::endl; 
		if (frame.empty()) { 
			cout << "|------视频已读完！------|\n"; break;
	 	} 
		imshow("视频",frame); waitKey(20); // 不加 waitKey() 视频是播放不出来的, 里面的参数单位为 ms, 为每帧之间的间隔时间 
	}	 
	capture.release(); 
	return 0;
}
