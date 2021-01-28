#include "PixelCostComputorBase.h"


//���캯�����Գ�Ա�������г�ʼ��
PixelCostComputorBase::PixelCostComputorBase(void)
{
	m_pNearFrames=nullptr;
	m_pCurrentFrame=nullptr;
	m_fDataCostWeight=1.0;
	m_dColorMissPenalty=30;
}

//��������
PixelCostComputorBase::~PixelCostComputorBase(void)
{

}

//���õ�ǰ֡�����ڣ����㣩֡
void PixelCostComputorBase::SetFrames(VDRVideoFrame *currentFrame,std::vector<VDRVideoFrame*> * nearFrames)
{
	m_pCurrentFrame=currentFrame;
	m_pNearFrames=nearFrames;
}

// ������ɫ��disparity��Sigmaֵ��sigma_c��sigma_d)
void PixelCostComputorBase::SetSigma(double dColorSigma,double dDspSigma)
{
	m_dColorSigma=dColorSigma;
	m_dDspSigma=dDspSigma;
}






