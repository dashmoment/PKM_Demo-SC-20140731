#include "pipe_server.h"
#include "RS323_COM.h"
#include"GrabImage.h"

#include <cv.h>  
#include <cxcore.h>  
#include <highgui.h>  
#include <opencv2/video/background_segm.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/video/tracking.hpp>

#include <windows.h>
#include <time.h>
#include <iostream>  
#include <stdio.h>
#include <stdlib.h>
#include <io.h> 

using namespace std;  
using namespace cv;  

pipe_sever* ps = new pipe_sever;
RS232_COM* com = new RS232_COM;
 
////****************camera param*********************
int img_width = 1280;
int img_height = 720;

int img_bpp = 24;

int DST_IMG_WIDTH = 320;
int DST_IMG_HEIGH = 180;                           

int iLastX = -1; 
int iLastY = -1;

HIDS hCam = 2;	
void * pMemVoid = NULL;
IplImage * img = cvCreateImage(cvSize(img_width, img_height), IPL_DEPTH_8U, 3);
IplImage * showimg = cvCreateImage(cvSize(DST_IMG_WIDTH, DST_IMG_HEIGH), IPL_DEPTH_8U, 3);
IplImage * hsv_showimg = cvCreateImage(cvSize(DST_IMG_WIDTH, DST_IMG_HEIGH), IPL_DEPTH_8U, 3);
IplImage * hue_showimg = cvCreateImage(cvSize(DST_IMG_WIDTH, DST_IMG_HEIGH), IPL_DEPTH_8U, 1);

 CvRect Rec_roi;
 

Mat grabimage;
char GUIInput ;
////*************Timer************************

clock_t t1, t2;
int time_idx = 0;

////**************TM**************************
IplImage * choose_temp =  cvCreateImage(cvSize(img_width, img_height), IPL_DEPTH_8U, 3);
IplImage * showtemp = cvCreateImage(cvSize(img_width, img_height), IPL_DEPTH_8U, 3); 
IplImage * src =  cvCreateImage(cvSize(DST_IMG_WIDTH, DST_IMG_HEIGH), IPL_DEPTH_8U, 3);
IplImage* dstimg = NULL;  
IplImage* ROIImg = NULL;
IplImage * temp_img = NULL; 
IplImage* roi_moment;

CvRect ROI_rect;   

vector<CvPoint>temp_center;
vector<CvPoint> v_grap;
vector<CvPoint> v_max;
vector<int>rectempnum;

CvPoint rec_max;

double max_temp[2];
double threshold_mat = 0.8;

bool check_line_state=false; 

int temp_num;


////*******************Rotate Temp****************

void rotateImage(IplImage* img, IplImage *img_rotate, int  degree);
CvPoint center;
float radius;


//******************Img2global param*****************

float Ax =  0.33;
float Bx = - 640;
float Ay = -0.33;
float By = -360;

float pre_bias;   //bias for moving direction

float sc_rate = 3.5;
float down_sample = 4; 
////********************Function****************************
void on_mouse4(int event, int x,int y,int flags, void* param);
CvPoint tracking_moment(IplImage* treatedimg);
string pre_fetch(CvPoint es_g);
float tran_2GX(int img_x);
float tran_2GY(int img_y ,  int img_x);


