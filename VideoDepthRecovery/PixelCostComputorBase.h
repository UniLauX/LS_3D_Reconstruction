#pragma once
#include <vector>
#include "VDRVideoFrame.h"
#include "DataCostUnit.h"
class PixelCostComputorBase
{
public:
	//声明：  构造函数
	PixelCostComputorBase(void);
	
	//声明：  析构函数
	virtual ~PixelCostComputorBase(void);

	//声明：   设置当前帧及相邻（计算）帧
	virtual void SetFrames(VDRVideoFrame *currentFrame,std::vector<VDRVideoFrame*> * nearFrames);
	
	//声明：   设置颜色和disparity的Sigma值（sigma_c和sigma_d)
	void SetSigma(double dColorSigma,double dDspSigma);

	//计算（x,y)坐标各DepthLevel层上的最大DataCost（disparity likelihood值）和bestlabel  
	virtual void PixelDataCost(int x,int y,const std::vector<double> &dspV, DataCostUnit &dataCostI, int &bestLabel)=0;


	//得到和设置sigma_d
	double GetDspSigma()
	{
		return m_dDspSigma;
	}
	void SetDspSigma(double sigma)
	{
		m_dDspSigma = sigma;
	}

	//得到和设置sigma_c
	double GetColorSigma()
	{
		return m_dColorSigma;
	}
	void SetColorSigma(double simga)
	{
		m_dColorSigma = simga;
	}

	//得到和设置DataCostWeight
	double GetDataCostWeight(){
		return m_fDataCostWeight;
	}
	void SetDataCostWeight(double weight)
	{
		m_fDataCostWeight = weight;
	}

protected:
	std::vector<VDRVideoFrame*> *m_pNearFrames;    //与当前帧临近的参考帧指针
	VDRVideoFrame  *m_pCurrentFrame;       //当前帧指针

	double m_dDspSigma;     //Sigma_d
	double m_dColorSigma;   //Sigma_c
	double m_dColorMissPenalty;    //有一部分点投射到图像外部（的惩罚值）

	float  m_fDataCostWeight;     // 能量函数中DataCost的权重，为固定值

};

