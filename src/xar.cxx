#include <iostream>
#include <experimental/filesystem>
#include <bitset>
#include <chrono>
#include <thread>
#include <cmath>

#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <simple-web-server/client_http.hpp>
#include <faiss/Clustering.h>
#include <faiss/Index.h>
#include <faiss/IndexFlat.h>

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
int compute(const cv::Mat & frame, cv::Mat &out_frame)
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
        compute(frame, out_frame);
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
int main3(int argc, char* argv[] ) { 
	Mat frame; 
	VideoCapture capture; 
	if(argc > 1) {
		string path = argv[1];
		std::cout<<"try to open file "<<path<<std::endl;
		capture = VideoCapture(argv[1]);// path 为视频文件的路径 
	}else{
		std::cout<<"try to open camera"<<std::endl;
		capture = VideoCapture(0);
	}
	while (1) { 
		capture >> frame;
		if (frame.empty()) {
                       cout << "|------视频已读完！------|\n"; break;
                }

		cv::Mat out_frame;
		compute(frame, out_frame);
		for(int i = 0; i < out_frame.rows; ++i){
			for(int j = 0; j < out_frame.cols; ++j){
				std::cout<<(unsigned int)out_frame.at<unsigned char>(i, j)<<" ";
			}
			std::cout<<std::endl;
		}
		std::cout<<out_frame.rows<<":"<<out_frame.cols<<std::endl; 
		//imshow("视频",frame); waitKey(20); // 不加 waitKey() 视频是播放不出来的, 里面的参数单位为 ms, 为每帧之间的间隔时间 
	}	 
	capture.release(); 
	return 0;
}
void tofloat(float *dest, Mat & frame ){
	for(int i = 0; i < frame.rows; ++i){
		for(int j = 0; j < frame.cols; ++j){
			std::bitset<8> bits = (unsigned int)frame.at<unsigned char>(i, j);
			for(int k = 0; k < 8; ++k){
				if(bits.test(7-k)) {
					*dest = 1.0;
				} else{
					*dest = 0.0;
				}
				++dest;
			}
		}
	}
}
const int DIM = 256;
const int CENTROIDS = 8192;
static faiss::IndexFlatL2 img_index(DIM);
static faiss::IndexFlatL2 centroid_index(DIM);
static float * img_data = nullptr;
static float * centroids = nullptr;
static faiss::Clustering *clus = nullptr;
float totrain(size_t d, size_t n, size_t k,
		const float *x,
		float *centroids){
	clus = new faiss::Clustering(d, k);
	clus->verbose = d * n * k > (1L << 30);
	//img_index.metric_type = faiss::METRIC_L2;
	clus->train(n, x, img_index);
	memcpy(centroids, clus->centroids.data(), sizeof(*centroids) * d * k);
	return 0.0;

}
void train(std::vector<std::string> imgs){
	img_data = new float[imgs.size() * 500 * 32 * 8];
	memset(img_data, 0, sizeof(float) * imgs.size() * 500 * 32 * 8);
	float * tmp = img_data;
	int number = 0;
	for(auto img: imgs){
		Mat frame, gray_frame, out_frame;
		frame = imread(img);
		//cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
		compute(frame, out_frame);
		tofloat(tmp, out_frame);
		tmp += out_frame.rows * out_frame.cols * 8;
		number += out_frame.rows;		
	}
	number = number / DIM * DIM;
	centroids = new float[DIM * CENTROIDS];
	std::cout<<"imgs.size() "<<imgs.size()<< " | vectors = " << number<<std::endl;
	totrain(DIM, number, CENTROIDS, img_data, centroids);
	/*for(int i = 0; i < CENTROIDS; ++i){
		for(int j = 0; j < DIM; ++j){
			std::cout<< *(centroids + i * DIM + j)<<" "; 
		}
		std::cout<<std::endl;
	}//*/
	centroid_index.add(CENTROIDS, centroids);
}
namespace fs = std::experimental::filesystem;
int main(int argc, char* argv[] ) {
	if(argc < 4){
		return -1;
	}
	std::vector<std::string> imgs;
	string path = argv[1];
	string img  = argv[2];
        string lookup = argv[3];	
	for(auto p: fs::directory_iterator(path)){
		string file = p.path().string() ;
		imgs.push_back(file);
	}
	imgs.push_back(img);
	train(imgs);
        std::cout<<"training finished"<<std::endl;	
	faiss::IndexFlatL2 final_index(CENTROIDS); 
	Mat frame, gray_frame, out_frame;
	for(auto img: imgs){
		frame = imread(img);
		//cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
		compute(frame, out_frame);
		float * img_data  = new float[out_frame.rows * 32 * 8];
		float * distances = new float[out_frame.rows];
		faiss::Index::idx_t * labels    = new faiss::Index::idx_t[out_frame.rows];
		tofloat(img_data, out_frame);
	
        	centroid_index.search(out_frame.rows, img_data, 1, distances, labels);
                float * xb = new float[ CENTROIDS ];
		memset(xb, 0, sizeof(float) * CENTROIDS) ;
		for(int i = 0; i < out_frame.rows; ++i){
			//std::cout<<"distance: " << distances[i] << " label "<< labels[i] <<std::endl;
			if( labels[i] >= 0  ){
				xb[labels[i]] = xb[labels[i]] + 1;
			}
		}
		final_index.add(1, xb);
	}
	std::cout<<"imgs preparing finished: we will looking up "<<imgs.size() - 1<<std::endl;
	frame = imread(lookup);
	compute(frame, out_frame);
	float * img_data  = new float[out_frame.rows * 32 * 8];
        tofloat(img_data, out_frame);
	float * distances = new float[out_frame.rows];
	faiss::Index::idx_t * labels    = new faiss::Index::idx_t[out_frame.rows];
	centroid_index.search(out_frame.rows, img_data, 1, distances, labels);

        float * xq = new float[ CENTROIDS ];
        memset(xq, 0, sizeof(float) * CENTROIDS) ;
        for(int i = 0; i < out_frame.rows; ++i){
		if( labels[i] >= 0 ){
			xq[labels[i]] = xq[labels[i]] + 1;
		}
	}
	float square_sum = 0;
	for(int i = 0; i < CENTROIDS; ++i){
		if(labels[i] >= 0) square_sum += pow(xq[i], 2);
	}//*/	
	long I = -1;
	float D = -1;
	final_index.search(1, xq, 1, &D, &I);
	std::cout<<"D: "<< D << " I: " << I<< " sqrt: " << sqrt(square_sum) <<std::endl;


	std::cout<<"finished"<<std::endl;
	//std::this_thread::sleep_for(std::chrono::seconds(15));
        delete [] img_data; 
        delete [] centroids; 
}