int main(){

	Rec_roi = cvRect(0,0,320,180);

	//int init_pipe = 0;
	int recv_num = 0;
	char* recv_data;


    //init_pipe = ps->init_pipe();   //// inti pipe server
		
	GrabImage *grab = new GrabImage(); 

	hCam = grab->InitCam(pMemVoid , hCam , img_width , img_height , img_bpp);
	
    cvNamedWindow( "Grabimage_server", 0);
	cvvResizeWindow("Grabimage_server", 320 , 180);

	

start:

	int checkfile = 1;
	vector<IplImage*> tempdata;

	while(true){
		char tempname[50];
		sprintf(tempname,"C://temp_img/server/temp_%d.jpg",checkfile);

		//cout<<tempname<<endl;

		int check = _access(tempname, 0);
		//int check = 0 ;

		if(check == 0){
			temp_img = cvLoadImage(tempname , -1);
			
			//cvShowImage("tempshow",temp_img);
			tempdata.push_back(temp_img);
			roi_moment = cvCreateImage(cvGetSize(temp_img),IPL_DEPTH_8U,1);

			//cvCvtColor(temp_img, roi_moment, CV_RGB2GRAY);
			temp_center.push_back(tracking_moment(temp_img));			
			cout<<"Input temp no.= "<<checkfile<<endl;
			temp_num = checkfile;
			checkfile++;
			//cvShowImage("tempshow",tempdata[0]);
		}

		else{
			cout<<"No such temp no.= "<<checkfile<<endl;
			break;
		}
	}

	cout<<"Input temp stage clear"<<endl;

	int rs232_idx = 0;
	IplImage * roi_src;
	
	while(true){
		int no_sim = 0;
		cvResetImageROI(hue_showimg);
		grabimage = grab->Grabimg(pMemVoid , img , grabimage , hCam ,  img_width , img_height);
		cvResize(img,showimg);
		cvResize(img,src);
		cvCvtColor(showimg , hsv_showimg , CV_BGR2HSV );

		cvSplit(hsv_showimg,0,0,hue_showimg,0);
		cvSetImageROI(hue_showimg , Rec_roi );

		cvSmooth(hue_showimg , hue_showimg , CV_GAUSSIAN ,3,3,0,0);

		//cvInRangeS(hue_showimg , cvScalar(50,0,0,0),cvScalar(255,0,0,0),hue_showimg);
		
		cvShowImage("hue_src" , hue_showimg);
		

		if(tempdata.size() != 0){
			max_temp[0] = 0;
			double min, max;
			CvSize size;
			CvPoint minLoc;
			CvPoint maxLoc;
			CvPoint mintemp;
			CvPoint maxtemp;
			CvPoint grab_p = cvPoint(-1,-1);

			//cout<<grab_p.x<<endl;

			//****************chose best match template**************************
			for(int i = 0 ; i < tempdata.size() ; i++){

				//cout<<tempdata.size()<<endl;
				no_sim++;
				size.width = DST_IMG_WIDTH - tempdata[i]->width  + 1;      
				size.height = DST_IMG_HEIGH - tempdata[i]->height + 1;
				dstimg = cvCreateImage(size, IPL_DEPTH_32F, 1);  

				

			   cvMatchTemplate(hue_showimg , tempdata[i] , dstimg, CV_TM_CCOEFF_NORMED);		
			   cvMinMaxLoc(dstimg, &min, &max, &mintemp, &maxtemp);
					//cvReleaseImage(&temp_rot);	

					if(max > 0.5){
						cout<<"max = "<<max<<endl;
						cout<<"Most sim temp = "<< no_sim + 1 <<endl;
						
						minLoc = mintemp;
						maxLoc = maxtemp;
						maxLoc.x = maxtemp.x+25;
						no_sim = i;

						grab_p.x = temp_center[no_sim].x + maxLoc.x ;
						grab_p.y = temp_center[no_sim].y + maxLoc.y;
											
						
						
									
						rec_max = cvPoint(maxLoc.x+tempdata[no_sim]->width , maxLoc.y+tempdata[no_sim]->height);			
						
						
						v_grap.push_back(grab_p);
						v_max.push_back(maxLoc);
						
						break;
							
					}		
			}

			if(v_grap.size() != 0){

				for(int j = 0 ; j < v_grap.size() ; j++){

					cvCircle( showimg , v_grap[j] , 3, Scalar(0,0,255), 1);
					cvRectangle(showimg, maxLoc, rec_max, cvScalar(0,255,180),1,CV_AA,0);		
							
							
				}
			}
			 
			//cout<<"Recognized stage clear"<<endl;

			if(rs232_idx == 0 && v_grap.size() > 0){ //// 1st sender

			
				char pre_x[10];
				sprintf(pre_x, "%.3f", tran_2GX(v_grap[0].x));
				//cout<<"sc_x = "<<v_grap[0].x<<endl;
				//ps->send_msg(pre_x);

				Sleep(50);

				char pre_y[10];
				sprintf(pre_y, "%.3f", tran_2GY(v_grap[0].y , v_grap[0].x));
				//cout<<"sc_xy = "<<v_grap[0].y<<endl;
				//ps->send_msg(pre_y);

				char temp_id[10];
				//sprintf(temp_id, "%d",  no_sim+1);
				//ps->send_msg(temp_id);
				
				
				cout<<"Send to client"<<endl;
				rs232_idx = 1;


				//////***timer**********************
				//if(time_idx == 1){
				//			t2 = clock();
				//			time_idx = 2;
				//			cout<<"time = "<<(t2-t1)/(double)(CLOCKS_PER_SEC)<<endl;
				//			cout<<"dst y = "<<pre_y<<endl;
				//			system("pause");
				//		}
				//			if(time_idx == 0){
				//			t1 = clock();
				//			cout<<"origin y = "<<pre_y<<endl;
				//			time_idx = 1;
				//		}
				
			}

			if(rs232_idx == 1){

			
				//recv_num = atoi(ps->read_msg()); //// 1st reader
			
				//char endnote[] = "1";
				//int end = recv_num;
   
				cout<<"read from clinet stage clear : "<< recv_num <<endl;
							
				//if(recv_num == 0){
					rs232_idx = 0;
				//}

			}

			v_grap.clear();
		}

		cvShowImage("Grabimage_server", showimg);

		GUIInput = cvWaitKey(10);
		if(GUIInput=='p') break;

		if(GUIInput=='q') goto endstream;
	}

	choose_temp = cvCloneImage(showimg);
	cvShowImage("choose_temp",showimg);   
	cvSetMouseCallback("choose_temp",on_mouse4); 
	cvWaitKey(0);
	goto start;
endstream:
	return 0;
}

