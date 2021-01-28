#pragma once
#include "Block.h"
#include <iostream>

class ImgPartitionBase
{
public:
	//���캯��
	ImgPartitionBase(void);
	ImgPartitionBase(int BlockCountX,int BlockCountY,double dOverlap);
	
   //��������
	~ImgPartitionBase(void);
     
	//����ֵ����ʼ��
	virtual void Init(int BlockCountX,int BlockCountY,double dOverlap);

	//�ֿ����
	virtual void GetBlockStrategy(double oriImgWidth,double oriImgHeight,int dspLayer,double overlap,int &BlockCountX,int &BlockCountY)=0;

    virtual Block GetBlockAt(int BlockIdX,int BlockIdY,int iImgWidht,int iImgHeight)=0;




	//�������(x,y)��Ӧ�Ŀ�ţ�ˮƽ�ʹ�ֱ����||blockIdX,blockIdYΪ���
	void GetBlockId(int x,int y,int iImgWidth,int iImgHeight,int &blockIdX,int &blockIdY);


	void GetMaxBlockSize(int iImgWidth,int iImgHeight,int &width,int &height);

    void SetBlocksCount(int xCount,int yCount)
	{
		m_iBlockCountX=xCount;
		m_iBlockCountY=yCount;
	}

	int GetBlockCountX()
	{
		return m_iBlockCountX;
	}

	int GetBlockCountY()
	{
		return m_iBlockCountY;
	}
	

	//�����ӡ��ִ�д˺���������Ϣ
	virtual void print();


protected:
	  double m_dOverlap;   //���ڿ�֮���ص���С
	  int m_iBlockCountX;  // ˮƽ����ֿ����
	  int m_iBlockCountY;  // ��ֱ����ֿ����


};

