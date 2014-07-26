
#include <tchar.h>
#include <cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <ml.h>
#include <iostream>

using namespace std;
using namespace cv;

#define TRAIN_SAMPLE_COUNT 50
#define SIGMA 60

class machinelearning{

private:


public: 
	
	vector<Mat>readfile(CvFileStorage *cvfs, CvFileStorage *cvrs , int mat_n  , char *filename);
	Mat normalize_forPCA(Mat src);
	Mat parse_PCA(vector<Mat> src);
	Mat combinedata(Mat src);
	int SVM_model(Mat data); 

	CvMat *cv_parse_PCA(Mat src);
	
	~machinelearning();
	  
};


vector<Mat> machinelearning::readfile(CvFileStorage *cvfs, CvFileStorage *cvrs , int mat_n   , char *filename){
	
	char mat_name[20];
	IplImage *Ipl_read_mat;
	vector<Mat>read_mat;
	int numofdata = mat_n;


	for(int i = 0 ; i < numofdata ; i++ ){ 
		
		sprintf(mat_name,"test%d",i);
		Ipl_read_mat = (IplImage*)cvReadByName(cvrs,NULL,mat_name); 
		Mat temp_mat(Ipl_read_mat,0);

		cvtColor(temp_mat,temp_mat,CV_BGR2GRAY);
	
		read_mat.push_back(temp_mat);


		
		//cout<<"temp"<<temp_mat<<endl;
		//cout<<"in_mat"<<read_mat[i]<<endl;
	}

	imshow("temp_mat",read_mat[0]);

	cvReleaseFileStorage(&cvrs);  
	cvReleaseImage(&Ipl_read_mat);

	//cout<<"in_mat = "<<read_mat.size()<<endl;
	return read_mat;

}

Mat machinelearning::normalize_forPCA(Mat src){
	Mat srcnorm;

	//cvtColor(src,src,CV_BGR2GRAY);
	normalize(src, srcnorm, 0, 255, NORM_MINMAX, CV_8UC1);

	//cout<<"srcnorm.channels = "<<srcnorm.channels()<<endl;
	return srcnorm;
}

Mat machinelearning::parse_PCA(vector<Mat> src){

	

	//cvtColor(src,src,CV_BGR2GRAY);
	

	int total = src[0].rows*src[0].cols;
	Mat temp;
	Mat PCA_result;
	Mat test_temp; 

	Mat data (src.size() , total, CV_32FC1 );

	//cout<<"data.size = "<<data.size()<<endl;
	//cout<<"src.size = "<<src.size()<<endl;

	

	for(int i = 0; i < src.size() ; i++){

		Mat col_tmp = data.row(i);

		src[i].reshape(1,1).convertTo(col_tmp , CV_32FC1);	 

		//cout<<data.row(i).at<float>(0,1)<<endl;
	}
	
	
	

	int number_principal_compent = 9;

	PCA pca(data, Mat(), CV_COVAR_ROWS  , number_principal_compent);
	Mat project_dst(data.rows , number_principal_compent , CV_32FC1);
	
	for(int i = 0 ; i < data.rows ; i++){
		pca.project(data.row(i) , project_dst.row(i));

		//cout<<data.at<float>(i,1)<<endl;
		//cout<<project_dst.at<float>(i,1)<<endl;
	}

	Mat pca_r = normalize_forPCA(pca.eigenvectors.row(0).reshape(1 , src[0].rows));
	Mat project_r = normalize_forPCA(pca.project(data).row(0));

	//cvNamedWindow( "pca_r", 0);
	//imshow("pca_r",pca_r);
	//imshow("project_r",project_r);
	
	//cout<<project_r<<endl;
	

	return project_dst;
	
}


int machinelearning::SVM_model(Mat data) {

	//Mat image = Mat::zeros(10, 10, CV_8UC3);
	//Vec3b green(0,255,0), blue (255,0,0);

	CvSVMParams params;

	vector<float> class_data;

	int num_data = 0;
	int class_l = 1;

	for(int i = 0 ; i < data.rows ; i++){

		class_data.push_back(class_l);
		num_data++;

		if(num_data == 10){
			num_data = 0;
			class_l++;
		}
		
	}

	cout<<"class_data = "<<class_data.size()<<endl;
	cout<<"data.row = "<< data.rows<<endl;

	Mat labelsMat(data.rows , 1 , CV_32FC1);

	for(int i = 0 ; i < data.rows ; i++){

		labelsMat.at<float>(i,0) = class_data[i];
		cout<<labelsMat.at<float>(i,0)<<endl;
	}



	if(data.rows > 10){
	
		params.kernel_type = CvSVM::RBF;
		params.svm_type = CvSVM::C_SVC;
		params.C = 0.1;
		//params.term_crit=cvTermCriteria(CV_TERMCRIT_ITER,100,0.000001);

		cout<<data.at<float>(0,0)<<endl;
		//cout<<labelsMat.size()<<endl;

		CvSVM svm;
		bool res = svm.train(data,labelsMat,cv::Mat(),cv::Mat(),params);

		for (int i = 0 ; i <20 ; i++){

			int sampl_row = i;
	
			Mat sampleMat = data.row(sampl_row);
			float response = svm.predict(sampleMat);

			cout<<"label of sample_row = "<<labelsMat.at<float>(sampl_row,0)<<endl;
			cout<<"Predict = "<<response<<endl;
		}

		
		svm.save("classifier.xml");
	}

	return 1;

}



CvMat *machinelearning::cv_parse_PCA(Mat src){
	
	Mat data;

	src.reshape(1,1).convertTo(data,CV_32FC1,1/255.);

	cout<<data.size()<<endl;
	 
	 IplImage temp;
	 CvMat *Vector1 = cvCreateMat(data.rows,data.cols,CV_32FC1);
     CvMat *AvgVector = cvCreateMat(1,1200,CV_32FC1);;
     CvMat *EigenValue_Row = cvCreateMat(1,1,CV_32FC1);
	 CvMat *EigenVector = cvCreateMat(1,1200,CV_32FC1);

	 for(int i = 0 ; i < data.rows ; i++){ 
		 for(int j = 0 ; j < data.cols ; j++){
			 cvmSet(Vector1,i,j,(double)data.at<float>(i,j));
			//cout<<i<<endl;
		 }
	 }

	// cout<<"writed "<<endl;


	 //cvCalcPCA(Vector1,AvgVector,EigenValue_Row,EigenVector,CV_PCA_DATA_AS_ROW);

	 return Vector1;


}

