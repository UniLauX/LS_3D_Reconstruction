#pragma once
#include "DataCostUnit.h"
#include "ZImage.h"
class DataCost
{
public:
	//������ ���캯��
	DataCost(void);
	DataCost(int width,int height,int dspLevel,bool setZero=false);

    //������  ��������
	~DataCost(void);

	//������ ��������Levelͼ��Ԫ�ش洢�ռ䣨float����)���������ֵΪ0
	 void Create(int width,int height,int dspLevel,bool setZero=false );


	//������  �õ�level_i�ϣ�x,y)����λ��Ԫ�أ�ֵ)
	 float GetValueAt(int x,int y,int level_i);

    //������   At(x,y,level_i)�õ�level_i�ϣ�x,y)����λ��Ԫ�أ�ֵ)
	 float &At(int x,int y,int level_i);


	 //������  ��ã�x,y)�����ϵ�DataCostUnitֵ
	void  GetDataCostUnit(int x,int y,DataCostUnit &dataCostUnit);

	//�õ�dataCost����ָ�룩��ַ
	float *GetBits()
	{
		return m_pfData;
	}

	//�õ�ͼ����
	int GetWidth()
	{
		return m_iWidth;
	}

	
	//�õ�disparity�Ĳ���
	int GetDspLevels()
	{
		return m_iDspLevels;
	}

	//��labelImg������ÿ���������Сdisparity��Ӧ��level
	void SetLabelImg(int offsetX,int offsetY,int blockWidth,int blockHeight,ZIntImage &labelImg);




private:

	int m_iWidth;       //ͼ����
	int m_iHeight;      //ͼ��߶�
	int m_iDspLevels;   //disparity�Ĳ�����Level)

	float * m_pfData;    //DataCostUnit����ָ��
	int m_iSlice;        //һ���е�Ԫ�ظ��� ��m_iSlice=m_iWidth*m_iHeight��
	int m_iTotal;        //���в��е�Ԫ��������m_iTotal=m_iSlice*m_iDspLevels)

};

