//#include "opencv2/video/tracking.hpp"
//#include "opencv2/highgui/highgui.hpp"
//
//#include <stdio.h>
//
//using namespace cv;
//using namespace std;
//
//
//class tracking_kal{
//
//	public:
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
//
//};
//
//int tracking_kal::kalan_init(CvPoint center_init){
//
//
//	//KF.init(4,1,0);
// //   state = Mat(4, 1, CV_32F);
//
//	//state.at<float>(0,0) = center_init.x;
//	//state.at<float>(1,0) = center_init.y;
//	//state.at<float>(2,0) = 0;
//	//state.at<float>(3,0) = 0;
//
// //   processNoise = Mat(4, 1, CV_32F);
// //   measurement = Mat::zeros(2, 1, CV_32F);
//	//
//	//KF.transitionMatrix = *(Mat_<float>(4,4) << 1,0,1,0,0,1,0,1,0,0,1,0,0,0,0,1 );
//	//setIdentity(KF.measurementMatrix);
//	//setIdentity(KF.processNoiseCov, Scalar::all(1e-5));
//	//setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));
//
//	//randn(KF.statePost, state + Scalar::all(0), state + Scalar::all(0.1));
//	
//
//	return 1;
//
//}
//
//CvPoint tracking_kal::kal_track(IplImage *src , CvPoint measure){
//
//	
//    Mat m_src = Mat(src , 0);
//
//	Mat prediction = KF.predict();
//
//    measure += KF.measurementMatrix*state;
//
//}