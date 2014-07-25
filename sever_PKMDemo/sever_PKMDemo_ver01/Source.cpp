//#include"GrabImage.h"
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
//CvRect ROI_rect;   
//IplImage* dstimg = NULL;  
//IplImage* ROIImg = NULL;
//
//int img_width = 1280;
//int img_height = 720;
//
//int img_bpp = 24;
//
//int DST_IMG_WIDTH = 320;
//int DST_IMG_HEIGH = 180;
//
//int iLastX = -1; 
//int iLastY = -1;
//
//
//bool check_line_state=false; 
//void on_mouse4(int event, int x,int y,int flags, void* param);
//CvPoint tracking_moment(IplImage* treatedimg , IplImage* result_img);
//
//IplImage * img = cvCreateImage(cvSize(img_width, img_height), IPL_DEPTH_8U, 3);
//IplImage * showimg = cvCreateImage(cvSize(DST_IMG_WIDTH, DST_IMG_HEIGH), IPL_DEPTH_8U, 3);
//IplImage * choose_temp =  cvCreateImage(cvSize(img_width, img_height), IPL_DEPTH_8U, 3);
//IplImage * showtemp = cvCreateImage(cvSize(img_width, img_height), IPL_DEPTH_8U, 3);  
//IplImage * temp_img = NULL; 
//
//
//////**************Cam******************
//Mat hsv;
//Mat hue;
//int bins = 25;
//void Hist_and_Backproj(Mat temp_cam);
//Mat backproj;
//
//Rect trackWindow;
//
//int cam_idx = 0;
//
//Mat src2 = Mat(DST_IMG_WIDTH, DST_IMG_HEIGH , CV_8UC3);
//
//Mat src2_hsv;
//Mat src2_hue;
//int cam_itr = 0;
//
//vector<CvPoint> v_grap;
//vector<CvPoint> v_max;
//
//////*************MOG*******************
//
//
////Kalman
//
//
//
//
//int temp_num;
//
//
//int main(){
//	Mat test;
//	IplImage* roi_moment;
//	vector<CvPoint>temp_center;
//
//	HIDS hCam = 0;	
//	void * pMemVoid = NULL;
//	char GUIInput ;
//	GrabImage *grab = new GrabImage(); 
//	Mat grabimage;
//    cvNamedWindow( "Grab_Frame", 0);
//	cvvResizeWindow("Grab_Frame", 640 , 360);
//
//	cvNamedWindow( "track_Frame", 0);
//	cvvResizeWindow("track_Frame", 640 , 360);
//
//	
//	
//	hCam = grab->InitCam(pMemVoid , hCam , img_width , img_height , img_bpp);
//	
//	
//	CvPoint point2;
//
//	//check tmep	
//
//	double max_temp[2];
//
//	int kal_idx = 0;
//	
//start: //****************************************************************************
//
//	int checkfile = 1;
//	vector<IplImage*> tempdata;
//	while(true){
//		char tempname[20];
//		sprintf(tempname,"temp_%d.jpg",checkfile);
//
//		//cout<<tempname<<endl;
//		int check = _access(tempname, 0);
//
//		if(check == 0){
//			temp_img = cvLoadImage(tempname , -1);
//			tempdata.push_back(temp_img);
//			roi_moment = cvCreateImage(cvGetSize(temp_img),IPL_DEPTH_8U,1);
//			cvCvtColor(temp_img, roi_moment, CV_RGB2GRAY);
//			temp_center.push_back(tracking_moment(roi_moment , showimg));
//					
//
//			cout<<"Input temp no.= "<<checkfile<<endl;
//			temp_num = checkfile;
//			checkfile++;
//			//cvShowImage("tempshow",tempdata[0]);
//		}
//
//		else{
//			//cout<<"No such temp no.= "<<checkfile<<endl;q
//			break;
//		}
//	}
//
//	
//
//	while(true){
//		grabimage = grab->Grabimg(pMemVoid , img , grabimage , hCam ,  img_width , img_height);
//		cvResize(img,showimg);
//		resize(grabimage,src2 , Size(DST_IMG_WIDTH, DST_IMG_HEIGH));
//		GUIInput = cvWaitKey(10);
//
//		
//
//		//pMOG->operator()(grabimage , fgMaskMOG);
//
//		//imshow("Frame", grabimage);
//        //imshow("FG Mask MOG", fgMaskMOG);
//		//grabimage.copyTo(test , fgMaskMOG);
//		//imshow("test", test);
//			
//
//		if(tempdata.size() != 0){
//			max_temp[0] = 0;
//			int no_sim;
//			double min, max;
//			CvSize size;
//			CvPoint minLoc;
//			CvPoint maxLoc;
//			CvPoint mintemp;
//			CvPoint maxtemp;
//			CvPoint grab_p = cvPoint(-1,-1);
//			
//			
//
//			//cout<<grab_p.x<<endl;
//
//			
//
//			for(int i = 0 ; i < tempdata.size() ; i++){
//
//				size.width = DST_IMG_WIDTH - tempdata[i]->width  + 1;      
//				size.height = DST_IMG_HEIGH - tempdata[i]->height + 1;
//				dstimg = cvCreateImage(size, IPL_DEPTH_32F, 1);  	
//
//				cvMatchTemplate(showimg, tempdata[i] , dstimg, CV_TM_CCOEFF_NORMED);		
//				cvMinMaxLoc(dstimg, &min, &max, &mintemp, &maxtemp);
//
//				if(max > 0.6){
//
//					//max_temp[1] = max;			
//					//if(max_temp[1] >= max_temp[0]){
//
//						//max_temp[0] = max_temp[1];
//						minLoc = mintemp;
//						maxLoc = maxtemp;
//						no_sim = i;
//
//						//cout<<"Most sim temp = "<< no_sim + 1 <<endl;
//						//cout<<"Max = "<< max_temp[0] <<endl;
//
//						point2 = cvPoint(maxLoc.x+ tempdata[no_sim]->width, maxLoc.y+tempdata[no_sim]->height);			
//						cvRectangle(showimg, maxLoc, point2, cvScalar(0,255,150),1,CV_AA,0);		
//						
//						grab_p.x = temp_center[no_sim].x + maxLoc.x;
//						grab_p.y = temp_center[no_sim].y + maxLoc.y;
//
//						v_grap.push_back(grab_p);
//						v_max.push_back(maxLoc);
//						
//						for(int j = 0 ; j < v_grap.size() ; j++){
//							cvCircle( showimg , v_grap[j] , 3, Scalar(0,0,255), 1);
//						}
//					//}			
//				}
//
//			}
//		////**************CamShift***************************************
//			if(v_max[0].x >= 0 || cam_idx == 1){
//
//				//Mat m_temp = Mat(tempdata[no_sim] , 0);
//	
//				src2_hsv.create(src2.size(),src2.depth());
//
//				//src2_hsv = src2;
//				cvtColor(  src2 , src2_hsv , CV_BGR2HSV );
//
//				int roi_x = v_max[0].x - 10;
//				int roi_y = v_max[0].y - 10;
//				int roi_width = tempdata[no_sim]->width + 20;
//				int roi_height = tempdata[no_sim]->height + 20;
//
//				
//				src2_hsv = src2_hsv( Rect(roi_x , roi_y , roi_width , roi_height));
//				
//				src2_hue.create( src2_hsv.size(), src2_hsv.depth());
//				int ch[] = { 0, 0 };
//				
//				mixChannels( &src2_hsv, 1, &src2_hue, 1, ch, 1 );
//
//
//				if(cam_idx == 0){
//
//					trackWindow.x = v_max[0].x;
//					trackWindow.y = v_max[0].y;
//					trackWindow.width = tempdata[no_sim]->width;
//					trackWindow.height = tempdata[no_sim]->height;
//					cam_idx = 1;
//					//cout<<"Create target"<<endl;
//				}
//
//				if(cam_idx == 1 && src2_hue.rows != 0 && trackWindow.width != 0){
//
//					if(cam_itr < 5){
//
//						Hist_and_Backproj(src2_hue);
//						RotatedRect trackBox = CamShift(backproj,trackWindow , TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 5, 1 ));
//
//						CvPoint proj_grap;
//						trackBox.center.x= trackBox.center.x - trackWindow.width/2 + v_max[0].x +1;
//						trackBox.center.y = trackBox.center.y - trackWindow.height/2 + v_max[0].y +1;
//
//
//						circle( src2 ,proj_grap , 1, Scalar(0,0,255), 1);
//						ellipse( src2 , trackBox, Scalar(0,0,255), 2, CV_AA );
//					
//					}
//
//					else if(cam_itr == 5){
//						
//						cam_idx = 0;
//						cam_itr = 0;
//						v_grap.clear();
//						v_max.clear();
//						
//					}
//
//					cam_itr++;
//
//					//cout<<cam_itr<<endl;
//
//				}
//
//				
//				
//			}
//
//								
//			cvShowImage("Grab_Frame", showimg);
//			imshow("track_Frame", src2);
//
//		}
//
//		else{
//			
//			cvShowImage("Grab_Frame", showimg);
//		}
//
//
//		//Keyboard Action
//
//		if(GUIInput=='p') break;
//
//		if(GUIInput=='q') goto endstream;
//	}
//
//	choose_temp = cvCloneImage(showimg);
//	cvShowImage("choose_temp",choose_temp);   
//	cvSetMouseCallback("choose_temp",on_mouse4);  
//		
//	cvWaitKey(0);
//	
//	goto start;
//	  
//	endstream: //****************************************************************************
//
//
//	cvReleaseImage(&showimg);
//
//	return 0;
//
//}
//
//void on_mouse4(int event, int x,int y,int flags, void* param){  
//    int thickness=2;  
//    CvPoint p1,p2;
//	char temp[20];
//
//    if(event == CV_EVENT_LBUTTONDOWN){    //left button of mouse is down  
//        ROI_rect.x=x;  
//        ROI_rect.y=y;  
//        check_line_state=true;  
//    }  
//    else if(check_line_state && event == CV_EVENT_MOUSEMOVE){  
//        showtemp = cvCreateImage(cvGetSize(choose_temp), IPL_DEPTH_8U, 3);//use Imgshow to show that we draw a green rect in it    
//        showtemp  = cvCloneImage(choose_temp);  
//        p1=cvPoint(ROI_rect.x,ROI_rect.y);  
//        p2=cvPoint(x,y);  
//        cvRectangle(showtemp,p1,p2,CV_RGB(0,255,150),thickness,CV_AA,0);  
//        cvShowImage("choose_temp",showtemp);   
//        cvReleaseImage(& showtemp);   
//    }  
//    else if(check_line_state && event == CV_EVENT_LBUTTONUP){ 
//		
//		cvDestroyWindow("choose_temp");
//
//        ROI_rect.width = abs(x - ROI_rect.x);   
//        ROI_rect.height = abs(y - ROI_rect.y);
//
//        cvSetImageROI(choose_temp, ROI_rect);   
//        
//        IplImage* dstImg;  
//        
//        ROIImg = cvCreateImage(cvSize(ROI_rect.width,ROI_rect.height),8,3);  
//		
//        cvCopy(choose_temp,ROIImg);  
//        cvResetImageROI(showimg);  
//        cvNamedWindow("ROI",1);  
//        cvShowImage("ROI",ROIImg);  
//
//		if(ROIImg != NULL){		
//			temp_num++;
//			cout<<"write file no. = "<<temp_num<<endl;
//			sprintf(temp, "temp_%d.jpg",temp_num);
//			cvSaveImage(temp,  ROIImg);
//			cvReleaseImage(& ROIImg);
//
//			//cvWaitKey(20);
//		}
//        
//      
//    }    
//}  
//
//CvPoint tracking_moment(IplImage* treatedimg , IplImage* result_img){
//
//	CvPoint center;
//	CvMoments oMoments;
//	cvMoments(treatedimg,&oMoments,1);
//
//	double dM01 = cvGetSpatialMoment(&oMoments , 0 , 1);
//	double dM10 = cvGetSpatialMoment(&oMoments , 1 , 0);
//	double dArea = cvGetSpatialMoment(&oMoments , 0 , 0);
//	
//	int posX = dM10 / dArea;
//	int posY = dM01 / dArea;  
//	center.x = posX;
//	center.y = posY;
//	
//
//     return center;
//}
//
//
//
//
//
//
//
//void Hist_and_Backproj(Mat temp_cam){
//
//	Mat hist;
//	int histSize = MAX( bins, 2 );
//	float hue_range[] = { 0, 100 };
//	const float* ranges = { hue_range };
//
//	 calcHist( &temp_cam, 1, 0, Mat(), hist, 1, &histSize, &ranges, true, false );
//	 normalize( hist, hist, 0, 255, NORM_MINMAX, -1, Mat() );
//
//	
//	 calcBackProject( &temp_cam, 1, 0, hist, backproj, &ranges, 1, true );
//
//	 imshow( "BackProj", backproj );
//
//
//}
