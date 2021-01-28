#ifndef SelectPoints
#define SelectPoints
#pragma once
#include<stdio.h>
#include <iostream>
#include <vector>
#include <WmlVector3.h>
#include <algorithm>
#include "LSVideoFrame.h"
#include "LSStructureMotion.h"
#include "SelectPointParam.h"
#include "PointSetNormal.h"

using namespace std;

class LSStructureMotion;
class LSVideoFrame;


typedef struct  _ThreeDPoint  
{
	Wml::Vector3f Vertex3D;
	Wml::Vector3f  Normal;
	//RGBQUAD MainFrameRGB;
	int MainFrameIdx;
	float Conf;
	float nccConf;
	int x,y;
	bool isValid;
}ThreeDPoint;

const double CosAngleFilter = 0.4;

class SelectPoint
{

public:
	 typedef  bool ( *calNormalFn)(std::vector<Wml::Vector3f>& ,Wml::Vector3f& );
     SelectPoint(LSStructureMotion* pMotion,int startFrame,int endFrame);
	~SelectPoint(void);

	void SetParam(SelectPointParam& param);
	void doStart(std::string& fileName);
	void Start(std::vector<Wml::Vector3f>& SelectedVerts,std::vector<Wml::Vector3f>& SelectedNoraml);


	//LY Add Function
	//get world coord from image coord
	Wml::Vector3d Reproject(LSVideoFrame* pVFrm, Wml::Vector2d p, double z);

	//�Զ���ת�����������ܻ�Ӱ�캯��ִ��Ч��
	Wml::Vector3d CvtV3fToV3d(Wml::Vector3f& v3fNum);
	Wml::Vector3f CvtV3dToV3f(Wml::Vector3d& v3dNum);

private:

	//Select_Conf
	void LoadDepthAndMask();

	//��һ֡�ϵ�����ͶӰ������֡�ϼ����������ȵĿ��Ŷ�
	void EstConffidence(std::vector<float* >& ConfMapList);  //Select_Conf

	//Select_Conf
	void PrepareImg(LSStructureMotion*  pMOtion,
		std::vector<int >&  frameNoSet,
		std::vector<ZByteImage* >&  loadImg);





	//Select_Priority
	void ProjSelect(std::vector<ThreeDPoint* >&SelectedVertex,std::vector<float>& CoarseDisp,
		std::vector<int* >& OccupyMapList,std::vector<float* >& ConfMapList);

	//Select_Priority
	void SelectCadidateV(std::vector<ThreeDPoint* >&CandidateV,float segstart,float segend,float* ConfMap,
		int CurCamNo,int* OccupyMap,float confStart,float confEnd);

	//Select_Priority
	void ProjOccupy(std::vector<ThreeDPoint* >&SelectedV,std::vector<ThreeDPoint* >&CandidateV,
		std::vector<int* >& OccupyMapList);

	//Select_Priority
	void ProjCoccupyIthFrame(ThreeDPoint* CurVertex,int* OccupyMap,LSVideoFrame* otherFrame);

	//Select_Priority
	//bool  ConfGreater(const ThreeDPoint* x,const ThreeDPoint* y);

	//Select_VisualHull
	bool VisualhullCheck(Wml::Vector3f& ThreeDV);

	//Select_VisualHull
	bool FitNormal(calNormalFn  fn,LSVideoFrame* CurVFrame,
		int x,int y,Wml::Vector3f& Noraml,Wml::Vector3d& OptiView);

	//Select_VisualHull
	void DebugDesity(std::vector<int*>& OccupyMapList);

	
	//Select_VisualHull
	void ReleaseDepthAndMask();

    //Select_VisualHull
	void WritePts(std::vector<Wml::Vector3f>& PointCloud,std::vector<Wml::Vector3f>& NormalCloud,std::string& FileName);


	SelectPointParam SPParam;
	LSStructureMotion*  mMotion;
	int mStartFrame;
	int mEndFrame;
    //����ÿ�������Ƿ��Ѿ���һ����ά�������Ӧ�����ﵱһ���㣨8�ھӣ���Χ�и�mDesity��ʱ
    //�Ѿ�����ά�д��ڶ�Ӧ��ʱ��,�����øĵ���Ч
};

#endif