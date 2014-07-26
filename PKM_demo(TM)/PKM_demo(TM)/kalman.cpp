//#include "opencv2/video/tracking.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include <cv.h>
//
//#include <stdio.h>
//
//using namespace cv;
//
//
//int main()
//{
//	// Initialize Kalman filter object, window, series generator, etc
//	cvNamedWindow( "Kalman", 1 );
//	CvRandState rng;
//	cvRandInit( &rng, 0, 1, -1, CV_RAND_UNI );
//
//	IplImage* img = cvCreateImage( cvSize(500,500), 8, 3 );
//	CvKalman* kalman = cvCreateKalman( 2, 1, 0 );
//
//	// State is phi, delta_phi - indicate of view as well as bony velocity
//	// Initialize with pointless guess
//	CvMat* x_k = cvCreateMat( 2, 1, CV_32FC1 );
//	cvRandSetRange( &rng, 0, 0.1, 0 );
//	rng.disttype = CV_RAND_NORMAL;
//	cvRand( &rng, x_k );
//
//	// Process noise
//	CvMat* w_k = cvCreateMat( 2, 1, CV_32FC1 );
//
//	// Measurements, usually a single parameter for angle
//	CvMat* z_k = cvCreateMat( 1, 1, CV_32FC1 );
//	cvZero( z_k );
//
//	// Transition pattern F describes indication parameters during as well as k as well as k+1
//	const boyant F[] = { 1, 1, 0, 1 };
//	memcpy( kalman->transition_matrix->data.fl, F, sizeof(F));
//
//	// Initialize alternative Kalman parameters
//	cvSetIdentity( kalman->measurement_matrix, cvRealScalar(1) );
//	cvSetIdentity( kalman->process_noise_cov, cvRealScalar(1e-5) );
//	cvSetIdentity( kalman->measurement_noise_cov, cvRealScalar(1e-1) );
//	cvSetIdentity( kalman->error_cov_post, cvRealScalar(1) );
//
//	// Choose pointless primary state
//	cvRand( &rng, kalman->state_post );
//
//	// Make colors
//	CvScalar yellow = CV_RGB(255,255,0);
//	CvScalar white = CV_RGB(255,255,255);
//	CvScalar red = CV_RGB(255,0,0);
//
//	while( 1 ){
//		// Predict indicate position
//		const CvMat* y_k = cvKalmanPredict( kalman, 0 );
//
//		// Generate Measurement (z_k)
//		cvRandSetRange( &rng, 0, sqrt( kalman->measurement_noise_cov->data.fl[0] ), 0 );			
//		cvRand( &rng, z_k );
//		cvMatMulAdd( kalman->measurement_matrix, x_k, z_k, z_k );
//
//		// Plot Points
//		cvZero( img );
//		// Yellow is celebrated state
//		cvCircle( img, 
//			cvPoint( cvRound(img->width/2 + img->width/3*cos(z_k->data.fl[0])),
//			cvRound( img->height/2 - img->width/3*sin(z_k->data.fl[0])) ), 
//			4, yellow );
//		// White is a likely state around a filter
//		cvCircle( img, 
//			cvPoint( cvRound(img->width/2 + img->width/3*cos(y_k->data.fl[0])),
//			cvRound( img->height/2 - img->width/3*sin(y_k->data.fl[0])) ), 
//			4, white, 2 );
//		// Red is a genuine state
//		cvCircle( img, 
//			cvPoint( cvRound(img->width/2 + img->width/3*cos(x_k->data.fl[0])),
//			cvRound( img->height/2 - img->width/3*sin(x_k->data.fl[0])) ),
//			4, red );
//		cvShowImage( "Kalman", img );
//
//		// Adjust Kalman filter state
//		cvKalmanCorrect( kalman, z_k );
//
//		// Apply a passing from one to another pattern F as well as request "process noise" w_k
//		cvRandSetRange( &rng, 0, sqrt( kalman->process_noise_cov->data.fl[0] ), 0 );
//		cvRand( &rng, w_k );
//		cvMatMulAdd( kalman->transition_matrix, x_k, w_k, x_k );
//
//		// Exit upon esc key
//		if( cvWaitKey( 100 ) >= 0 ) 
//			break;
//	}
//
//	return 0;
//}