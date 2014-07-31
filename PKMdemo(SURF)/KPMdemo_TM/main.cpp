#include"GrabImage.h"

#include <cv.h>  
#include <cxcore.h>  
#include <highgui.h>  
#include <opencv2/video/background_segm.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2\nonfree\features2d.hpp>
#include <features2d\features2d.hpp>

#include <iostream>  
#include <stdio.h>
#include <io.h> 

using namespace std;  
using namespace cv;  

CvRect ROI_rect;   
IplImage* dstimg = NULL;  
IplImage* ROIImg = NULL;

int img_width = 1280;
int img_height = 720;

int img_bpp = 24;

int DST_IMG_WIDTH = 640;
int DST_IMG_HEIGH = 360;

int iLastX = -1; 
int iLastY = -1;


bool check_line_state=false; 
void on_mouse4(int event, int x,int y,int flags, void* param);


IplImage * img = cvCreateImage(cvSize(img_width, img_height), IPL_DEPTH_8U, 3);
IplImage * showimg = cvCreateImage(cvSize(DST_IMG_WIDTH, DST_IMG_HEIGH), IPL_DEPTH_8U, 3);
IplImage * choose_temp =  cvCreateImage(cvSize(img_width, img_height), IPL_DEPTH_8U, 3);
IplImage * Imgshow = cvCreateImage(cvSize(img_width, img_height), IPL_DEPTH_8U, 3);  
IplImage * temp_img = NULL; 
CvPoint tracking_moment(IplImage* treatedimg , IplImage* result_img);

int temp_num;


