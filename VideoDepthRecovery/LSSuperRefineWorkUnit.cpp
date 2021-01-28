#include "LSSuperRefineWorkUnit.h"
#include "LSDepthEstimator.h"


LSSuperRefineWorkUnit::~LSSuperRefineWorkUnit(void)
{
}

LSSuperRefineWorkUnit::LSSuperRefineWorkUnit(LSVideoFrame* pCurrentFrame, std::vector<LSVideoFrame*>& FwFrames, std::vector<LSVideoFrame*>& BwFrames, ZIntImage& labelImg, ZIntImage& offsetImg,
	DataCost& dataCost, int iLineIndex, int Width, int totalDspLevel, int SubSample, LSVideoFrame::LABEL_IMG_TYPE type) 
	: m_FwFrames(FwFrames), m_BwFrames(BwFrames), m_labelImg(labelImg), m_OffsetImg(offsetImg), m_dataCost(dataCost)
{
	m_pCurrentFrame = pCurrentFrame;
	m_iLineIndex = iLineIndex;

	m_iWidth = Width;

	m_iTotalDspLevelCount = totalDspLevel;

	m_iSubSample = SubSample;

	m_type = type;
}


void LSSuperRefineWorkUnit::Execute()
{
    LSDepthEstimator* funs = LSDepthEstimator::GetInstance();
	int v = m_iLineIndex;

	int subDsplevel = 2 * m_iSubSample + 1;
	int iLayerCount = m_iTotalDspLevelCount - 1;

	float fDspMin, fDspMax;
	m_pCurrentFrame->GetDspRange(fDspMin, fDspMax);

	//std::cout<<"fDspMin:"<<fDspMin<<",fDspMax:"<<fDspMax<<std::endl;
   // std::cout<<"iLayerCount:"<<iLayerCount<<std::endl;


	std::vector<float> subDspV(subDsplevel);

	int x, y;

	int minX, minY, maxX, maxY;
	m_pCurrentFrame->GetMaskRectangle(minX, minY, maxX, maxY);
	DataCostUnit dataCosti;
	for(int u =0; u < m_iWidth; u++){
		//float * dataCosti = m_dataCost + (v * m_iWidth + u) * subDsplevel;
		m_dataCost.GetDataCostUnit(u, v, dataCosti);

		funs->GetGlobalCoordinateFrmBlockCoordinate(u, v, x, y);				

		for(int k=0; k<subDsplevel; ++k){
			subDspV[k] = fDspMin * (iLayerCount - m_OffsetImg.at(x-minX, y-minY) - k)/(float)(iLayerCount) + 
				fDspMax * ( m_OffsetImg.at(x-minX, y-minY) + k )/(float)(iLayerCount);
		}

		int bestLabel;
		if(m_type == LSVideoFrame::INIT_DE)
			funs->GetInitialDataCostAt(m_FwFrames, m_BwFrames, m_pCurrentFrame, x, y, dataCosti, bestLabel, subDspV, false);
		else
			funs->GetBODataCostAt(m_FwFrames, m_BwFrames, m_pCurrentFrame, x, y, dataCosti, bestLabel, subDspV, true);
		m_labelImg.at(x, y) = bestLabel;
	}
	std::cout << m_iLineIndex << " ";

}