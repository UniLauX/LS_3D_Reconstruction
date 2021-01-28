#pragma once    //��֤ͷ�ļ�ֻ������һ��

#include <array>
#include <vector>
#include "WmlVector3.h"

//���ٵ�
class TrackPoint
{
public:
	int m_iFrameNo;   //���ٵ�����֡��
	int m_iIndex;     
    double m_dX;     //���ٵ�Ķ�ά����
	double m_dY;
};

//
class MatchLinker     //����ĳ�����ٵ�
{
public:
	TrackPoint & Point(size_t index)   //size_t = unsign int
	{
		 if(index<m_ptList.size())
			 return m_ptList[index];
		 throw std::exception("ERROR:TrackPoint:: index out of range.");
	}
	//Ϊ��������洢�ռ�
	void Reserve(int iCount)
	{
		m_ptList.resize(iCount);
	}

	//���ظ��ٵ����
	size_t GetCount()
	{
		return m_ptList.size();
	}

public:     

	/***���ٵ���Ϣ***/
	int m_iFlag;       //��ʶ������Ӧÿ�����ٵ㣬0��ʾ�����ά�㣬��ά������洢��m_v3D�У�1��ʾδ��ø��ٵ㣬��������Ϊ�̶�ֵ<0.0000000	0.0000000	17.3869146>)
	int m_index;       
	int m_iErrorLevel; //��ʾ�������ά���Ƿ���ȷ
	Wml::Vector3d m_v3D; //�洢�������ά�����꣨��δ��ã���Ϊ�̶�ֵ<0.0000000	0.0000000	17.3869146>��

protected:
	std::vector<TrackPoint>m_ptList;  //��Ӧĳ�����ٵ㣬�洢���ٵ��������

};

//����֡�ϵ�����ƥ��
class MatchPoint
{
public:
	double m_dX;
	double m_dY;
	std::shared_ptr<MatchLinker>m_pMatchLinker;    //�����ض����ٵ㣬ָ������ٵ�����
};