int main(){
	Mat test;
	IplImage* roi_moment;
	vector<CvPoint>temp_center;

	
	HIDS hCam = 0;	
	void * pMemVoid = NULL;
	char GUIInput ;
	GrabImage *grab = new GrabImage(); 
	Mat grabimage;
    //cvNamedWindow( "Grab_Frame", 0);
	//cvvResizeWindow("Grab_Frame", 640 , 360);
	cvNamedWindow( "match", 0);
	
	
	hCam = grab->InitCam(pMemVoid , hCam , img_width , img_height , img_bpp);
	
	

	//check tmep	

	

	
 //****************************************************************************

	
	////************SURF***********************
	Mat src_resize;
	Mat image_match;
	Size size_mat;
	SurfFeatureDetector detector(400);
	SurfDescriptorExtractor extractor;
	FlannBasedMatcher matcher;
	vector<DMatch> matches;

	vector<KeyPoint> keypoints_src, keypoints_tmp;
	Mat descriptors_src, descriptors_tmp;

	vector<IplImage*> tempdata;
	vector<Mat> tempdata_mat;
	vector<vector<KeyPoint>> keypoints;
	vector<Mat>descriptor;

	Mat temp_Mat;

	double max_dist = 0; 
	double min_dist = 100;
	vector< DMatch > good_matches;

	vector<Point2f> obj;
	vector<Point2f> scene;

	Mat homograqphy;
	vector<Point2f> obj_corners(4);
	vector<Point2f> scene_corners(4);

	////***************************************
start:
	int checkfile = 1;

	while(true){
		char tempname[20];
		sprintf(tempname,"temp_%d.jpg",checkfile);

		//cout<<tempname<<endl;
		int check = _access(tempname, 0);

		if(check == 0){
			temp_img = cvLoadImage(tempname , -1);
			temp_Mat = imread(tempname , CV_LOAD_IMAGE_COLOR);

			
			tempdata.push_back(temp_img);
			tempdata_mat.push_back(temp_Mat);

			detector.detect(temp_Mat, keypoints_tmp); /////KeyPoint Detect
			keypoints.push_back(keypoints_tmp);
			extractor.compute(temp_img, keypoints_tmp, descriptors_tmp);
			descriptor.push_back(descriptors_tmp);


			roi_moment = cvCreateImage(cvGetSize(temp_img),IPL_DEPTH_8U,1);
			cvCvtColor(temp_img, roi_moment, CV_RGB2GRAY);
			temp_center.push_back(tracking_moment(roi_moment , showimg));
					

			cout<<"Input temp no.= "<<checkfile<<endl;
			temp_num = checkfile;
			checkfile++;
			//cvShowImage("tempshow",tempdata[0]);
		}

		else{
			//cout<<"No such temp no.= "<<checkfile<<endl;
			break;
		}
	}

	

	while(true){

		grabimage = grab->Grabimg(pMemVoid , img , grabimage , hCam ,  img_width , img_height);
		cvResize(img,showimg);
		size_mat = Size(DST_IMG_WIDTH,DST_IMG_HEIGH);
		src_resize = Mat(size_mat , grabimage.type());
		resize(grabimage,src_resize , size_mat);
		image_match = src_resize;

		

		detector.detect( src_resize, keypoints_src );
		extractor.compute( src_resize, keypoints_src, descriptors_src );


		GUIInput = cvWaitKey(10);		

		if(descriptor.size()!=0 ){

			for(int k  = 0 ; k < descriptor.size() ; k++){

				if(descriptors_src.empty() || descriptor[k].empty()){
					break;
				}

				matcher.match( descriptor[k] , descriptors_src, matches );

				for( int i = 0; i < descriptor[k].rows; i++ ){
					double dist = matches[i].distance;
					if( dist < min_dist ) min_dist = dist;
					if( dist > max_dist ) max_dist = dist;
				}

				printf("-- Max dist : %f \n", max_dist );
				printf("-- Min dist : %f \n", min_dist );


			   for( int i = 0; i <  descriptor[k].rows; i++ ){ 
				 if( matches[i].distance < 3*min_dist ){ 
					 good_matches.push_back( matches[i]); 
				 }
			  }

			 
			   /*drawMatches( tempdata[k], keypoints[k], src_resize, keypoints_src,
               good_matches, image_match, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );*/
			   
			   cout<< keypoints[k].size()<<endl;
			   cout<<good_matches.size()<<endl;
			   cout<<"finished good match"<<endl;

			   for( int i = 0; i < good_matches.size(); i++ ){
				//-- Get the keypoints from the good matches
				   
					obj.push_back( keypoints[k][ good_matches[i].queryIdx ].pt );
					scene.push_back( keypoints_src[ good_matches[i].trainIdx ].pt );
				}

			   cout<<"finished obj and scene"<<endl;

			    if(good_matches.size() > 4){
			   
					homograqphy = findHomography( obj, scene, CV_RANSAC );

					cout<<"finished homograqphy"<<endl;
			
			   
					obj_corners[0] = cvPoint(0,0); 
					obj_corners[1] = cvPoint( tempdata_mat[k].cols, 0 );
					obj_corners[2] = cvPoint( tempdata_mat[k].cols , tempdata_mat[k].rows ); 
					obj_corners[3] = cvPoint( 0, tempdata_mat[k].rows );

					cout<< tempdata_mat[k].cols<<endl;
					cout<<obj_corners[0]<<endl;
					cout<<obj_corners[1]<<endl;
					//std::vector<Point2f> scene_corners(4);
					perspectiveTransform( obj_corners, scene_corners, homograqphy);

					cout<<"finished perspectiveTransform"<<endl;

					/*line( image_match, scene_corners[0] + Point2f( tempdata_mat[k].cols, 0), scene_corners[1] + Point2f( tempdata_mat[k].cols, 0), Scalar(0, 255, 0), 1 );
					line( image_match, scene_corners[1] + Point2f( tempdata_mat[k].cols, 0), scene_corners[2] + Point2f( tempdata_mat[k].cols, 0), Scalar( 0, 255, 0), 1 );
					line( image_match, scene_corners[2] + Point2f( tempdata_mat[k].cols, 0), scene_corners[3] + Point2f( tempdata_mat[k].cols, 0), Scalar( 0, 255, 0), 1 );
					line( image_match, scene_corners[3] + Point2f( tempdata_mat[k].cols, 0), scene_corners[0] + Point2f( tempdata_mat[k].cols, 0), Scalar( 0, 255, 0), 1 );*/

					line( image_match, scene_corners[0] , scene_corners[1] , Scalar(0, 255, 0), 1 );
					line( image_match, scene_corners[1] , scene_corners[2] , Scalar( 0, 255, 0), 1 );
					line( image_match, scene_corners[2] , scene_corners[3] , Scalar( 0, 255, 0), 1 );
					line( image_match, scene_corners[3] , scene_corners[0] , Scalar( 0, 255, 0), 1 );

				}
				
				good_matches.clear();
				obj.clear();
				scene.clear();
				//obj_corners.clear();
				//scene_corners.clear();
				
			}

			imshow("match" , image_match);		
			
			
			
			//cvShowImage("Grab_Frame", showimg);

		}

		else{
			
			cvShowImage("Grab_Frame", showimg);
		}


		//Keyboard Action

		if(GUIInput=='p') break;

		if(GUIInput=='q') goto endstream;
	}

	choose_temp = cvCloneImage(showimg);
	cvShowImage("choose_temp",choose_temp);   
	cvSetMouseCallback("choose_temp",on_mouse4);  
		
	cvWaitKey(0);
	
	goto start;
	  
	endstream: //****************************************************************************


	cvReleaseImage(&showimg);

	return 0;

}

