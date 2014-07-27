//#include "pipe_server.h"
//#include <iostream>  
//#include <stdio.h>
//#include <io.h> 
//
//#include <cv.h>  
//#include <cxcore.h>  
//#include <highgui.h>  
//#include <opencv2/video/background_segm.hpp>
//#include <opencv2/video/video.hpp>
//#include <opencv2/video/tracking.hpp>
//
//using namespace std;  
//using namespace cv;  
//
//pipe_sever* ps = new pipe_sever;
//
//char *recv_data;
//
//void rotateImage(IplImage* img, IplImage *img_rotate, int  degree);
//IplImage* temp_rot;
//
//int main(){
//
//	IplImage* src;
//	IplImage* dst;
//	double degree =  45;
//
//	char tempname[50];
//	int checkfile = 1;
//	sprintf(tempname,"C://temp_img/server/temp_%d.jpg",checkfile);
//
//	src = cvLoadImage(tempname , -1);
//	dst = cvCreateImage(cvSize(src->width , src->height) , src->depth , src->nChannels);
//
//	rotateImage(src , dst , degree);
//
//
//	cvShowImage("Origin temp" , src);
//	cvShowImage("dst temp" , dst);
//
//	cvWaitKey(0);
//
////	/*int init_pipe = ps->init_pipe();
////
////	char pre_x[] = "send form server";
////	ps->send_msg(pre_x);
////
////	recv_data = ps->read_msg();
////	cout<<recv_data<<endl;*/
////
////	char num[] = "1";
////
////		int num_val = atoi(num);
////
////		if(num_val != 0)
////		printf("num_val = %d" , num_val);
////
////
////
//}
//
//void rotateImage(IplImage* img, IplImage *img_rotate,int degree)  
//{  
//    //旋转中心为图像中心  
//    CvPoint2D32f center;    
//    center.x=float (img->width/2.0 + 1);  
//    center.y=float (img->height/2.0 + 1);  
//    //计算二维旋转的仿射变换矩阵  
//    float m[6];              
//    CvMat M = cvMat( 2, 3, CV_32F, m );  
//    cv2DRotationMatrix( center, degree,1, &M);  
//    //变换图像，并用黑色填充其余值  
//    cvWarpAffine(img,img_rotate, &M,CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS,cvScalarAll(0) );  
//}  
//
