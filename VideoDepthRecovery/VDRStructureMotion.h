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

#include ".\libs\CxImage\lib\cximagelink.h"  //���ļ�
#include "cximage.h"
#include "CxImageFunction.h"
#include "depthparawidget.h"



class VDRStructureMotion
{
public:
	VDRStructureMotion(void);             //���캯��
	~VDRStructureMotion(void);            //�鹹����

//�Զ��庯���б�
public:
	static VDRStructureMotion * GetInstance()       
	{
		static VDRStructureMotion instance;
		return &instance;
	}
    bool LoadFrames(const std::string &actsFilePath); //������������Ƶ����
	
	


	//�������Ϊindex��Frame
	VDRVideoFrame *GetFrameAt(int index)
	{
		return mFrames.at(index).get();
	}

	//�õ��������Ƶ����֡��
	int GetFrameCount()
	{
		return m_iFrameCount;
	}


	void SetScaleVal(double dScale);

	//�õ����Disparity
	double GetMaxDsp(){
		if(m_dAutoMaxDsp > 0)
			return m_dAutoMaxDsp;
		return m_dAutoMaxDsp = EstimateDisparityRange();
	}


private:
	void Reset();         //������ �������в���
	void CreateFrames(int iCount);  // ����iCount֡ͼ��


	//����Dispartiy��ȡֵ��Χ���ȣ�
	double EstimateDisparityRange();

protected:
	int m_iFrameCount;     //�ڼ�֡
	std::vector<std::shared_ptr<VDRVideoFrame>>mFrames;               //shared_ptr Ϊ����ָ��
    
	double m_dAutoMaxDsp;   //�Ƿ��Զ��������disparity
	
	Wml::Matrix3d m_K;     //�ڲξ���
	Wml::Matrix3d m_ScaledK;   //Scaled Intrincsic Matrix



public:
	//track����������ָ�룩
	std::vector<std::shared_ptr<MatchLinker>> m_vpMatchLinkers;

	ActsParserBase *m_pActsParser;     //�ֽ�act�ļ�(��cam�ļ���������ָ��

//for Ls
	int camNum;
	ActsParserBase *m_pCamListParser; 




/*********************************************************************************/
                            //for 3D recovery//
	                        //lightstage  parameters//
/*********************************************************************************/

public:

	 bool LoadLightstageFileFolders(const std::string &cameraListPath);
	 bool LoadLightstageFrames(const std::string &actsFilePath);   //������Ƶ���У����lightstage�е�cam�ļ�
     
	 bool CalculateBox(int startFrame, int endFrame);  //����boundingBox

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

	
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           