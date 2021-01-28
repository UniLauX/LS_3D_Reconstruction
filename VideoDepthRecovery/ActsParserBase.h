#pragma once
#include <iostream>
#include<vector>
#include "WmlMatrix4.h"
#include "WmlMatrix3.h"
#include "MatchLinker.h"
class ActsParserBase
{
public:
	ActsParserBase(void);
	~ActsParserBase(void);

public:
	      //�麯����������̳�������ʾʵ�֣�����ᱨ��
	virtual bool LoadProject(const std::string & fileName)=0;    //������Ƶ����
    virtual bool SaveProject(const std::string & fileName,const std::string &imageName);
	virtual bool SaveProject(const std::string & fileName,const std::string &imageName, const std::vector<int> &index)=0;
	

	void clear();
    

public:
	//��Ӧact/actb�ļ���<motion type>��Ϣ
	enum MotionType{FREE_MOVE,ROTATION_ONLY};         //�˶�����{�����˶�������ת}
	enum FocalType{FOCAL_KNOWN,FOCAL_CONSTANT,FOCAL_VARIABLE};     //��������{������֪�����಻�䣬����仯}
	enum AspectType{ASPECT_KNOWN,ASPECT_CONSTANT,ASPECT_VARIABLE};  // 
	enum PrincipalType{PRINCIPAL_KNOWN,PRINCIPAL_CONSTANT,PRINCIPAL_VARIABLE};  //����{������֪�����㲻�䣬����仯}
	enum SkewType{SKEW_KNOWN,SKEW_CONSTANT,SKEW_VARIABLE};     //��������{������֪���������䣬�����仯}

	//��Ӧact/actb�ļ���<Image sequence>��Ϣ
    std::string m_sImagePath;
	int m_iStartFrame;
	int m_iFrameStep;
	int m_iEndFrame;

	int m_iFrameCount;        //֡��
	
	//��Ӧact/acb�ļ���<Motion type>��Ϣ
	MotionType m_motionType;         
	FocalType  m_focalType;
	AspectType m_aspectType;
	PrincipalType m_principalType;
	SkewType  m_skewType;


	//��Ӧact/actb�ļ���<intrinsic parameter>
    double m_dFx,m_dFy;       //  ����   = Alpha_x,Alpha_y ?
	double m_dCx,m_dCy;       //����
	double m_dSkew;           //������scaling)
	double m_dAspectRatio;    //����ȣ�aspect ratio��=Alpha_y/Alpha_x


	//��Ӧcam�ļ���<intrinsic parameter>
    std::vector<Wml::Matrix3d>m_vdIntrinsicParameters; //intrinsic parameters for camFile
    
    std::vector<Wml::Matrix4d> m_vdExtrinsicParameters;    //�洢����֡��ξ���
    std::vector<std::shared_ptr<std::vector<MatchPoint>>> m_vpFramesMatchPoints;  //����֡�����е�ƥ���
    std::vector<std::shared_ptr<MatchLinker>> m_vpMatchLinkers;                   //?


	//for Lightstage
	Wml::Matrix3d m_dLsIntrinsicParameter;
	Wml::Matrix4d m_dLsExtrinsicParameter;
    

};

