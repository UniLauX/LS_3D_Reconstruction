#include "LSStereoWorkUnit.h"
#include "LSDepthEstimator.h"

// Init the variables
LSStereoWorkUnit::LSStereoWorkUnit(LSVideoFrame* pCurrentFrame, std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, 
	ZIntImage& labelImg, DataCost& dataCost, std::vector<float>& dspV, int iLineIndex, int Width, CAL_TYPE callType): m_FwFrames(FwFrames), m_BwFrames(BwFrames), 
	m_labelImg(labelImg), m_dataCost(dataCost), m_dspV(dspV)
{
	m_pCurrentFrame = pCurrentFrame;
	m_iLineIndex = iLineIndex;
	m_iWidth = Width;
	m_callType = callType;
}


LSStereoWorkUnit::~LSStereoWorkUnit(void)
{
}

void LSStereoWorkUnit::Execute()
{
	int v = m_iLineIndex;   // ith row
	LSDepthEstimator* funs = LSDepthEstimator::GetInstance();

	int x, y;
	DataCostUnit dataCosti;
	for(int u =0; u < m_iWidth; u++){
		//float * dataCosti = m_dataCost + (v * m_iWidth + u) * funs->GetDspLevel(); // datacostUnit in (v,u)
	 
		m_dataCost.GetDataCostUnit(u, v, dataCosti);     //may have problem

		int bestLabel;
		
        // get image coord from block coord
		funs->GetGlobalCoordinateFrmBlockCoordinate(u, v, x, y);

		switch(m_callType)
		{
		case INIT : funs->GetInitialDataCostAt(m_FwFrames, m_BwFrames, m_pCurrentFrame, x, y, dataCosti, bestLabel, m_dspV, true); break;
		case BO : funs->GetBODataCostAt(m_FwFrames, m_BwFrames, m_pCurrentFrame, x, y, dataCosti, bestLabel, m_dspV, true); break;
		case OPT: funs->GetOptDataCostAt(m_FwFrames, m_BwFrames, m_pCurrentFrame, x, y, dataCosti, bestLabel, m_dspV, true); break;

		default: std::cout << "ERROR:";
		}
	
		if(funs->IfInTrueRegion(u, v))
			m_labelImg.SetPixel(x, y, 0, bestLabel);
		
	}

	std::cout << m_iLineIndex << " ";

}
