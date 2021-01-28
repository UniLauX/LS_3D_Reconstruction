#pragma once
#include "PixelCostComputorBase.h"
#include <Windows.h>
class PixelCostComputer_Init:public PixelCostComputorBase
{
private:
	
	PixelCostComputer_Init(void);

public:
	static PixelCostComputer_Init *GetInstance()
	{
		static PixelCostComputer_Init instance;
		return &instance;
	}
	~PixelCostComputer_Init(void);

    //覆盖原函数
   //计算（x,y)坐标各DepthLevel层上的最大DataCost（disparity likelihood值）和bestlabel
   virtual	void PixelDataCost( int x, int y, const std::vector<double> &dspV, DataCostUnit &dataCosti, int &bestLabel )override;

};

