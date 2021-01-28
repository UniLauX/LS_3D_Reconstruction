#pragma once    //保证头文件只被编译一次

#include <array>
#include <vector>
#include "WmlVector3.h"

//跟踪点
class TrackPoint
{
public:
	int m_iFrameNo;   //跟踪点所属帧号
	int m_iIndex;     
    double m_dX;     //跟踪点的二维坐标
	double m_dY;
};

//
class MatchLinker     //对于某个跟踪点
{
public:
	TrackPoint & Point(size_t index)   //size_t = unsign int
	{
		 if(index<m_ptList.size())
			 return m_ptList[index];
		 throw std::exception("ERROR:TrackPoint:: index out of range.");
	}
	//为向量分配存储空间
	void Reserve(int iCount)
	{
		m_ptList.resize(iCount);
	}

	//返回跟踪点个数
	size_t GetCount()
	{
		return m_ptList.size();
	}

public:     

	/***跟踪点信息***/
	int m_iFlag;       //标识符，对应每个跟踪点，0表示解得三维点，三维点坐标存储在m_v3D中；1表示未解得跟踪点，三个坐标为固定值<0.0000000	0.0000000	17.3869146>)
	int m_index;       
	int m_iErrorLevel; //表示所解的三维点是否正确
	Wml::Vector3d m_v3D; //存储所解得三维点坐标（若未解得，则为固定值<0.0000000	0.0000000	17.3869146>）

protected:
	std::vector<TrackPoint>m_ptList;  //对应某个跟踪点，存储跟踪到点的坐标

};

//所有帧上单个点匹配
class MatchPoint
{
public:
	double m_dX;
	double m_dY;
	std::shared_ptr<MatchLinker>m_pMatchLinker;    //对于特定跟踪点，指向其跟踪点序列
};

