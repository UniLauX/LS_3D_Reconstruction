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

    //����ԭ����
   //���㣨x,y)�����DepthLevel���ϵ����DataCost��disparity likelihoodֵ����bestlabel
   virtual	void PixelDataCost( int x, int y, const std::vector<double> &dspV, DataCostUnit &dataCosti, int &bestLabel )override;

};

