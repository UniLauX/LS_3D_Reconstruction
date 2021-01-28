#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "WmlMatrix3.h"       //Z:\Math\Basic\WmlMath\include
#include "VDRVideoFrame.h"
#include "ActsParserBase.h"
#include "FileNameParser.h"
#include "ActsFileParser.h"
#include "ActbFileParser.h"
#include "CamFileParser.h"

#include ".\libs\CxImage\lib\cximagelink.h"  //库文件
#include "cximage.h"
#include "CxImageFunction.h"
#include "depthparawidget.h"



class VDRStructureMotion
{
public:
	VDRStructureMotion(void);             //构造函数
	~VDRStructureMotion(void);            //虚构函数

//自定义函数列表
public:
	static VDRStructureMotion * GetInstance()       
	{
		static VDRStructureMotion instance;
		return &instance;
	}
    bool LoadFrames(const std::string &actsFilePath); //声明：加载视频序列
	
	


	//获得索引为index的Frame
	VDRVideoFrame *GetFrameAt(int index)
	{
		return mFrames.at(index).get();
	}

	//得到载入的视频序列帧数
	int GetFrameCount()
	{
		return m_iFrameCount;
	}


	void SetScaleVal(double dScale);

	//得到最大Disparity
	double GetMaxDsp(){
		if(m_dAutoMaxDsp > 0)
			return m_dAutoMaxDsp;
		return m_dAutoMaxDsp = EstimateDisparityRange();
	}


private:
	void Reset();         //声明： 重置所有参数
	void CreateFrames(int iCount);  // 创建iCount帧图像


	//估计Dispartiy的取值范围（等）
	double EstimateDisparityRange();

protected:
	int m_iFrameCount;     //第几帧
	std::vector<std::shared_ptr<VDRVideoFrame>>mFrames;               //shared_ptr 为智能指针
    
	double m_dAutoMaxDsp;   //是否自动设置最大disparity
	
	Wml::Matrix3d m_K;     //内参矩阵
	Wml::Matrix3d m_ScaledK;   //Scaled Intrincsic Matrix



public:
	//track对象容器（指针）
	std::vector<std::shared_ptr<MatchLinker>> m_vpMatchLinkers;

	ActsParserBase *m_pActsParser;     //分解act文件(或cam文件）的类型指针

//for Ls
	int camNum;
	ActsParserBase *m_pCamListParser; 




/*********************************************************************************/
                            //for 3D recovery//
	                        //lightstage  parameters//
/*********************************************************************************/

public:

	 bool LoadLightstageFileFolders(const std::string &cameraListPath);
	 bool LoadLightstageFrames(const std::string &actsFilePath);   //加载视频序列，针对lightstage中的cam文件
     
	 bool CalculateBox(int startFrame, int endFrame);  //计算boundingBox

	 void CalculateVisualHull(int startFrame, int endFrame);
	 void LS_RunInit(int startFrame,int endFrame);
	 void LS_RunRefine(int startFrame,int endFrame);
	 void LS_RunMesh(int startFrame,int endFrame);


private:
	//CVisualHull m_Visualhull;



public:
    std::vector<std::string> m_sWorkDirArr;
	std::string m_sWorkDir;
	std::string m_sPossionPath;
	std::string m_sMeshDir;

	
	Wml::Vector3d m_BoxMinXYZ;
	Wml::Vector3d m_BoxMaxXYZ;

/**********************************************************************************/
	                       //           //
/**********************************************************************************/



};

	
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           