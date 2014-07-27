#include "pipe_server.h"
#include "RS323_COM.h"
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
Mat grabimage;
char GUIInput ;


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
IplImage* temp_rot;

//******************Img2global param*****************

float Ax =  0.385;
float Bx = - 640;
float Ay = -0.385;
float By = -360;

float pre_bias;   //bias for moving direction

int sc_rate = 4;
////********************Function****************************
void on_mouse4(int event, int x,int y,int flags, void* param);
CvPoint tracking_moment(IplImage* treatedimg , IplImage* result_img);
string pre_fetch(CvPoint es_g);
float tran_2GX(int img_y);
float tran_2GY(int img_x);


int main(){

	int init_pipe = 0;

	int recv_num = 0;
	char* recv_data;
	char* endnote = "finished";

	
    init_pipe = ps->init_pipe();   //// inti pipe server
		
	GrabImage *grab = new GrabImage(); 

	hCam = grab->InitCam(pMemVoid , hCam , img_width , img_height , img_bpp);
	
    cvNamedWindow( "Grabimage_server", 0);
	cvvResizeWindow("Grabimage_server", 320 , 180);

	//com->init_port();

	/*char send_data[10];
	string t_str = "&1b22r";
	com->write_port(t_str);
	Sleep(100);*/
	//t_str = "resplc6";
	//com->write_port(t_str);
	//Sleep(100);

	
	//char msg[] = "test msg";
	//ps->read_msg();
	//ps->send_msg(msg);

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

			cvCvtColor(temp_img, roi_moment, CV_RGB2GRAY);
			temp_center.push_back(tracking_moment(roi_moment , showimg));			
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


	while(true){

		
		grabimage = grab->Grabimg(pMemVoid , img , grabimage , hCam ,  img_width , img_height);
		cvResize(img,showimg);
		cvResize(img,src);

		/*Rect src_ROI;

		src_ROI.x = 0;
		src_ROI.y = 0;
		src_ROI.width = 50;
		src_ROI.height = 50;

		cvSetImageROI(showimg , src_ROI);*/
		
			int no_sim;

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

				size.width = DST_IMG_WIDTH - tempdata[i]->width  + 1;      
				size.height = DST_IMG_HEIGH - tempdata[i]->height + 1;
				dstimg = cvCreateImage(size, IPL_DEPTH_32F, 1);  

				for(int j = 0 ; j < 361 ; j = j+30){

				
					temp_rot = cvCreateImage(cvSize(tempdata[i]->width , tempdata[i]->height),tempdata[i]->depth,tempdata[i]->nChannels);
					rotateImage(tempdata[i] , temp_rot , j);

			
					cout<<"angle = " << j <<endl;
					
					//Sleep(500);

					cvMatchTemplate(showimg, temp_rot , dstimg, CV_TM_CCOEFF_NORMED);		
					cvMinMaxLoc(dstimg, &min, &max, &mintemp, &maxtemp);
					//cvReleaseImage(&temp_rot);

					cout<<max<<endl;

					if(max > 0.5){
						cvShowImage("Rotate temp" , temp_rot);
						//max_temp[1] = max;			
						//if(max_temp[1] >= max_temp[0]){

							//max_temp[0] = max_temp[1];
							minLoc = mintemp;
							maxLoc = maxtemp;
							no_sim = i;

							cout<<"Most sim temp = "<< no_sim + 1 <<endl;
							//cout<<"Max = "<< max_temp[0] <<endl;

							IplImage *recog_temp;

							rec_max = cvPoint(maxLoc.x+ tempdata[no_sim]->width, maxLoc.y+tempdata[no_sim]->height);			
							cvRectangle(showimg, maxLoc, rec_max, cvScalar(0,255,150),1,CV_AA,0);		
						
							grab_p.x = temp_center[no_sim].x + maxLoc.x;
							grab_p.y = temp_center[no_sim].y + maxLoc.y;

							v_grap.push_back(grab_p);
							v_max.push_back(maxLoc);
							//rectempnum.push_back(no_sim);
							cout<<"recog_temp"<<no_sim<<endl;
							cvReleaseImage(&temp_rot);
							break;
						//}			
					}

					
				}

			}

			if(v_grap.size() != 0){

				for(int j = 0 ; j < v_grap.size() ; j++){

					cvCircle( showimg , v_grap[j] , 3, Scalar(0,0,255), 1);
							
							
				}
			}
			 
			cout<<"Recognized stage clear"<<endl;

			if(rs232_idx == 0 && v_grap.size() > 0){ //// 1st sender

			
				char pre_x[10];
				sprintf(pre_x, "%.3f", tran_2GX(v_grap[0].x));
				ps->send_msg(pre_x);

				Sleep(50);

				char pre_y[10];
				sprintf(pre_y, "%.3f", tran_2GY(v_grap[0].y));
				ps->send_msg(pre_y);

				char temp_id[10];
				sprintf(temp_id, "%d",  no_sim+1);
				ps->send_msg(temp_id);
				
				
				cout<<"Send to client"<<endl;
				rs232_idx = 1;
				
			}

			if(rs232_idx == 1){

			
				recv_num = atoi(ps->read_msg()); //// 1st reader
			
				//char endnote[] = "1";
				//int end = recv_num;
   
				cout<<"Send to clinet stage clear "<< recv_num <<endl;
							
				if(recv_num == 0){
					rs232_idx = 0;
				}

			}

			v_grap.clear();
		}

		cvShowImage("Grabimage_server", showimg);

		GUIInput = cvWaitKey(10);
		if(GUIInput=='p') break;

		if(GUIInput=='q') goto endstream;
	}

	choose_temp = cvCloneImage(src);
	cvShowImage("choose_temp",src);   
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
        check_line_state=true;  
    }  
    else if(check_line_state && event == CV_EVENT_MOUSEMOVE){  
        showtemp = cvCreateImage(cvGetSize(choose_temp), IPL_DEPTH_8U, 3);//use Imgshow to show that we draw a green rect in it    
        showtemp  = cvCloneImage(choose_temp);  
        p1=cvPoint(ROI_rect.x,ROI_rect.y);  
        p2=cvPoint(x,y);  
        cvRectangle(showtemp,p1,p2,CV_RGB(0,255,150),thickness,CV_AA,0);  
		
		cvShowImage("choose_temp",showtemp);   
        cvReleaseImage(& showtemp);   
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
        //cvNamedWindow("ROI",1);  
        //cvShowImage("ROI",ROIImg);  

		if(ROIImg != NULL){		
			temp_num++;

			char temp[50];
			char temp2[50] ;

			IplImage * client_temp = cvCreateImage(cvSize(ROIImg->width*sc_rate , ROIImg->height*sc_rate) , IPL_DEPTH_8U,3);
			cout<<"write file no. = "<<temp_num<<endl;
			sprintf(temp, "C://temp_img/server/temp_%d.jpg",temp_num);
			sprintf(temp2, "C://temp_img/client/temp_%d.jpg",temp_num);
			cvSaveImage(temp,  ROIImg);

			cvResize(ROIImg, client_temp);
			cvSaveImage(temp2,  client_temp);

			
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



float tran_2GX(int img_x){

	float gx = Ax*(4*img_x  + Bx);
	cout<<"gx ="<<gx<<endl;
	cout<<"px ="<<img_x<<endl;
	return gx;
}

float tran_2GY(int img_y){
	
	float gy = Ay*(4*img_y + By) ;
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
   
    cvWarpAffine(img,img_rotate, &M,CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS,cvScalarAll(0) );  
}  