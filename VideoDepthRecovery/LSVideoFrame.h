#pragma once
#include <iostream>
#include <string>
#include <vector>


# define CamNum 20     //camera numbers in system: according the practial use change

#include "LSCameraParameters.h"
#include "VisualHull/SoftRasterize.h"
//CxImage
#include "ximage.h"
#include "ZImage.h"
#include "ZImageUtil.h"

#include "WmlMatrix4.h"
#include "wmlmatrix2.h"

//OpenCV
#include <video/background_segm.hpp>
#include <highgui/highgui.hpp>
#include <cvaux.h>
#include <opencv2/opencv.hpp> 
//#include <opencv2/objdetect/objdetect.hpp>
//#include "D:\Program Files\OpenCV2.4.10\opencv\build\include\opencv2\objdetect\objdetect.hpp"
#include "GMM.h"
class LSVideoFrame
{
public:
	enum LABEL_IMG_TYPE{    
		INIT,
		BO,
		DE,
		DATACOST_Init,
		DATACOST_BO,
		DATACOST_OPT,
		MAXDSP,
		MINDSP,
		BO_DE,
		INIT_DE,
		INIT_SEG,
		OPT,
		OPT_DE
	};
	LSVideoFrame(void);
	~LSVideoFrame(void);
	void Clear();    //make the all sort of images point to Null

	// get pixel color value at (x,y) coord
	void GetColorAt(int x, int y, Wml::Vector3f& out_Color);

	void GetColorAt(float x, float y, Wml::Vector3f& out_Color );


	// get fore optical flow elm at （x,y)
    void GetForeOptAt(double x, double y, Wml::Vector2d& out_ForeOpt );



	void GetBackOptAt(double x, double y, Wml::Vector2d& out_BackOpt );
	/**********************************************************************/
	/*** Camera Parameters 
	/**********************************************************************/

	void SetCameraParaK(Wml::Matrix3d& K){
		m_pLsCameraParameter->m_K = K;
	}


	// set element of Intrinsic parameters K
	void SetCameraParaKAt(int r, int c, double f)
	{
		m_pLsCameraParameter->m_K= f;
	}
	//get element of Intrinsic parameters K
	double GetCameraParaKAt(int r, int c){
		return m_pLsCameraParameter->m_K(r, c);
	}

	//set element of Extrinsic parameters R
	void SetCameraParaRAt(int r, int c, double f){
		m_pLsCameraParameter->m_R(r, c) = f;
	}
	//get element of Extrinsic parameters R
	double GetCameraParaRAt(int r, int c){
		return m_pLsCameraParameter->m_R(r, c);
	}
	//set element of Extrinsic parameters T
	void SetCameraParaTAt(int index, double f){
		m_pLsCameraParameter->m_T[index] = f;
	}
	//get element of Extrinsic parameters T
	double GetCameraParaTAt(int index){
		return m_pLsCameraParameter->m_T[index];
	}
	
	//Init camera position
	void InitCameraPos(){
		m_pLsCameraParameter->InitCameraPos();
	}
	//get camera postion
	Wml::Vector3d GetCameraPos(){
		return m_pLsCameraParameter->GetCameraPos();
	}


	//
	Wml::Vector3d GetCameraDir(){
		return m_pLsCameraParameter->m_R.GetRow(2);
	}



	void GetKXR(Wml::Matrix3d& KXR){
		m_pLsCameraParameter->GetKXR(KXR);
	}

	// coord transfer

	void GetCameraCoordFrmWorldCoord(Wml::Vector3d& c_pt3d, Wml::Vector3d& w_pt3d);

	void GetWorldCoordFrmImgCoord(double u, double v, double dsp, Wml::Vector3d& w_pt3d);
	   
	 // world coord to image coord
	void GetImgCoordFrmWorldCoord(double& out_u, double& out_v, double& out_dsp, Wml::Vector3d& w_pt3d);


	/***********************************************/
	        /** Related to all kinds of images
	/***********************************************/

	//load mask image
//	 bool LoadMaskImg(int corrode = 0);  //ORG

	 bool LoadMaskImg(); //NEW ADD  

	 
     bool GenerateMaskImgFromBGS(); //Produce mask image throUgh Background Subtraction method (with background image)

	 bool GenerateMaskImgFromGMMBGS();   //Produce mask image throUgh Background Subtraction method (without background image)

	 bool InitMaskImgFromGMM(GMM &bgdGMM,GMM &fgdGMM);  //new-add 通过单个GMM对第0帧进行初始化求出mask.  5/19/2015

