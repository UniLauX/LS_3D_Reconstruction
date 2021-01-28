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

	
	//���캯��
	//DataCostWorkUnit(void);

	//���캯�������أ�
	DataCostWorkUnit(int lineID,const Block &block, const std::vector<double> &dspV,ZIntImage &labelImg,
		DataCost &dataCost, PixelCostComputorBase &pixelCostComputer,ZIntImage *pOffsetImg);


	//��������
	~DataCostWorkUnit(void);

   virtual void Execute() throw();

	void Excute_offsetImg() throw();



private:
	int  m_iLineIndex;
	const Block &m_block;  //�飨��������
	ZIntImage &m_labelImg; //��label(disparity)ֵ��ͼ��
	DataCost &m_dataCost;  //DataCost(��������
	const std::vector<double> &m_dspV;  //��disparityֵ����������������
	ZIntImage *m_pOffsetImg;    //�洢��ͼ��ƫ�ƣ�
	PixelCostComputorBase &m_pixelCostComputor; //��������DataCost�����ࣨ��������



};

