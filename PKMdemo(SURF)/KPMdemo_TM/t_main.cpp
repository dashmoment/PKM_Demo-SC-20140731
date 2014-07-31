////#include "kal_tracking.h"
//#include"GrabImage.h"
//#include "opencv2/video/tracking.hpp"
//#include "opencv2/highgui/highgui.hpp"
//
//#include <cv.h>  
//#include <cxcore.h>  
//#include <highgui.h>  
//#include <opencv2/video/background_segm.hpp>
//#include <opencv2/video/video.hpp>
//#include <opencv2/video/tracking.hpp>
//
//#include <iostream>  
//#include <stdio.h>
//#include <io.h> 
//
//using namespace std; 
//using namespace cv;
//
//int main(){
//
//	Mat m_src;
//
//    KalmanFilter KF;
//    Mat state; 
//    Mat processNoise;
//    Mat measurement;
//
//
//	int kalan_init(CvPoint center_init);
//	CvPoint kal_track(IplImage *src , CvPoint measure);
//	Mat src(500, 500, CV_8UC3);
//	//src = Scalar::all(0);
//	
//
//	CvPoint ini;
//	ini.x = 25;
//	ini.y = 25;
//
//	KF.init(4,4,0);
//    state = Mat(4, 1, CV_32F);
//
//
//	state.at<float>(0,0) = ini.x;
//	state.at<float>(1,0) = ini.y;
//	state.at<float>(2,0) = 0.5;
//	state.at<float>(3,0) = 0.5;
//
//    processNoise = Mat(4, 1, CV_32F);
//    measurement = Mat::zeros(4, 1, CV_32F);
//	
//	KF.transitionMatrix = *(Mat_<float>(4,4) << 1,0,1,0,0,1,0,1,0,0,1,0,0,0,0,1 );
//	KF.measurementMatrix = *(Mat_<float>(4,4) << 1,0,1,0,0,1,0,1,0,0,1,0,0,0,0,1 );
//
//	//setIdentity(KF.measurementMatrix, Scalar::all(1e-5));
//	setIdentity(KF.processNoiseCov, Scalar::all(1e-5));
//	setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));
//	setIdentity(KF.errorCovPost, Scalar::all(1));
//
//	Mat post_noise(4,1,CV_32F);
//
//	randn(post_noise,  Scalar::all(0), Scalar::all(0.1));
//
//	KF.statePost = state + post_noise;
//
//	while(state.at<float>(0,0) < src.cols){
//
//		Mat prediction = KF.predict();
//		randn( measurement, Scalar::all(0), Scalar::all(KF.measurementNoiseCov.at<float>(0)));
//		measurement += KF.measurementMatrix*state;
//
//		//cout<<  KF.errorCovPost.size()<<endl;
//
//		circle(src,cvPoint(state.at<float>(0,0),state.at<float>(1,0)), 3 ,cvScalar(0,255,255),1);
//		circle(src,cvPoint(prediction.at<float>(0,0),prediction.at<float>(1,0)), 3 ,cvScalar(255,0,0),1);
//
//		//cout<<"state X = "<<state.at<float>(0,0)<< "state Y = "<<state.at<float>(1,0)<<endl;
//		//cout<<"pre X = "<<prediction.at<float>(0,0)<< "pre Y = "<<prediction.at<float>(1,0)<<endl;
//
//		float error_X = abs(state.at<float>(0,0) - prediction.at<float>(0,0));
//		float error_Y = abs(state.at<float>(1,0) - prediction.at<float>(1,0));
//		
//		if( error_X < 1 && error_Y < 1){
//
//		KF.correct(measurement);
//		cout<<"Stop"<<endl;
//
//		}
//	
//		state = KF.transitionMatrix*state;
//
//		randn( processNoise, Scalar(0), Scalar::all(sqrt(KF.processNoiseCov.at<float>(0, 0))));
//
//		state = state + processNoise;
//
//		cvNamedWindow("prediction",0);
//
//		imshow("prediction" , src);
//
//		if(cvWaitKey(10)>=0){
//				  break;
//		}
//	
//
//	}
//
//
//}