void on_mouse4(int event, int x,int y,int flags, void* param){  
    int thickness=2;  
    CvPoint p1,p2;
	

    if(event == CV_EVENT_LBUTTONDOWN){    //left button of mouse is down  
       ROI_rect.x=x;  
       ROI_rect.y=y;  
		center = cvPoint(x,y);
		

        check_line_state=true;  
    }  
    else if(check_line_state && event == CV_EVENT_MOUSEMOVE){  

		
		showtemp = cvCreateImage(cvGetSize(choose_temp), choose_temp->depth, choose_temp->nChannels);//use Imgshow to show that we draw a green rect in it    
        showtemp  = cvCloneImage(choose_temp);  
        p1=cvPoint(ROI_rect.x,ROI_rect.y);  
        p2=cvPoint(x,y);  

		//radius = abs(sqrt((x - ROI_rect.x)^2 + ( y - ROI_rect.y)^2));
        cvRectangle(showtemp,p1,p2,CV_RGB(0,255,150),thickness,CV_AA,0);  
		cvCircle(showtemp,center ,radius,CV_RGB(0,255,150),3,CV_AA,0);
		
		cvShowImage("choose_temp",showtemp);   
        cvReleaseImage(& showtemp);   
    }  
    else if(check_line_state && event == CV_EVENT_LBUTTONUP){ 
		
		cvDestroyWindow("choose_temp");
		
        ROI_rect.width = abs(x - ROI_rect.x);   
        ROI_rect.height = abs(y - ROI_rect.y);
		//ROI_rect.x = ROI_rect.x - radius;
		//ROI_rect.y = ROI_rect.y - radius;
		//ROI_rect.width = 2*radius;   
        //ROI_rect.height = 2*radius;

        cvSetImageROI(choose_temp, ROI_rect);   
        
        IplImage* dstImg;  
        
        ROIImg = cvCreateImage(cvSize(ROI_rect.width,ROI_rect.height), choose_temp->depth, choose_temp->nChannels);  
		
        cvCopy(choose_temp,ROIImg);  
        cvResetImageROI(showimg);  
        //cvNamedWindow("ROI",1);  
        //cvShowImage("ROI",ROIImg);  

		if(ROIImg != NULL){		
			

			char temp[50];
			char temp2[50] ;

			//IplImage * client_temp = cvCreateImage(cvSize(ROIImg->width*sc_rate , ROIImg->height*sc_rate) , ROIImg->depth,ROIImg->nChannels);
			
		

			for(int j = 0 ; j < 361 ; j =  j + 20){	

				IplImage * server_temp = cvCreateImage(cvSize(ROIImg->width , ROIImg->height),ROIImg->depth,ROIImg->nChannels);
				IplImage * shue_temp = cvCreateImage(cvSize(ROIImg->width , ROIImg->height) , ROIImg->depth,1);
				IplImage* temp_rot = cvCreateImage(cvSize(ROIImg->width , ROIImg->height) , ROIImg->depth,1);
				temp_num++;

			    cvCvtColor(ROIImg , server_temp , CV_BGR2HSV );
				cvSplit(server_temp,0,0,shue_temp,0);
				cvSmooth(shue_temp , shue_temp , CV_GAUSSIAN ,3,3,0,0);
				//cvInRangeS(shue_temp , cvScalar(50,0,0,0),cvScalar(255,0,0,0),shue_temp);		
				rotateImage(shue_temp , temp_rot , j);
				
				
				sprintf(temp, "C://temp_img/server/temp_%d.jpg",temp_num);
				cout<<"write file no. = "<<temp_num<<endl;
				cvSaveImage(temp,  temp_rot);

				cvReleaseImage(&server_temp);
			    cvReleaseImage(&shue_temp);
				cvReleaseImage(&temp_rot);
			
			}

			//sprintf(temp2, "C://temp_img/client/temp_%d.jpg",temp_num);
			//IplImage * temp_hsv = cvCreateImage(cvSize(ROIImg->width , ROIImg->height),ROIImg->depth,ROIImg->nChannels);
			//IplImage * temp_hue = cvCreateImage(cvSize(ROIImg->width , ROIImg->height),ROIImg->depth,1);
			//IplImage * hue_temp = cvCreateImage(cvSize(ROIImg->width*sc_rate , ROIImg->height*sc_rate) , ROIImg->depth,1);

			//cvCvtColor(ROIImg , temp_hsv , CV_BGR2HSV );
			//cvSplit(temp_hsv,0,0,temp_hue,0);
			////cvShowImage("temp_hsv",temp_rot );

			//cvResize(temp_hue, hue_temp,INTER_LANCZOS4 );
			//cvSaveImage(temp2,  hue_temp);

			

			
			cvReleaseImage(& ROIImg);

			//cvWaitKey(20);
		}
        
      
    }    
}  

