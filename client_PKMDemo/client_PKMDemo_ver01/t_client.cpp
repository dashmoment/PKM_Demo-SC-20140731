#include "pipe_client.h"
#include"GrabImage.h"
#include "RS323_COM.h" 

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


RS232_COM* com = new RS232_COM;
pipe_client* pc = new pipe_client;

////****************camera param*********************
 
int img_width = 1280;
int img_height = 720;
int img_bpp = 24;

int DST_IMG_WIDTH = 640;
int DST_IMG_HEIGH = 360;

int iLastX = -1; 
int iLastY = -1;

HIDS hCam = 1;	
void * pMemVoid = NULL;
IplImage * img = cvCreateImage(cvSize(img_width, img_height), IPL_DEPTH_8U, 3);
IplImage * showimg = cvCreateImage(cvSize(DST_IMG_WIDTH, DST_IMG_HEIGH), IPL_DEPTH_8U, 3);
Mat grabimage;
char GUIInput ;


////***************Track_init***************************

char* recv_data;
CvPoint recv_p = cvPoint(-1,-1);
Rect roi_rect;
int temp_id;
IplImage *tempdata;
IplImage *roi_moment;

Mat roi;

CvPoint temp_center;

CvSize size;
IplImage * dstimg;

////***************Robot com******************

string pre_x , pre_y;

string send_x ="P197="; //General Command
string send_y ="P196=";
string send_z ="P193=-250";
string call_prog ="&1b23r";
string speed  = "F 400";
string TA  = "TA 20";
string write_com;
////**************CamShift*******************************
void Hist_and_Backproj(Mat temp_cam);

Mat hsv;
Mat hue;
int bins = 25;
void Hist_and_Backproj(Mat temp_cam);
Mat backproj;

Rect trackWindow;
Rect roi_cam;

int cam_idx = 0;
int cam_itr = 0;

Mat src2 = Mat(DST_IMG_WIDTH, DST_IMG_HEIGH , CV_8UC3);
Mat src2_hsv;
Mat src2_hue;

CvPoint final_grap;

char* send_data;

//******************Img2global param*****************

float Ax =  2.156;
float Bx = - 191.661;
float Ay = 1;
float By = -165.596;
float rw_y =  2.228;
float rh_x = 0.773;
float pre_bias;   //bias for moving direction


////****************Function*****************************

void on_mouse4(int event, int x,int y,int flags, void* param);
CvPoint tracking_moment(IplImage* treatedimg , IplImage* result_img);
float tran_2GX(int img_y);
float tran_2GY(int img_x);
 
