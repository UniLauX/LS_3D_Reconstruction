#pragma once
#include "Block.h"
#include <iostream>

class ImgPartitionBase
{
public:
	//构造函数
	ImgPartitionBase(void);
	ImgPartitionBase(int BlockCountX,int BlockCountY,double dOverlap);
	
   //析构函数
	~ImgPartitionBase(void);
     
	//（赋值）初始化
	virtual void Init(int BlockCountX,int BlockCountY,double dOverlap);

	//分块策略
	virtual void GetBlockStrategy(double oriImgWidth,double oriImgHeight,int dspLayer,double overlap,int &BlockCountX,int &BlockCountY)=0;

    virtual Block GetBlockAt(int BlockIdX,int BlockIdY,int iImgWidht,int iImgHeight)=0;




	//求坐标点(x,y)对应的块号（水平和垂直方向）||blockIdX,blockIdY为输出
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
	

	//输出打印（执行此函数）的信息
	virtual void print();


protected:
	  double m_dOverlap;   //相邻块之间重叠大小
	  int m_iBlockCountX;  // 水平方向分块个数
	  int m_iBlockCountY;  // 垂直方向分块个数


};

