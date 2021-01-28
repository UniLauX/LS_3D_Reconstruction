#include "ExWRegularGridBP.h"

#include <iostream>
//#include "ParallelManager.h"
#include "ExWRegularGridBPWorkUnit.h"

namespace NumericalAlgorithm{

#define  INF  1e20


	//float CRegularGridBP::m_DW = 0.07;
	//float CExWRegularGridBP::m_DiscK = 1.7;
	int CExWRegularGridBP::VALUES = 16;
	int CExWRegularGridBP::m_iMaxIter = 10;
	int CExWRegularGridBP::m_iThreadCount = 1;

	CExWRegularGridBP::CExWRegularGridBP(void)
	{
	}

	CExWRegularGridBP::~CExWRegularGridBP(void)
	{
	}

	void CExWRegularGridBP::Solve(ZCubeFloatImage& dataCost, ZIntImage& labelImg, ZIntImage& offsetImg, ZFloatImage& wImg, ZFloatImage& truncImg)
	{
		int iMaxLabel = dataCost.GetChannel();
		VALUES = iMaxLabel;

		ZCubeFloatImage uI,dI,rI,lI;
		int iWidth = dataCost.GetWidth();
		int iHeight = dataCost.GetHeight();

		uI.CreateAndInit(iWidth,iHeight,iMaxLabel);
		dI.CreateAndInit(iWidth,iHeight,iMaxLabel);
		rI.CreateAndInit(iWidth,iHeight,iMaxLabel);
		lI.CreateAndInit(iWidth,iHeight,iMaxLabel);


		Bp_CP(uI, dI, rI, lI, dataCost, offsetImg, wImg, truncImg);

		labelImg.Create(iWidth,iHeight);
		for (int y = 1; y < iHeight-1; y++) {
			for (int x = 1; x < iWidth-1; x++) {
				// keep track of best value for current pixel
				int best = 0;
				float best_val = INF;
				for (int value = 0; value < iMaxLabel; value++) {
					float val = 
						uI.at( x, y+1,value) +
						dI.at( x, y-1,value) +
						lI.at( x+1, y,value) +
						rI.at( x-1, y,value) +					
						dataCost.at(x,y,value);
					if (val < best_val) {
						best_val = val;
						best = value;
						labelImg.at(x,y) = best;					
					}
				}
			}
		}

		//Boundary
		for (int y = 0; y < iHeight; y++) {
			labelImg.at(0,y) = labelImg.at(1,y);
			labelImg.at(iWidth-1,y) = labelImg.at(iWidth-2,y);
		}
		for (int x = 0; x < iWidth; x++) {
			labelImg.at(x,0) = labelImg.at(x,1);
			labelImg.at(x,iHeight-1) = labelImg.at(x,iHeight-2);
		}
	}

	void CExWRegularGridBP::Bp_CP(ZCubeFloatImage& uI, ZCubeFloatImage& dI, ZCubeFloatImage& rI, ZCubeFloatImage& lI, 
		ZCubeFloatImage& dataCost, ZIntImage& offsetImg, ZFloatImage& wImg, ZFloatImage& truncImg)
	{
		int width = dataCost.GetWidth();
		int height = dataCost.GetHeight();

		int ITER = m_iMaxIter;


		if(m_iThreadCount <=1){
			for (int t = 0; t < ITER; t++) {
				std::cout << "iter " << t << "\n";

				for (int y = 1; y < height-1; y++) {
					for (int x = ((y+t) % 2) + 1; x < width-1; x+=2) {

						//message (x,y) -> (x,y-1)
						msg(&uI.at(x,y+1),&lI.at( x+1, y),&rI.at( x-1, y),
							&dataCost.at(x, y), &uI.at(x, y), offsetImg.at(x,y-1)-offsetImg.at(x,y), wImg.at(x,y,2), truncImg.at(x,y,2));

						//(x,y) -> (x,y+1)
						msg(&dI.at(x, y-1),&lI.at(x+1, y),&rI.at(x-1, y),
							&dataCost.at(x, y), &dI.at(x, y), offsetImg.at(x,y+1)-offsetImg.at(x,y), wImg.at(x,y,3), truncImg.at(x,y,3));

						//(x,y) -> (x+1,y)
						msg(&uI.at(x, y+1),&dI.at(x, y-1),&rI.at(x-1, y),
							&dataCost.at(x, y), &rI.at(x, y), offsetImg.at(x+1,y)-offsetImg.at(x,y), wImg.at(x,y,1), truncImg.at(x,y,1));

						//(x,y) -> (x-1,y)
						msg(&uI.at(x, y+1),&dI.at(x, y-1),&lI.at(x+1, y),
							&dataCost.at(x, y), &lI.at(x, y), offsetImg.at(x-1,y)-offsetImg.at(x,y), wImg.at(x,y,0), truncImg.at(x,y,0));

					}
				}
			}
		}
		else{
			std::cout << "Multi-thread BP:\n" <<std::endl;
			for (int t = 0; t < ITER; t++) {
				std::cout << "iter " << t << "\n";
				/*
				ParallelManager pm;
				pm.SetSize(m_iThreadCount);
				*/
				for (int y = 1; y < height-1; y++) {
					CExWRegularGridBPWorkUnit* pWorkUnit = new CExWRegularGridBPWorkUnit(y,width,t,uI,dI,rI,lI,dataCost,offsetImg,wImg,truncImg);

					pWorkUnit->Execute();
				//	pm.EnQueue(pWorkUnit);
				}

				
				//pm.Run();
			}
		}
	}

	void CExWRegularGridBP::Dt(float* f, float w)
	{
		for (int q = 1; q < VALUES; q++) {
			float prev = f[q-1] + w;
			if (prev < f[q])
				f[q] = prev;
		}
		for (int q = VALUES-2; q >= 0; q--) {
			float prev = f[q+1] + w;
			if (prev < f[q])
				f[q] = prev;
		}
	}

	void CExWRegularGridBP::msg(float* s1, float* s2, 
		float* s3, float* s4, float* dst, int offset, float w, float trunc) {
			float val;

			// offset: dstOffset - srcOffset
			// aggregate and find min
			float minimum = INF;

			for(int value = 0; value <VALUES; value++){
				dst[value] = INF;
			}
			for (int value = 0; value < VALUES; value++) {
				//message y->x: h(y) + |x-y+offset|
				//h(y), y = value, dstOffset + x = srcOffset + y
				float hVal = s1[value] + s2[value] + s3[value] + s4[value];			

				if (hVal < minimum)
					minimum = hVal;

				int xVal = min(VALUES-1, max(0,value - offset));
				hVal += w * abs(xVal - (value-offset));
				dst[xVal] = min(dst[xVal],hVal);

			}

			// dt
			Dt(dst,w);

			// truncate 
			minimum += w * trunc;
			for (int value = 0; value < VALUES; value++)
				if (minimum < dst[value])
					dst[value] = minimum;

			// normalize
			val = 0;
			for (int value = 0; value < VALUES; value++) 
				val += dst[value];

			val /= VALUES;
			for (int value = 0; value < VALUES; value++) 
				dst[value] -= val;

	}


}

