#ifndef XAR_UTIL_DRAW
#define XAR_UTIL_DRAW

#include <opencv2/opencv.hpp>
//using namespace cv;
using PointPtr = cv::Point*;
namespace xar{
    struct draw{
        static void polylines(cv::Mat & img, std::vector<cv::Point> & points, const cv::Scalar& color){
            int npts = points.size();
            PointPtr *pts = new PointPtr[npts];
            for(int i = 0; i < npts; i++){
                pts[i] = &points[i];
            }
            cv::polylines(img, pts, &npts, 1, true, color, 1);
            delete []pts;
        }
        static void rectangle(cv::Mat &img, cv::Point pt1, cv::Point pt2, const cv::Scalar &color) {
            cv::rectangle(img, pt1, pt2, color, 1, 8, 0);
        }
        static void text(cv::Mat &img, const std::string &text, cv::Point pt, int fontFace, double fontScale, const cv::Scalar &color){
            cv::putText(img, text, pt, fontFace, fontScale, color, 1, 8, false);
        }
    };
};

#endif