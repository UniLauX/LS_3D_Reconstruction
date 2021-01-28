#include "DataCostWorkUnit.h"


/*
//���캯����Ĭ�ϣ�
DataCostWorkUnit::DataCostWorkUnit(void)
{

}
*/
//��������
DataCostWorkUnit::~DataCostWorkUnit(void)
{
}

//���캯�������أ�,��Ҫ��ֵ��ʼ��
DataCostWorkUnit::DataCostWorkUnit(int lineID, const Block &block, const std::vector<double> &dspV, 
	ZIntImage &labelImg, DataCost &dataCost, PixelCostComputorBase & pixelCostComputor, ZIntImage *pOffsetImg)
	: m_block(block), m_dspV(dspV), m_labelImg(labelImg), m_dataCost(dataCost), m_pixelCostComputor(pixelCostComputor)
{
	m_iLineIndex = lineID;
	m_pOffsetImg = pOffsetImg;
}


void DataCostWorkUnit::Execute() throw()
{
	if(m_pOffsetImg!=nullptr)
	{
		Excute_offsetImg();
    	return;
	}

	// û�У�ƫ��ͼ�������£���ʼ��DataCost���õ�label
	int y=m_iLineIndex;
	DataCostUnit dataCosti;
	for(int x = m_block.m_X; x < m_block.m_X + m_block.m_iWidth; x++)
	{
		m_dataCost.GetDataCostUnit(x-m_block.m_X, y - m_block.m_Y, dataCosti);
		int bestLabel;
		m_pixelCostComputor.PixelDataCost(x, y, m_dspV, dataCosti, bestLabel);
		if(m_block.m_pInnerBlock->IsInBlock(x, y) == true)
			m_labelImg.SetPixel(x, y, 0, bestLabel);
	     
		//��������bestLabel����ȷ
		//std::cout<<"bestLabel: "<<bestLabel<<std::endl;
	}
	std::cout<<m_iLineIndex<<" ";


}



//�У�ƫ��ͼ�������£���ʼ��DataCost���õ�label
void DataCostWorkUnit::Excute_offsetImg() throw()
{
	int y=m_iLineIndex;
	int subDspLevel=DepthParaWidget::GetInstance()->m_iSubLevels*2+1;   //����disparity�Ĳ���
	//std::cout<<"subDspLevel"<<subDspLevel<<std::endl;

	std::vector<double> subDspV(subDspLevel);
	DataCostUnit dataCosti;
    for(int x=m_block.m_X;x<m_block.m_X+m_block.m_iWidth;x++)
	{
		auto pBegin=m_dspV.cbegin()+m_pOffsetImg->at(x,y);          // ����������⺬��
		auto pEnd=pBegin+subDspLevel;
       // std::cout<<"pBegin: "<<*pBegin<<"  ";
	   //	std::cout<<"pEnd: "<<*pEnd<<std::endl;

		//std::cout<<"m_dspV.begin(): "<<*m_dspV.begin()<<std::endl;
		//std::cout<<"m_pOffsetImg->at(x,y): "<<m_pOffsetImg->at(x,y)<<std::endl;
		


		subDspV.assign(pBegin,pEnd);  //assign() ����������[start, end)��Ԫ�ظ�����ǰvector
		
		//������꣨x,y���ڿ��ϵ�datacost.
		m_dataCost.GetDataCostUnit(x-m_block.m_X,y-m_block.m_Y,dataCosti);

		int bestLabel=0;
		//�õ���Ӧ��Label
		m_pixelCostComputor.PixelDataCost(x,y,subDspV,dataCosti,bestLabel);
       
		//����ڿ��ڣ���������Ӧ��label.
		if(m_block.m_pInnerBlock->IsInBlock(x,y)==true)
			m_labelImg.SetPixel(x,y,0,bestLabel);


		//std::cout<<"bestLabel: "<< bestLabel<<std::endl;
	}

}