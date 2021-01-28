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
	/*  ��ȡ��̬ʵ���������ⲿ����
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

 //�õ���x,y)λ�õ�diaprityֵ
 float GetDspAt(int x, int y);
 double GetDspAt(double x, double y);


//���ã�x,y)λ�õ�disparity
void SetDspAt(int x, int y, float DspValue);


//��գ�ͼ��ռ�ã��ڴ�
 void Clear();

 //����ͼ��disparity
 void SetDspImg(ZIntImage& labelImg, const Block &block, double dspMin, double dspMax, int dspLevelCount);

 void SaveDspImg();

 //�������ͼ
 bool LoadDepthImg();
 bool LoadDepthImg(ZFloatImage *&dspImg);

 bool LoadDepthImg_small(ZFloatImage* &p);
 bool LoadDepthImg_normal(ZFloatImage* &p);




private:
    //�洢disparityͼ��
	void SaveDspImg(ZFloatImage* p, const std::string &path);
	
	//��ȡdisparityͼ��Ĵ洢·��
	std::string GetDspImgPath_samll();
	std::string GetDspImgPath_normal();




public:
   int m_iID;   //֡��
   std::string m_sImagePathName;   //ÿ֡ͼ��·�����ƣ�����ͼ�����ƣ�
   std::string m_sName;
   
	
   //track(���ڸ��٣�
   std::shared_ptr<std::vector<MatchPoint>> m_pvMatchPoints;//�洢ĳһ֡�����и��ٵ�

   //����lightstage��K��Ԫ��ֵ
   void SetIntrinsicParameterAt(int row,int col,double val)
   {
	   m_pCameraParameter->Lm_K(row,col)=val;
	   //m_pCameraParameter->Lm_K(row,col)=val;
   }


	//������ξ���R��Ԫ��ֵ
   void SetCameraParameterRAt(int row,int col,double val)
   {
	   m_pCameraParameter->m_R(row,col)=val;
   }


   //������ξ���T��Ԫ��ֵ
   void SetCameraParameterTAt(int index,double val)
   {
	   m_pCameraParameter->m_T[index]=val;
   }

   ////ͼ�����굽������꣬��ά����ά����x,y,1)-->(Xcam,Ycam,f)
    void GetCameraCoordFromImageCoord(int u,int v,double dsp,Wml::Vector3d& c_pt3d)
	{
		m_pCameraParameter->GetCameraCoordFromImageCoord(u,v,dsp,c_pt3d,s_dScale<1.0);
	}

	//��ͼ������(x,y,1)ת������������(X,Y,Z)
	void GetWorldCoordFromImageCoord(double u, double v, double dsp, Wml::Vector3d& w_pt3d){
		m_pCameraParameter->GetWorldCoordFromImageCoord(u, v, dsp, w_pt3d, s_dScale < 1.0);
	}

	//
	void GetImageCoordFromWorldCoord(double& out_u, double& out_v, double& out_dsp, Wml::Vector3d& w_pt3d){
		m_pCameraParameter->GetImageCoordFromWorldCoord(out_u, out_v, out_dsp, w_pt3d, s_dScale < 1.0);
	}

	//�����������ø��������ֵ
	double GetDepthFromWorldCoord(Wml::Vector3d& w_pt3d)
	{
		Wml::Vector3d c_pt3d;
		m_pCameraParameter->GetCameraCoordFromWorldCoord(c_pt3d, w_pt3d);  //��Ҫ�������Ƿ���ȷ
		return c_pt3d[2];
	}


   //��ʼ��ͼ���width��height
   static void InitImageSzie(int width,int height)
   {
	  s_iWidth=width;
	  s_iHeight=height;
   }

   //�õ�ͼ����
   static int GetImageWidth()
   {
	   if(s_dScale<1.0)
		   return s_iWidth*s_dScale+0.5F;   //����m_dScale������Сͼ���ȣ�+0.5����������������)

	   return s_iWidth;

   }

//�õ�ͼ�����������
   static int GetImgWidthNormal()
   {
	   return s_iWidth;
   }

//�õ�ͼ�����ź󣩿��
   static int GetImgWidthSmall()
   {
	   return s_iWidth * s_dScale  + 0.5F;
   }


   //�õ�ͼ��߶�
   static int GetImageHeight()
   {
	   if(s_dScale<1.0)
		   return s_iHeight*s_dScale+0.5F; //����m_dScale������Сͼ��߶ȣ�+0.5������Ϊ�������룩

	   return s_iHeight;
   }

   //�õ�ͼ���������߶�
   static int GetImgHeightNormal()
   {
	   return s_iHeight;
   }

   //�õ�ͼ�����ź󣩸߶�
   static int GetImgHeightSmall()
   {
	   return s_iHeight * s_dScale  + 0.5F;
   }

   //����Դ����ɫ��ͼ��(���أ�
   void LoadColorImage();

   //����Դ����ɫ��ͼ��
   void LoadColorImage(ZByteImage * &colorImage);

   //�������ͼ��
   void CreateDspImage();

   //���(MeanShift)�ָ�����ͼ��洢·��
   std::string GetSegmSavePath();

   //�洢Labelͼ��
   void SaveLabelImage(ZIntImage &labelImage,int labelCount,std::string runType);

   //Ӧ��BO������õ�disparityͼ���ʼ��DE�����Labelͼ��
   void InitLabelImgByDspImg(ZIntImage& labelImg, int LevelCount, double dDspMin, double dDspMax);


public:
   ZByteImage *m_pColorImage;       //��ɫͼ
   ZFloatImage * m_pDspImage;       //disparityͼ��



private:
	CameraParameters * m_pCameraParameter;    //�������ָ��

	/***ע�� ��̬��Ա����������cpp�г�ʼ��������ᱨ��***/
	static int s_iWidth;           //���ڳ�ʼ��ͼ��width��height
	static int s_iHeight;
	static double s_dScale;        //��־ͼ���Ƿ�����


	static std::string s_tempDir;      //���ô洢Ŀ¼
	static std::string s_dataDir;
	static std::string s_maskDir;
	//static std::string s_removalDir;



/************************* Lightstage*************************************/

//Visuallhull

	bool m_FaceVis[6];//��������Voxel��Face�ķ�����ֻ����������������������������Щ��������Viewi�Ƿ�ɼ�
	//��������˳���Ǵ�ֱ��x��Ľ�Զ����ƽ�棬��ֱ��y��Ľ�Զ����ƽ�棬��ֱ��z��Ľ�Զ����ƽ��

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
	ZByteImage *m_pMaskImage;       //��ɫͼ
	std::string m_sMaskImgPathName;  //maskͼ��洢·��������ͼ�����ƣ�

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

