#pragma once
#include <cv.h>
class GMM3D
{
public:
    static const int componentsCount = 5;
	//GMM3D(void);
	 GMM3D( cv::Mat& _model );

	~GMM3D(void);

public:
	void InitLearning();

	//与二维上不同，三维上的color是voxel投到各个相机图像color的平均值 
	void AddSample( int ci, const cv::Vec3d color );  //void AddSample( int ci, const cv::Vec3d avrColor );  

	void EndLearning();

	int WhichComponent( const cv::Vec3d color ) const;

	double operator()(int ci, const cv::Vec3d color) const;
    
	double operator()( const cv::Vec3d color ) const;


private:
	void CalcInverseCovAndDeterm( int ci );
	cv::Mat model;
	double* coefs;
	double* mean;
	double* cov;

	double inverseCovs[componentsCount][3][3];
	double covDeterms[componentsCount];

	double sums[componentsCount][3];
	double prods[componentsCount][3][3];
	int sampleCounts[componentsCount];
	int totalSampleCount;


};