void on_mouse4(int event, int x,int y,int flags, void* param){  
    int thickness=2;  
    CvPoint p1,p2;
	char temp[20];

    if(event == CV_EVENT_LBUTTONDOWN){    //left button of mouse is down  
        ROI_rect.x=x;  
        ROI_rect.y=y;  
        check_line_state=true;  
    }  
    else if(check_line_state && event == CV_EVENT_MOUSEMOVE){  
        Imgshow = cvCreateImage(cvGetSize(choose_temp), IPL_DEPTH_8U, 3);//use Imgshow to show that we draw a green rect in it    
        Imgshow  = cvCloneImage(choose_temp);  
        p1=cvPoint(ROI_rect.x,ROI_rect.y);  
        p2=cvPoint(x,y);  
        cvRectangle(Imgshow,p1,p2,CV_RGB(0,255,150),thickness,CV_AA,0);  
        cvShowImage("choose_temp",Imgshow);   
        cvReleaseImage(& Imgshow);   
    }  
    else if(check_line_state && event == CV_EVENT_LBUTTONUP){ 
		
		cvDestroyWindow("choose_temp");

        ROI_rect.width = abs(x - ROI_rect.x);   
        ROI_rect.height = abs(y - ROI_rect.y);

        cvSetImageROI(choose_temp, ROI_rect);   
        
        IplImage* dstImg;  
        
        ROIImg = cvCreateImage(cvSize(ROI_rect.width,ROI_rect.height),8,3);  
		
        cvCopy(choose_temp,ROIImg);  
        cvResetImageROI(showimg);  
        cvNamedWindow("ROI",1);  
        cvShowImage("ROI",ROIImg);  

		if(ROIImg != NULL){		
			temp_num++;
			cout<<"write file no. ="<<temp_num<<endl;
			sprintf(temp, "temp_%d.jpg",temp_num);
			cvSaveImage(temp,  ROIImg);
			cvReleaseImage(& ROIImg);

			//cvWaitKey(20);
		}
        
      
    }    
}  

CvPoint tracking_moment(IplImage* treatedimg , IplImage* result_img){

	CvPoint center;
	CvMoments oMoments;
	cvMoments(treatedimg,&oMoments,1);

	double dM01 = cvGetSpatialMoment(&oMoments , 0 , 1);
	double dM10 = cvGetSpatialMoment(&oMoments , 1 , 0);
	double dArea = cvGetSpatialMoment(&oMoments , 0 , 0);

		 
			
	int posX = dM10 / dArea;
	int posY = dM01 / dArea;  
	center.x = posX;
	center.y = posY;
	

     return center;
}


