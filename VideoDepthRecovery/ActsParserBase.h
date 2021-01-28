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
	      //虚函数必须在其继承类中显示实现，否则会报错
	virtual bool LoadProject(const std::string & fileName)=0;    //加载视频序列
    virtual bool SaveProject(const std::string & fileName,const std::string &imageName);
	virtual bool SaveProject(const std::string & fileName,const std::string &imageName, const std::vector<int> &index)=0;
	

	void clear();
    

public:
	//对应act/actb文件中<motion type>信息
	enum MotionType{FREE_MOVE,ROTATION_ONLY};         //运动类型{自由运动，仅旋转}
	enum FocalType{FOCAL_KNOWN,FOCAL_CONSTANT,FOCAL_VARIABLE};     //焦距类型{焦距已知，焦距不变，焦距变化}
	enum AspectType{ASPECT_KNOWN,ASPECT_CONSTANT,ASPECT_VARIABLE};  // 
	enum PrincipalType{PRINCIPAL_KNOWN,PRINCIPAL_CONSTANT,PRINCIPAL_VARIABLE};  //主点{主点已知，主点不变，主点变化}
	enum SkewType{SKEW_KNOWN,SKEW_CONSTANT,SKEW_VARIABLE};     //放缩类型{放缩已知，放缩不变，放缩变化}

	//对应act/actb文件中<Image sequence>信息
    std::string m_sImagePath;
	int m_iStartFrame;
	int m_iFrameStep;
	int m_iEndFrame;

	int m_iFrameCount;        //帧数
	
	//对应act/acb文件中<Motion type>信息
	MotionType m_motionType;         
	FocalType  m_focalType;
	AspectType m_aspectType;
	PrincipalType m_principalType;
	SkewType  m_skewType;


	//对应act/actb文件中<intrinsic parameter>
    double m_dFx,m_dFy;       //  焦距   = Alpha_x,Alpha_y ?
	double m_dCx,m_dCy;       //主点
	double m_dSkew;           //放缩（scaling)
	double m_dAspectRatio;    //方向比（aspect ratio）=Alpha_y/Alpha_x


	//对应cam文件中<intrinsic parameter>
    std::vector<Wml::Matrix3d>m_vdIntrinsicParameters; //intrinsic parameters for camFile
    
    std::vector<Wml::Matrix4d> m_vdExtrinsicParameters;    //存储所有帧外参矩阵
    std::vector<std::shared_ptr<std::vector<MatchPoint>>> m_vpFramesMatchPoints;  //所有帧上所有的匹配点
    std::vector<std::shared_ptr<MatchLinker>> m_vpMatchLinkers;                   //?


	//for Lightstage
	Wml::Matrix3d m_dLsIntrinsicParameter;
	Wml::Matrix4d m_dLsExtrinsicParameter;
    

};

