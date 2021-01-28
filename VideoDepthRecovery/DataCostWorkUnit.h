#pragma once
#include <vector>
#include "ZImage.h"
#include "depthparawidget.h"
#include "Block.h" 
#include "DataCost.h"
#include "PixelCostComputorBase.h"

class DataCostWorkUnit
{
public:

	
	//构造函数
	//DataCostWorkUnit(void);

	//构造函数（重载）
	DataCostWorkUnit(int lineID,const Block &block, const std::vector<double> &dspV,ZIntImage &labelImg,
		DataCost &dataCost, PixelCostComputorBase &pixelCostComputer,ZIntImage *pOffsetImg);


	//析构函数
	~DataCostWorkUnit(void);

   virtual void Execute() throw();

	void Excute_offsetImg() throw();



private:
	int  m_iLineIndex;
	const Block &m_block;  //块（对象）引用
	ZIntImage &m_labelImg; //存label(disparity)值的图像
	DataCost &m_dataCost;  //DataCost(对象）引用
	const std::vector<double> &m_dspV;  //存disparity值的向量（对象）引用
	ZIntImage *m_pOffsetImg;    //存储（图像偏移）
	PixelCostComputorBase &m_pixelCostComputor; //计算像素DataCost基础类（对象）引用



};