CvPoint tracking_moment(IplImage* treatedimg){

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



float tran_2GX(int img_x){

	float gx = Ax*(down_sample*img_x  + Bx+ 150) ;
	cout<<"gx ="<<gx<<endl;
	cout<<"px ="<<img_x<<endl;
	return gx;
}

float tran_2GY(int img_y , int img_x){
	 
	float gy = -Ay*(down_sample*img_y + By) + 100 - 0.3*abs(img_x-100) ;
	cout<<"gy ="<<gy<<endl;
	cout<<"py ="<<img_y<<endl;
	return gy;

}

string pre_fetch(CvPoint es_g){

	
		char fetch_p[50]="";
		char par_x[] ="P197=";
		char par_y[] ="P196=";
		char par_z[] ="P193=-300";

		sprintf(fetch_p,"%s%s%s",par_x  , par_y  , par_z);
		//cout<<fetch_p<<endl;
		return fetch_p;

	
}

void rotateImage(IplImage* img, IplImage *img_rotate,int degree)  
{  
    
    CvPoint2D32f center;    
    center.x=float (img->width/2.0 + 1);  
    center.y=float (img->height/2.0 + 1);  
   
    float m[6];              
    CvMat M = cvMat( 2, 3, CV_32F, m );  
    cv2DRotationMatrix( center, degree,1, &M);  
   
    cvWarpAffine(img,img_rotate, &M,CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS,cvScalarAll(1) );  
}  