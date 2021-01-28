#pragma once

//#include "ParallelWorkUnit.h"
#include "ZCubeImage.h"
#include "ExWRegularGridBP.h"

namespace NumericalAlgorithm{

	class CExWRegularGridBPWorkUnit //: public ParallelWorkUnit
	{
	public:
		CExWRegularGridBPWorkUnit(int y1, int iWidth, int iIter, ZCubeFloatImage& uI1,ZCubeFloatImage& dI1,ZCubeFloatImage& rI1,ZCubeFloatImage& lI1, ZCubeFloatImage& dataCost1,
			ZIntImage& offsetImg1, ZFloatImage& wImg1, ZFloatImage& truncImg1)
			: y(y1), m_iWidth(iWidth), m_iIter(iIter), uI(uI1), dI(dI1), rI(rI1), lI(lI1), dataCost(dataCost1), offsetImg(offsetImg1), wImg(wImg1), truncImg(truncImg1)
		{

		}

		~CExWRegularGridBPWorkUnit(void);

	public:

		/*virtual*/ void Execute()
		{

			for (int x = ((y+m_iIter) % 2) + 1; x < m_iWidth-1; x+=2) {

				//message (x,y) -> (x,y-1)
				CExWRegularGridBP::msg(&uI.at(x,y+1),&lI.at( x+1, y),&rI.at( x-1, y),
					&dataCost.at(x, y), &uI.at(x, y), offsetImg.at(x,y-1)-offsetImg.at(x,y), wImg.at(x,y,2), truncImg.at(x,y,2));

				//(x,y) -> (x,y+1)
				CExWRegularGridBP::msg(&dI.at(x, y-1),&lI.at(x+1, y),&rI.at(x-1, y),
					&dataCost.at(x, y), &dI.at(x, y), offsetImg.at(x,y+1)-offsetImg.at(x,y), wImg.at(x,y,3), truncImg.at(x,y,3));

				//(x,y) -> (x+1,y)
				CExWRegularGridBP::msg(&uI.at(x, y+1),&dI.at(x, y-1),&rI.at(x-1, y),
					&dataCost.at(x, y), &rI.at(x, y), offsetImg.at(x+1,y)-offsetImg.at(x,y), wImg.at(x,y,1), truncImg.at(x,y,1));

				//(x,y) -> (x-1,y)
				CExWRegularGridBP::msg(&uI.at(x, y+1),&dI.at(x, y-1),&lI.at(x+1, y),
					&dataCost.at(x, y), &lI.at(x, y), offsetImg.at(x-1,y)-offsetImg.at(x,y), wImg.at(x,y,0), truncImg.at(x,y,0));

			}
		}

		/*virtual*/ int GetPriority()
		{
			return -y;
		}


	private:
		int y;
		int m_iWidth;	
		int m_iIter;
		ZCubeFloatImage& uI,&dI,&rI,&lI;
		ZCubeFloatImage& dataCost;
		ZIntImage& offsetImg;
		ZFloatImage& wImg, & truncImg;
	};

}
