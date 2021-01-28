#pragma once
#include "WmlMatrix3.h"
#include "CameraParameters.h"
#include "MatchLinker.h"
#include "ZImage.h"
#include "ximage.h"
#include <vector>
#include "ZImageUtil.h"
#include <string>
#include <iostream>

#include "Block.h"

class VDRVideoFrame
{
public:
	/*  获取静态实例，便于外部引用
	static VDRVideoFrame GetInstance()
	{
		static VDRVideoFrame instance;
		return &instance;
	}
	*/

	VDRVideoFrame(const Wml::Matrix3d &K, const Wml::Matrix3d &Scaled_K);

	~VDRVideoFrame(void);

	 
	 friend class VDRStructureMotion;

 
 void GetColorAt(int x,int y,Wml::Vector3d &out_color);

 //得到（x,y)位置的diaprity值
 float GetDspAt(int x, int y);
 double GetDspAt(double x, double y);


//设置（x,y)位置的disparity
void SetDspAt(int x, int y, float DspValue);


//清空（图像占用）内存
 void Clear();

 //设置图像disparity
 void SetDspImg(ZIntImage& labelImg, const Block &block, double dspMin, double dspMax, int dspLevelCount);

 void SaveDspImg();

 //加载深度图
 bool LoadDepthImg();
 bool LoadDepthImg(ZFloatImage *&dspImg);

 bool LoadDepthImg_small(ZFloatImage* &p);
 bool LoadDepthImg_normal(ZFloatImage* &p);




private:
    //存储disparity图像
	void SaveDspImg(ZFloatImage* p, const std::string &path);
	
	//获取disparity图像的存储路径
	std::string GetDspImgPath_samll();
	std::string GetDspImgPath_normal();




public:
   int m_iID;   //帧号
   std::string m_sImagePathName;   //每帧图像路径名称（包含图像名称）
   std::string m_sName;
   
	
   //track(用于跟踪）
   std::shared_ptr<std::vector<MatchPoint>> m_pvMatchPoints;//存储某一帧上所有跟踪点

   //设置lightstage中K的元素值
   void SetIntrinsicParameterAt(int row,int col,double val)
   {
	   m_pCameraParameter->Lm_K(row,col)=val;
	   //m_pCameraParameter->Lm_K(row,col)=val;
   }


	//设置外参矩阵R的元素值
   void SetCameraParameterRAt(int row,int col,double val)
   {
	   m_pCameraParameter->m_R(row,col)=val;
   }


   //设置外参矩阵T的元素值
   void SetCameraParameterTAt(int index,double val)
   {
	   m_pCameraParameter->m_T[index]=val;
   }

   ////图像坐标到相机坐标，二维到三维。（x,y,1)-->(Xcam,Ycam,f)
    void GetCameraCoordFromImageCoord(int u,int v,double dsp,Wml::Vector3d& c_pt3d)
	{
		m_pCameraParameter->GetCameraCoordFromImageCoord(u,v,dsp,c_pt3d,s_dScale<1.0);
	}

	//从图像坐标(x,y,1)转换到世界坐标(X,Y,Z)
	void GetWorldCoordFromImageCoord(double u, double v, double dsp, Wml::Vector3d& w_pt3d){
		m_pCameraParameter->GetWorldCoordFromImageCoord(u, v, dsp, w_pt3d, s_dScale < 1.0);
	}

	//
	void GetImageCoordFromWorldCoord(double& out_u, double& out_v, double& out_dsp, Wml::Vector3d& w_pt3d){
		m_pCameraParameter->GetImageCoordFromWorldCoord(out_u, out_v, out_dsp, w_pt3d, s_dScale < 1.0);
	}

	//从世界坐标获得该坐标深度值
	double GetDepthFromWorldCoord(Wml::Vector3d& w_pt3d)
	{
		Wml::Vector3d c_pt3d;
		m_pCameraParameter->GetCameraCoordFromWorldCoord(c_pt3d, w_pt3d);  //需要看所求是否正确
		return c_pt3d[2];
	}


   //初始化图像的width和height
   static void InitImageSzie(int width,int height)
   {
	  s_iWidth=width;
	  s_iHeight=height;
   }

