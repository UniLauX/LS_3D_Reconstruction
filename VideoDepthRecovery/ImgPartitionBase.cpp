#include "ImgPartitionBase.h"


ImgPartitionBase::ImgPartitionBase(void)
{
	Init(0,0,0);
}

ImgPartitionBase::ImgPartitionBase(int BlockCountX,int BlockCountY,double dOverlap)
	:m_iBlockCountX(BlockCountX),m_iBlockCountY(BlockCountY),m_dOverlap(dOverlap){}

ImgPartitionBase::~ImgPartitionBase(void)
{
}


void ImgPartitionBase::Init(int BlockCountX,int BlockCountY,double dOverlap)
{
	m_iBlockCountX=BlockCountX;
	m_iBlockCountY=BlockCountY;
	m_dOverlap=dOverlap;
}

void ImgPartitionBase::GetBlockId(int x,int y,int iImgWidth,int iImgHeight,int &blockIdX,int &blockIdY)
{
	//每块的宽度和高度
    double blockWidth=(double)iImgWidth/m_iBlockCountX;
	double blockHeight=(double)iImgHeight/m_iBlockCountY;

	/*
	static_cast < type-id > ( expression )
	该运算符把expression转换为type-id类型，但没有运行时类型检查来保证转换的安全性。
	*/
	
	blockIdX=static_cast<int>(x/blockWidth+0.5);        //+0.5四舍五入          
	blockIdY=static_cast<int>(y/blockHeight+0.5);
}


//获取最大块大小（对此函数不是很理解）
void ImgPartitionBase::GetMaxBlockSize(int iImgWidth,int iImgHeight,int &width,int &height)
{
	int blockIdX=m_iBlockCountX/2;      //取中间的块（为最大块）
	int blockIdY=m_iBlockCountY/2;

	std::cout<<"blockIdX: "<<blockIdX<<" blockIdY:"<<blockIdY<<std::endl;
	std::cout<<"iImgWidth: "<<iImgWidth<<" iImgHeight:"<<iImgHeight<<std::endl;

	 Block block=GetBlockAt(blockIdX,blockIdY,iImgWidth,iImgHeight);
	 width=block.m_iWidth;
     height=block.m_iHeight;
	
}

void ImgPartitionBase::print()
{
	std::cout<<"Image partition Info."<<std::endl;
}


