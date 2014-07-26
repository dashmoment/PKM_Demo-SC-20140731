#include <tchar.h>
#include <cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <ml.h>
#include <iostream>

using namespace std;
using namespace cv;

CvFileStorage *cvrs;

int main(){

	char mat_name[20];
	IplImage *Ipl_read_mat;
	vector<Mat>read_mat;
	vector<Mat>pre_pca;
	
	int numofdata = 300;

	

	cvrs = cvOpenFileStorage("tt01.xml",0,CV_STORAGE_READ); 	

	for(int i = 0 ; i < numofdata ; i++ ){ 
		
		sprintf(mat_name,"test%d",i);
		Ipl_read_mat = (IplImage*)cvReadByName(cvrs,NULL,mat_name); 

		Mat temp_mat(Ipl_read_mat,0);

		cvtColor(temp_mat,temp_mat,CV_BGR2GRAY);
	
		read_mat.push_back(temp_mat);

		//cout<<temp_mat.at<int>(10,10)<<endl;

	}

	 //cout<<read_mat.size()<<endl;
	//Visuallization
	/*normalize(read_mat[11] , read_mat[11], 0, 255, NORM_MINMAX, CV_8UC1);
	cvNamedWindow("read_mat" , 0);
	imshow("read_mat" , read_mat[11]);*/

	//cout<<read_mat.size()<<endl;

	for(int i = 0 ; i < read_mat.size() ; i++){

		Mat temp( read_mat[i].rows,read_mat[i].cols,CV_32FC1);
		
		read_mat[i].convertTo(temp,CV_32FC1);

		pre_pca.push_back(temp.reshape(1,1));

		//cout<<temp.at<float>(0.0)<<endl;

	}


	Mat label(pre_pca.size(),1,CV_32FC1);

	Mat pca_in(pre_pca.size() , pre_pca[0].cols , CV_32FC1);

	float temp;

	//cout<<pre_pca.size()<<endl;

	for(int i = 0 ; i < pre_pca.size() ; i++){

		if(i < 100) label.at<float>(i,0) = 1; 

		else if( i >= 100 && i <  200 ) label.at<float>(i,0) = 2;

		else if( i >= 200 && i < 300 ) label.at<float>(i,0) = 1;
		
		for(int j = 0 ; j < pre_pca[0].cols ; j++){

			pca_in.at<float>(i,j) = pre_pca[i].at<float>(0,j);
			
			//cout<<pca_in.at<float>(i,j)<<endl;
		}
		//cout<<pre_pca[0].size()<<endl;
		//cout<<pca_in.row(i).size()<<endl;

		//cout<<pca_in.at<float>(0,48)<<endl;
		//cout<<pre_pca[0].at<float>(0,48)<<endl;
		
	}

	

	
//PCA
	int number_principal_compent = 9;

	PCA pca(pca_in, Mat(), CV_COVAR_ROWS  , number_principal_compent);


	Mat data_proj(pca_in.rows , number_principal_compent , CV_32FC1);

	for(int i = 0 ; i < pca_in.rows ; i++){

		pca.project(pca_in.row(i) , data_proj.row(i));

		//cout<<pca_in.at<float>(i,1)<<endl;
		//cout<<data_proj.at<float>(i,1)<<endl;

	}

//validation	

	vector<Mat>valid_mat;
	vector<Mat>pre_pca_valid;
	char valid[20] = "valid_u.xml";

	cvrs = cvOpenFileStorage(valid,0,CV_STORAGE_READ); 

	for(int i = 0 ; i < 10 ; i++ ){ 
		
		sprintf(mat_name,"test%d",i);

		Ipl_read_mat = (IplImage*)cvReadByName(cvrs,NULL,mat_name); 

		Mat temp_mat(Ipl_read_mat,0);

		cvtColor(temp_mat,temp_mat,CV_BGR2GRAY);
	
		valid_mat.push_back(temp_mat);


	}

	/*normalize(valid_mat[1], valid_mat[1], 0, 255, NORM_MINMAX, CV_8UC1);
	cvNamedWindow("val_mat" , 0);
	imshow("val_mat" , valid_mat[1]);*/

	for(int i = 0 ; i < valid_mat.size() ; i++){

		Mat temp( valid_mat[i].rows,valid_mat[i].cols,CV_32FC1);
		
		valid_mat[i].convertTo(temp,CV_32FC1);

		pre_pca_valid.push_back(temp.reshape(1,1));

		//cout<<temp.at<float>(1.0)<<endl;

	}

	Mat pca_in_valid(pre_pca_valid.size() , pre_pca_valid[0].cols , CV_32FC1);

	for(int i = 0 ; i < pre_pca_valid.size() ; i++){

		for(int j = 0 ; j < pre_pca_valid[0].cols ; j++){
				pca_in_valid.at<float>(i,j) = pre_pca_valid[i].at<float>(0,j);
			

				//cout<<pca_in_valid.at<float>(i,j)<<endl;
			}
	}

	

	PCA pca2(pca_in_valid, Mat(), CV_COVAR_ROWS  , number_principal_compent);


	Mat valid_proj(pca_in_valid.rows , number_principal_compent , CV_32FC1);

	for(int i = 0 ; i < pca_in_valid.rows ; i++){

		pca2.project(pca_in_valid.row(i) , valid_proj.row(i));

		//cout<<pca_in.at<float>(i,1)<<endl;
		//cout<<data_proj.at<float>(i,1)<<endl;

	}

	//SVM

	CvSVMParams params;
	CvSVM svm;

	params.kernel_type = CvSVM::RBF;
	params.svm_type = CvSVM::C_SVC;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 1000, 1e-6);
	//params.C = 0.7; 

	svm.train(data_proj , label , cv::Mat() , cv::Mat(),params);



	for (int i = 0 ; i < 100 ; i++){

		int sampl_row = i;
		Mat sampleMat = data_proj.row(sampl_row);
		float response = svm.predict(sampleMat);

		if(label.at<float>(sampl_row,0) != response){
			cout<<"predict error"<<endl;
		}

	}

	
	for (int i = 0 ; i < 10 ; i++){
		
		int sampl_row = i;
		Mat sampleMat = valid_proj.row(sampl_row);
		float response = svm.predict(sampleMat);

		//cout<<"label of sample_row = "<<label.at<float>(sampl_row,0)<<endl;
		cout<<"valid Predict = "<<response<<endl;
	}

	svm.save("classifier.xml");

	cvWaitKey(0);

	

}