int main(){

	com->init_port();
	string t_str = "resplc6"; ////back to home
	com->write_port(t_str);

	GrabImage *grab = new GrabImage(); 
	hCam = grab->InitCam(pMemVoid , hCam , img_width , img_height , img_bpp);
	
	cvNamedWindow( "Grabimage_client", 0);
	cvResizeWindow("Grabimage_client", 640 , 360);

	 
	int response;
	
	pc->client_init();   //// init pipe client


	//char msg[] = "test msg";
	//pc->send_msg(msg);
	

	//IplImage * roi_img = cvCreateImage(cvSize(roi_rect.width,roi_rect.height),8,3); 
	int response_idx = 0;

	while(true){

		double min, max;
		CvPoint mintemp;
		CvPoint maxtemp;

		grabimage = grab->Grabimg(pMemVoid , img , grabimage , hCam ,  img_width , img_height);
		
		resize(grabimage,src2 , Size(DST_IMG_WIDTH, DST_IMG_HEIGH)); //// Mat for camshift


		
		cvResize(img,showimg);         //// Iplimage*
		//cvSetImageROI(showimg , roi_rect);
		//cvCopy(showimg,roi_img);  
		//cvResetImageROI(showimg);   
		//cvShowImage("Grabimage_client", showimg);

		
		//roi = src2(roi_rect);
		//esize(grabimage,src2 , Size(roi_rect.width , roi_rect.height));

			////************************recv data***************************

	
		if(response_idx == 0){		
			for(int i = 0 ; i < 3 ; i++){   //// 1st receiver

				recv_data = pc->read_msg();

				if(i == 0){

					pre_x  = recv_data;
					cout<<"pre_x = " <<pre_x<<endl;

				}

				if(i == 1){

					pre_y  = recv_data;
					cout<<"pre_y = " <<pre_y<<endl;

				}
		

				if(i == 2){
					temp_id = atoi(recv_data);
					cout<<"temp_id = " <<temp_id<<endl;		
				
				}

		
			}
			response_idx = 1;
		}

		if(recv_data){

				////********move robot to predict pose********
			
				write_com = send_x+pre_x+send_y+pre_y+send_z+call_prog;

				com->write_port(write_com);
			
			
				////***********input temp**************(Id: check)

			
				char tempname[50];
				sprintf(tempname,"C://temp_img/client/temp_%d.jpg",temp_id);

				//cout<<"temp_id = " <<temp_id<<endl;		
				//cout<<tempname<<endl;

				int check = _access(tempname, 0);

			
				cout<<"Access = "<<check<<endl;   

				if(check == 0 ){
				
		
					tempdata = cvLoadImage(tempname , -1);
					CvSize size_t = cvGetSize(tempdata);
					 cvShowImage("temp",tempdata);
				


					roi_moment = cvCreateImage(size_t,IPL_DEPTH_8U,1);
					cvCvtColor(tempdata, roi_moment, CV_RGB2GRAY);
					temp_center = tracking_moment(roi_moment , tempdata);


					size.width = DST_IMG_WIDTH - tempdata->width  + 1;      
					size.height = DST_IMG_HEIGH - tempdata->height + 1;
					dstimg = cvCreateImage(size, IPL_DEPTH_32F, 1);  	


		

					cvMatchTemplate(showimg, tempdata , dstimg, CV_TM_CCOEFF_NORMED);	 //IplImage*	
					cvMinMaxLoc(dstimg, &min, &max, &mintemp, &maxtemp);

			
				cout<<"max = "<<max<<endl;

			}


			if(max > 0.75 ){

				cout<<"max ="<<max<<endl;

			//////**************CamShift***************************************


				if(cam_idx == 0){
					src2_hsv.create(src2.size() , src2.depth());
					cvtColor(src2 , src2_hsv , CV_BGR2HSV );

					int roi_x = maxtemp.x ;
					int roi_y = maxtemp.y ;
					int roi_width = tempdata->width ;
					int roi_height = tempdata->height;

					src2_hsv = src2_hsv(Rect(roi_x , roi_y , roi_width , roi_height));
					src2_hue.create(src2_hsv.size(), src2_hsv.depth());
					int ch[] = { 0, 0 };
					mixChannels( &src2_hsv, 1, &src2_hue, 1, ch, 1 );

					trackWindow.x =  roi_x ;
					trackWindow.y =  roi_y ;
					trackWindow.width = roi_width;
					trackWindow.height = roi_height;
					cam_idx = 1;

					//circle( src2 , cvPoint(trackWindow.x ,trackWindow.y ), 3 , Scalar(0,150,255), 1);	

				
				}

					if(cam_idx == 1 && src2_hue.rows != 0 && trackWindow.width != 0){

						if(cam_itr < 200){
							Hist_and_Backproj(src2_hue);
							RotatedRect trackBox = CamShift(backproj,trackWindow , TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 5, 1 ));
						

							//cout<<trackBox.size<<endl;
							trackBox.center.x= trackBox.center.x + maxtemp.x ;
							trackBox.center.y = trackBox.center.y + maxtemp.y;

							final_grap = cvPoint(trackBox.center.x , trackBox.center.y);

							if(trackBox.size.width > 0 && trackBox.size.height > 0){
								circle( src2 ,trackBox.center , 1, Scalar(0,0,255), 1);
								ellipse( src2 , trackBox, Scalar(0,0,255), 1, CV_AA );
							}

							cam_itr++;
							cout<<"cam_itr = "<<cam_itr<<endl;

							char pre_x[] = "1";
							pc->send_msg(pre_x);
								
							goto track;

						
							
						}

						else if(cam_itr == 200){
						
								cam_idx = 0;
								cam_itr = 0;
								recv_data = NULL;

								////trans final pose

								float final_gx;
								float final_gy;

								final_gx = tran_2GX(final_grap.y);
								final_gy = tran_2GY(final_grap.x);


								char final_x[10] = "";
								sprintf(final_x,"%.3f",final_gx);
								char final_y[10] = "";
								sprintf(final_y,"%.3f",final_gy);
							
						 
								write_com = send_x + final_x + send_y + final_y + send_z + call_prog;
								//com->write_port(write_com);

								////send finished to server from named pipe
								//char pre_x[] = "0";
								//ps->send_msg(pre_x);   ////1st sender.

								response_idx = 0;
								cout<<"track sucess + finished"<<endl;
							
					  }
				}
			}

			//else{
			//	string home_x = "0";
			//	string home_y = "0";
			//	write_com = send_x + home_x + send_y + home_y + send_z + call_prog;
			//	//com->write_port(write_com);

			//	////send finished to server from named pipe
			//	char endnote[] = "test";
			//	ps->send_msg(endnote);


			//	cout<<"track fail + finished"<<endl;
			//	response_idx = 0;
			//}

			cam_idx = 0;
			cam_itr = 0;
			char pre_x[] = "0";
			pc->send_msg(pre_x);
			response_idx = 0;

		}

track:
		imshow("Grabimage_client",src2);
		GUIInput = cvWaitKey(10);
		if(GUIInput=='q') break;
  }

	
	return 0;
}


void Hist_and_Backproj(Mat temp_cam){

	Mat hist;
	int histSize = MAX( bins, 2 );
	float hue_range[] = { 0, 100 };
	const float* ranges = { hue_range };

	 calcHist( &temp_cam, 1, 0, Mat(), hist, 1, &histSize, &ranges, true, false );
	 normalize( hist, hist, 0, 255, NORM_MINMAX, -1, Mat() );

	
	 calcBackProject( &temp_cam, 1, 0, hist, backproj, &ranges, 1, true );

	 imshow( "BackProj", backproj );


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

float tran_2GX(int img_y){

	float gx = Ax*rh_x*img_y + Bx;
	cout<<"gx ="<<gx<<endl;
	return gx;
}

float tran_2GY(int img_x){
	
	float gy = Ay*rw_y*img_x + By ;
	cout<<"gy ="<<gy<<endl;
	return gy;

}

