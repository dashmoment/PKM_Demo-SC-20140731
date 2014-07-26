#include"GrabImage.h"

#include <cv.h>  
#include <cxcore.h>  
#include <highgui.h>  
#include <opencv2/video/background_segm.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/video/tracking.hpp>

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

int DST_IMG_WIDTH = 1280;
int DST_IMG_HEIGH = 720;

int iLastX = -1; 
int iLastY = -1;




bool check_line_state=false; 
void on_mouse4(int event, int x,int y,int flags, void* param);
CvPoint tracking_moment(IplImage* treatedimg , IplImage* result_img);

IplImage * img = cvCreateImage(cvSize(img_width, img_height), IPL_DEPTH_8U, 3);
IplImage * showimg = cvCreateImage(cvSize(DST_IMG_WIDTH, DST_IMG_HEIGH), IPL_DEPTH_8U, 3);
IplImage * choose_temp =  cvCreateImage(cvSize(img_width, img_height), IPL_DEPTH_8U, 3);
IplImage * Imgshow = cvCreateImage(cvSize(img_width, img_height), IPL_DEPTH_8U, 3);  
IplImage * temp_img = NULL; 


//MOG
Mat fgMaskMOG;
Ptr<BackgroundSubtractor> pMOG;


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
    cvNamedWindow( "Grab_Frame", 0);
	cvvResizeWindow("Grab_Frame", 640 , 360);
	
	
	hCam = grab->InitCam(pMemVoid , hCam , img_width , img_height , img_bpp);
	pMOG= new BackgroundSubtractorMOG();
	
	CvPoint point2;

	//check tmep	

	double max_temp[2];
	
start: //****************************************************************************

	int checkfile = 1;
	vector<IplImage*> tempdata;
	while(true){
		char tempname[20];
		sprintf(tempname,"temp_%d.jpg",checkfile);

		cout<<tempname<<endl;
		int check =access(tempname, 0);

		if(check == 0){
			temp_img = cvLoadImage(tempname , -1);
			tempdata.push_back(temp_img);
			roi_moment = cvCreateImage(cvGetSize(temp_img),IPL_DEPTH_8U,1);
			cvCvtColor(temp_img, roi_moment, CV_RGB2GRAY);
			temp_center.push_back(tracking_moment(roi_moment , showimg));
					

			cout<<"Input temp no.="<<check<<endl;
			temp_num = checkfile;
			checkfile++;
			//cvShowImage("tempshow",tempdata[0]);
		}

		else{
			cout<<"No such temp no.="<<checkfile<<endl;
			break;
		}
	}

	

	while(true){
		grabimage = grab->Grabimg(pMemVoid , img , grabimage , hCam ,  img_width , img_height);
		cvResize(img,showimg);
		GUIInput = cvWaitKey(10);

		//pMOG->operator()(grabimage , fgMaskMOG);

		//imshow("Frame", grabimage);
        //imshow("FG Mask MOG", fgMaskMOG);
		//grabimage.copyTo(test , fgMaskMOG);
		//imshow("test", test);
			

		if(tempdata.size() != 0){
			max_temp[0] = 0;
			int no_sim;
			double min, max;
			CvSize size;
			CvPoint minLoc;
			CvPoint maxLoc;
			CvPoint mintemp;
			CvPoint maxtemp;
			CvPoint grab_p;

			for(int i = 0 ; i < tempdata.size() ; i++){

				size.width = DST_IMG_WIDTH - tempdata[i]->width  + 1;      
				size.height = DST_IMG_HEIGH - tempdata[i]->height + 1;
				dstimg = cvCreateImage(size, IPL_DEPTH_32F, 1);  	

				cvMatchTemplate(showimg, tempdata[i] , dstimg, CV_TM_CCOEFF_NORMED);		
				cvMinMaxLoc(dstimg, &min, &max, &mintemp, &maxtemp);

				if(max > 0.8){
					max_temp[1] = max;			
					if(max_temp[1] >= max_temp[0]){

						max_temp[0] = max_temp[1];
						minLoc = mintemp;
						maxLoc = maxtemp;
						no_sim = i;

						//cout<<"Most sim temp = "<< no_sim + 1 <<endl;
						//cout<<"Max = "<< max_temp[0] <<endl;

						point2 = cvPoint(maxLoc.x+ tempdata[no_sim]->width, maxLoc.y+tempdata[no_sim]->height);			
						cvRectangle(showimg, maxLoc, point2, cvScalar(0,255,150),2,CV_AA,0);		
						
						grab_p.x = temp_center[no_sim].x + maxLoc.x;
						grab_p.y = temp_center[no_sim].y + maxLoc.y;
						
						
						cvCircle( showimg ,grab_p , 3, Scalar(0,0,255), 1);
						
					}			
				}

			}
						
			cvShowImage("Grab_Frame", showimg);

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
