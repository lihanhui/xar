#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>

int main(){
    CvCapture * captureDevice;
    IplImage * currentFrame;
    captureDevice = cvCaptureFromCAM(0);
    if(!captureDevice){
        printf("Error opening webcam. Exiting. \n");
        return -1;
    }
    cvNamedWindow("xar", CV_WINDOW_AUTOSIZE);
    
    int key = -1;
    while(key == -1){
        currentFrame = cvQueryFrame(captureDevice);
        if(!currentFrame){
            break;
        }
        for(int x = 0; x < currentFrame->width; x++){
            for(int y = 0; y < currentFrame->height; y++){
                int B = 255 - CV_IMAGE_ELEM(currentFrame, uchar, y, x*3);
                int G = 255 - CV_IMAGE_ELEM(currentFrame, uchar, y, x*3+1);
                int R = 255 - CV_IMAGE_ELEM(currentFrame, uchar, y, x*3)+2;
                
                CV_IMAGE_ELEM(currentFrame, uchar, y, x*3) = B;
                CV_IMAGE_ELEM(currentFrame, uchar, y, x*3+1) = G;
                CV_IMAGE_ELEM(currentFrame, uchar, y, x*3+2) = R;
            }
            
        }
        cvShowImage("xar", currentFrame);
        key = cvWaitKey(30);
    }
    cvReleaseCapture(&captureDevice);
    cvReleaseImage(&currentFrame);
    cvDestroyWindow("xar");
    return 0;
}