   //得到图像宽度
   static int GetImageWidth()
   {
	   if(s_dScale<1.0)
		   return s_iWidth*s_dScale+0.5F;   //按照m_dScale比例缩小图像宽度（+0.5的作用是四舍五入)

	   return s_iWidth;

   }

//得到图像（正常）宽度
   static int GetImgWidthNormal()
   {
	   return s_iWidth;
   }

//得到图像（缩放后）宽度
   static int GetImgWidthSmall()
   {
	   return s_iWidth * s_dScale  + 0.5F;
   }


   //得到图像高度
   static int GetImageHeight()
   {
	   if(s_dScale<1.0)
		   return s_iHeight*s_dScale+0.5F; //按照m_dScale比例缩小图像高度（+0.5的作用为四舍五入）

	   return s_iHeight;
   }

   //得到图像（正常）高度
   static int GetImgHeightNormal()
   {
	   return s_iHeight;
   }

   //得到图像（缩放后）高度
   static int GetImgHeightSmall()
   {
	   return s_iHeight * s_dScale  + 0.5F;
   }

   //加载源（彩色）图像(重载）
   void LoadColorImage();

   //加载源（彩色）图像
   void LoadColorImage(ZByteImage * &colorImage);

   //创建深度图像
   void CreateDspImage();

   //获得(MeanShift)分割（结果）图像存储路径
   std::string GetSegmSavePath();

   //存储Label图像
   void SaveLabelImage(ZIntImage &labelImage,int labelCount,std::string runType);

   //应用BO步骤求得的disparity图像初始化DE步骤的Label图像
   void InitLabelImgByDspImg(ZIntImage& labelImg, int LevelCount, double dDspMin, double dDspMax);


public:
   ZByteImage *m_pColorImage;       //彩色图
   ZFloatImage * m_pDspImage;       //disparity图像



private:
	CameraParameters * m_pCameraParameter;    //相机参数指针

	/***注： 静态成员变量必须在cpp中初始化，否则会报错***/
	static int s_iWidth;           //用于初始化图像width和height
	static int s_iHeight;
	static double s_dScale;        //标志图像是否缩放


	static std::string s_tempDir;      //设置存储目录
	static std::string s_dataDir;
	static std::string s_maskDir;
	//static std::string s_removalDir;



/************************* Lightstage*************************************/

//Visuallhull

	bool m_FaceVis[6];//这里所有Voxel的Face的法向量只可能是六个，而这个数组测试了这些法向量对Viewi是否可见
	//六个表面顺序是垂直于x轴的近远两个平面，垂直于y轴的近远两个平面，垂直于z轴的近远两个平面

public:
	void SetFaceVis(Wml::Vector3d* faceNormal);


private:
	int m_iMaskMinX;
	int m_iMaskMinY;
	int m_iMaskMaxX;
	int m_iMaskMaxY;



	unsigned char  LEASTFRONT;

	float m_fMinDsp;
	float m_fMaxDsp;

public:
	ZByteImage *m_pMaskImage;       //彩色图
	std::string m_sMaskImgPathName;  //mask图像存储路径（包括图像名称）

public:
	 bool LoadMaskImg(int corrode = 0);   

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


	 void GetKXR(Wml::Matrix3d& KXR){
		 m_pCameraParameter->GetKXR(KXR);
		// m_pCameraParameter->
	 }

	 Wml::Vector3d GetCameraPos(){
		 return m_pCameraParameter->GetCameraPos();
	 }
	 void GetCameraCoordFromWorldCoord(Wml::Vector3d& c_pt3d, Wml::Vector3d& w_pt3d){
		 m_pCameraParameter->GetCameraCoordFromWorldCoord(c_pt3d, w_pt3d);
	 }

	 void SetDspRange(float minDsp, float maxDsp){
		 m_fMinDsp = minDsp;
		 m_fMaxDsp = maxDsp;
	 }
	 void GetDspRange(float& minDsp, float& maxDsp){
		 minDsp = m_fMinDsp;
		 maxDsp = m_fMaxDsp;
	 }


};