	 bool GetMaskImgFromInitGMM(GMM &bgdGMM,GMM &fgdGMM);   //new-add 通过第0帧的前背景学习后续帧的mask


	 bool GenerateRectFromHogDetect();  //Generate Rectangle from Hog feature Detect(seedball based method)


	 //NEW ADD
	 bool LoadMinLabelImg();
	 bool LoadMaxLabelImg();

	 //NEW ADD 
	 //
	 void GenerateVariance(ZByteImage& Img,ZFloatImage& varianceImg,int halfWin = 2);


	 //load color(source) image
	 bool LoadColorImg();	

	 //load depth image
	 void LoadDepthImg();




	 //Load OpticalFlow image
	 void LoadForeOptFlwImg();
	 void LoadBackOptFlwImg();
	
	 // clear depth image
	 void Clear_depth();

     //set dsp image
	 void SetDspImg(ZIntImage& labelImg, int trueX1, int trueY1, int trueX2, int trueY2, int dspLevelCount);

	//void DrawMaskRect()

	void SetMaskRectangle(int minX, int minY, int maxX, int maxY){
		m_iMaskMinX = minX;
		m_iMaskMinY = minY;
		m_iMaskMaxX = maxX;
		m_iMaskMaxY = maxY;
	}
	void GetMaskRectangle(int& minX, int& minY, int& maxX, int& maxY){
		minX = m_iMaskMinX;
		minY = m_iMaskMinY;
		maxX = m_iMaskMaxX;
		maxY = m_iMaskMaxY;
	}


	//for seed ball based method
	void GetEnlargeMaskRectangle(int& minX, int& minY, int& maxX, int& maxY,int elgrSize)
	{
		
		minX=(m_iMaskMinX-elgrSize)<0? 0:(m_iMaskMinX-elgrSize);
		//minX=std::min(0,(m_iMaskMinX-elgrSize));
		minY=(m_iMaskMinY-elgrSize)<0? 0:(m_iMaskMinY-elgrSize);
		maxX=(m_iMaskMaxX+elgrSize)<(m_iWidth-1)?(m_iMaskMaxX+elgrSize):(m_iWidth-1);
		maxY=(m_iMaskMaxY+elgrSize)<(m_iHeight-1)?(m_iMaskMaxY+elgrSize):(m_iHeight-1);
		//std::cout<<minX<<","<<minY<<","<<maxX<<","<<maxY<<std::endl;

	}



	// set dsp range according boundingBox
	void SetDspRange(float minDsp, float maxDsp){
		m_fMinDsp = minDsp;
		m_fMaxDsp = maxDsp;
	}

	//get dsp range
	void GetDspRange(float& minDsp, float& maxDsp){
		minDsp = m_fMinDsp;
		maxDsp = m_fMaxDsp;
	}

	//init min dsp image according visualhull
	void InitMinDspImg(float value);

	//init max dsp image according visualhull
	void InitMaxDspImg(float value);



	//init min depth image according visualhull
	void InitMinDepthImg(float value);           //new add

	//init max depth image according visualhull
	void InitMaxDepthImg(float value);          //new add



	//get dsp at (x,y) coord consider mask
	float GetDspAt(double x, double y, bool considerMask = false);
    
	 //get dsp at(x,y)
	float GetDspAt(int x, int y);

	// set dsp at(x,y)
	void SetDspAt(int x, int y, float dspValue);


public:

    //init disparity constrain according VisualHull(get the maxdepth,mindepth in this view)
	void InitDspConstrain(std::vector<IdxTraingle>& TriangleList, std::vector<Wml::Vector3f>& VertexList);


	// set the TempDir point to which camId
	static void SetTmpDirectory(int cameraIndex, const std::string& tempDir)
	{
		s_TempDir[cameraIndex] = tempDir;
	}
	
	// set the DataDir point to which camId
	static void SetDataDirectory(int cameraIndex, const std::string& dataDir)
	{
		s_DataDir[cameraIndex] = dataDir;
	}


	// set the RawDir point to which camId
	static void SetRawDirectory(int cameraIndex, const std::string& rawDir)
	{
		s_RawDir[cameraIndex] = rawDir;
	}


	static void SetSegDirectory(int cameraIndex, const std::string& segDir)
	{
		s_SegDir[cameraIndex] = segDir;
	}

	//new add_for optical flow
    static void SetOptFlwDirectory(int cameraIndex, const std::string& optDir )
	{
		s_OptFlwDir[cameraIndex] = optDir;
	}


	//set image size (width, height)
	void SetImgSize(int width, int height){
		m_iWidth = width;
		m_iHeight = height;
	}

