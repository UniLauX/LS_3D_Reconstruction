#pragma  once

#include "LSVideoFrame.h"
#include "LSStructureMotion.h"
#include <vector>

#ifndef _BoxCalculator
#define _BoxCalculator 

//#include <imgproc/imgproc.hpp>

class CBoxCalculator
{
public:
	CBoxCalculator();

	~CBoxCalculator();

	bool CalculateBox(LSStructureMotion* pMotion, int startFrame, int endFrame);

	//for seedball based method
	bool CalculateEnlargeBox(LSStructureMotion* pMotion, int startFrame, int endFrame);


	//bool CropImages(CBDMStructureMotion* pMotion, int startFrame, int endFrame);

	static CBoxCalculator* GetInstance(){
		return &m_Instance;
	}

private:
	//用单纯形法求解线性规划问题，计算出BoundingBox(主要是计算最小值minXYZ和最大值maxXYZ)。（目前可看做是黑盒 by Ly,3/18/2015）
	bool Calculate(std::vector<Wml::Vector3d> & minXYZ, std::vector<Wml::Vector3d>& maxXYZ);

	//for seedball method: enlarge boundingbox
	bool CalculateEnlarge(std::vector<Wml::Vector3d> & minXYZ, std::vector<Wml::Vector3d>& maxXYZ);


	bool InitMaskRectangle();

	bool InitMaskRectWithGMM();  //new add 5/19/2015 by ly

	bool InitEnlargeMaskRectangle();

	void InitDspRange(std::vector<double>& minDsp, std::vector<double>& maxDsp, std::vector<Wml::Vector3d>& minXYZ, std::vector<Wml::Vector3d>& maxXYZ);

	void prepareMat(std::vector<Wml::Vector4d >& planeSet,
									double*  coffMatA,double* B,int& M1);
private:
	int m_iObjectThreshold;     //the object mask threshold( between 0~255, here assign 5）
	int m_iColorApron;         //for the detect boundry, enrich m_iColorApron(here equal 30 pixel) pixel.
	double m_dSpaceApronSigma;  //used to set the init minDsp and maxDsp ( may need adjust)

	int m_iStartFrame;  // 0
	int m_iEndFrame;    // 7
	int m_iFrameNum;  // number of frames for every camera
	LSStructureMotion* m_pMotion;
	
    int m_iCamNum; //camera number in a circle

private:
	static CBoxCalculator m_Instance;
};

#endif  _BoxCalculator