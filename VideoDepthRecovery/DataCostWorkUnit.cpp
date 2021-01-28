#include "DataCostWorkUnit.h"


/*
//构造函数（默认）
DataCostWorkUnit::DataCostWorkUnit(void)
{

}
*/
//析构函数
DataCostWorkUnit::~DataCostWorkUnit(void)
{
}

//构造函数（重载）,主要赋值初始化
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

	// 没有（偏移图像的情况下）初始化DataCost并得到label
	int y=m_iLineIndex;
	DataCostUnit dataCosti;
	for(int x = m_block.m_X; x < m_block.m_X + m_block.m_iWidth; x++)
	{
		m_dataCost.GetDataCostUnit(x-m_block.m_X, y - m_block.m_Y, dataCosti);
		int bestLabel;
		m_pixelCostComputor.PixelDataCost(x, y, m_dspV, dataCosti, bestLabel);
		if(m_block.m_pInnerBlock->IsInBlock(x, y) == true)
			m_labelImg.SetPixel(x, y, 0, bestLabel);
	     
		//计算所得bestLabel不正确
		//std::cout<<"bestLabel: "<<bestLabel<<std::endl;
	}
	std::cout<<m_iLineIndex<<" ";


}



//有（偏移图像的情况下）初始化DataCost并得到label
void DataCostWorkUnit::Excute_offsetImg() throw()
{
	int y=m_iLineIndex;
	int subDspLevel=DepthParaWidget::GetInstance()->m_iSubLevels*2+1;   //设置disparity的层数
	//std::cout<<"subDspLevel"<<subDspLevel<<std::endl;

	std::vector<double> subDspV(subDspLevel);
	DataCostUnit dataCosti;
    for(int x=m_block.m_X;x<m_block.m_X+m_block.m_iWidth;x++)
	{
		auto pBegin=m_dspV.cbegin()+m_pOffsetImg->at(x,y);          // ？？？不理解含义
		auto pEnd=pBegin+subDspLevel;
       // std::cout<<"pBegin: "<<*pBegin<<"  ";
	   //	std::cout<<"pEnd: "<<*pEnd<<std::endl;

		//std::cout<<"m_dspV.begin(): "<<*m_dspV.begin()<<std::endl;
		//std::cout<<"m_pOffsetImg->at(x,y): "<<m_pOffsetImg->at(x,y)<<std::endl;
		


		subDspV.assign(pBegin,pEnd);  //assign() 函数将区间[start, end)的元素赋到当前vector
		
		//获得坐标（x,y）在块上的datacost.
		m_dataCost.GetDataCostUnit(x-m_block.m_X,y-m_block.m_Y,dataCosti);

		int bestLabel=0;
		//得到相应的Label
		m_pixelCostComputor.PixelDataCost(x,y,subDspV,dataCosti,bestLabel);
       
		//如果在块内，则设置相应的label.
		if(m_block.m_pInnerBlock->IsInBlock(x,y)==true)
			m_labelImg.SetPixel(x,y,0,bestLabel);


		//std::cout<<"bestLabel: "<< bestLabel<<std::endl;
	}

}