	//return image width
	static int GetImgWidth(){
		//if(s_bScaled)
		//	return s_iWidth * s_fScale  + 0.5F;
		return m_iWidth;
	}

	//return image height
	static int GetImgHeight(){
		//if(s_bScaled)
		//	return s_iHeight * s_fScale  + 0.5F;
		return m_iHeight;
	}


	//VisualHull
	// set for every frame wether their six face is visable
	void SetFaceVis(Wml::Vector3d* faceNormal);

	bool GetFaceVis(int i)
	{
		if(m_FaceVis)
			return m_FaceVis[i];
		else return false;
	}


	bool IsInMask(Wml::Vector3d& w_pt3d, int Apron);

	bool IsInMask(int u, int v);


	//NEW ADD
	//将三位中点（这里的中点是在投射到的二维图像近似取的，而并非在三维图像上取，是否欠妥？）
	//投射到这个图片中，看是否在Mask内,这个函数必须在SetupView函数已经被调用之后使用之后
	bool inMask(float x,float y,float z,int Apron = 0);
	void setup(LSVideoFrame* cameraFrame);




	// Init label range by dsp range( according visualhull)
	void InitLabelRangeByDspRange( int layerCount, float  VisualHullConstrianSigma, bool SaveImg);

	//save label image
	void SaveLabelImg(ZIntImage& labelImg, LABEL_IMG_TYPE type, int labelCount, bool showMask = false);

    //save model as obj format
	void SaveModel(bool considerMask);

	//Get SegmSavePath
	std::string GetSegmSavePath(LSVideoFrame* pFrm );



	//Init label image by Dsparity image
	void InitLabelImgByDspImg(ZIntImage& labelImg, int dspLevelCount);


	//save disparity image
	void SaveDspImg();	


private:
	// for Box caculator,initialization is -1 in constructor
	int m_iMaskMinX;
	int m_iMaskMinY;
	int m_iMaskMaxX;
	int m_iMaskMaxY;

public:
	int m_iCameraId;      //camId
	int m_iFrameId;	      //frameId
	
	ZByteImage* m_pColorImg; //color image
	ZByteImage* m_pMaskImg;  //mask image
	ZFloatImage* m_pDspImg;  //disparity image

	ZFloatImage* m_pForeOptImg;  //fore optical flow image
	ZFloatImage* m_pBackOptImg;  //back optical flow image
    
	cv::Mat m_mCastImg;    //cast voxel from VisualHull to Image mask  //Tmp
	cv::Mat m_mMaskImg;   //mask image represent by Mat of OpenCV
	cv::Mat m_mColorImg;  //color image represent by Mat of OPenCV
	cv::Mat m_mBgdSubImg;  //background subtraction image represent by Mat of OpenCV


	std::string m_sColorImgPathName;    //source Image path
	std::string m_sMaskImgPathName;     //mask image path
	std::string m_sName;                //image name (number defined)
   
	std::string m_sCastImgPathName;  //cast image path
    std::string m_sBgdSubImgPathName; //background subtraction image path 



	static std::string s_DataDir[CamNum];   //dataDir for all cam
    static std::string s_TempDir[CamNum];   //TempDir for all cam
	static std::string s_RawDir[CamNum];   //TempDir for all cam
	static std::string s_SegDir[CamNum];
    static std::string s_OptFlwDir[CamNum];



	// for depth recovery
	ZFloatImage* m_pMinDspImg;     // min dsp image
	ZFloatImage* m_pMaxDspImg;     // max dsp image

	ZFloatImage* m_pDepthMax;       //new add
	ZFloatImage* m_pDepthMin;      // new add

	ZFloatImage* m_pVarianceMap;      // new add

  

	ZIntImage* m_pMinLabelImg;     // min label image
	ZIntImage* m_pMaxLabelImg;     // max label image

private:
	LSCameraParameters * m_pLsCameraParameter;  // a pointer pointed to  object of LSCameraParameters


	static int m_iWidth;      //image width
	static int m_iHeight;     //image height

    unsigned char  LEASTFRONT;   //用来对mask图像进行处理，具体含义尚不清楚

	float m_fMinDsp;  // min dsp value on this frame
	float m_fMaxDsp;  // max dsp value on this frame 


	//VisulHull
	bool m_FaceVis[6];//这里所有Voxel的Face的法向量只可能是六个，而这个数组测试了这些法向量对Viewi是否可见
	//六个表面顺序是垂直于x轴的近远两个平面，垂直于y轴的近远两个平面，垂直于z轴的近远两个平面
	bool* m_pMaskMap;  //NEW ADD
	LSVideoFrame* m_pCameraFrame;
};

