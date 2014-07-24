#include "ueye.h"
#include <uEye_tools.h>
#include <ueye_deprecated.h>
#include <tchar.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

class GrabImage{

public:
	
	//IplImage * img;
	char* imgMem;
	int memId;
	int nRet;


	HIDS InitCam(void * pMemVoid , HIDS hCam ,int img_width , int img_height , int img_bpp);
	Mat Grabimg(void * pMemVoid , IplImage * img , Mat grabimage , HIDS hCam , int img_width , int img_height);


	~GrabImage();


};

HIDS GrabImage::InitCam(void * pMemVoid , HIDS hCam , int img_width , int img_height , int img_bpp){
	
	//HIDS hCam = 0;
	
	
    
	nRet = is_InitCamera (&hCam, NULL);

	if(nRet!= IS_SUCCESS){

	   cout<<"Fail to Init"<<endl;
	   //return 0;
    }

	cout<<"Success to Init"<<endl;
	is_AllocImageMem(hCam, img_width, img_height, img_bpp, &imgMem, &memId);
    is_SetImageMem (hCam, imgMem, memId);
    is_SetDisplayMode (hCam, IS_SET_DM_DIB);
    is_SetColorMode (hCam, IS_CM_RGB8_PACKED);
    is_SetImageSize (hCam, img_width, img_height);
	
	

	return hCam;

	 /*
     double enable = 1;
     double disable = 0;
     is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_GAIN, &enable, 0);
     is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_WHITEBALANCE, &enable, 0);
     is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_FRAMERATE, &disable, 0);
     is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_SHUTTER, &disable, 0);
     is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_SENSOR_GAIN, &enable, 0);
     is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_SENSOR_WHITEBALANCE,&enable,0);
     is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_SENSOR_SHUTTER, &disable, 0);

*/
 
}


Mat GrabImage::Grabimg(void * pMemVoid, IplImage * img , Mat grabimage , HIDS hCam , int img_width , int img_height){


	is_FreezeVideo(hCam, IS_WAIT);
	is_GetImageMem (hCam, &pMemVoid);

	if(pMemVoid != NULL){
		  
		img->nChannels=3;
		img->depth=8;
		img->width=img_width;
		img->height=img_height;
		img->imageData=(char*)pMemVoid; 
		img->imageDataOrigin=(char*)pMemVoid;

		grabimage = Mat(img , true);

		//cout<<grabimage.size()<<endl;
		return grabimage;
	}

	else{

		cout<<"Fail to is_GetImage"<<endl;
		
	}

	//img->nSize=112;
    //img->ID=0;           
    //img->alphaChannel=0;         
    //img->dataOrder=0;
    //img->origin=0;
    //img->align=4;
           

}


