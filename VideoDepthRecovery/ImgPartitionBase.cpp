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
	//ÿ��Ŀ�Ⱥ͸߶�
    double blockWidth=(double)iImgWidth/m_iBlockCountX;
	double blockHeight=(double)iImgHeight/m_iBlockCountY;

	/*
	static_cast < type-id > ( expression )
	���������expressionת��Ϊtype-id���ͣ���û������ʱ���ͼ������֤ת���İ�ȫ�ԡ�
	*/
	
	blockIdX=static_cast<int>(x/blockWidth+0.5);        //+0.5��������          
	blockIdY=static_cast<int>(y/blockHeight+0.5);
}


//��ȡ�����С���Դ˺������Ǻ���⣩
void ImgPartitionBase::GetMaxBlockSize(int iImgWidth,int iImgHeight,int &width,int &height)
{
	int blockIdX=m_iBlockCountX/2;      //ȡ�м�Ŀ飨Ϊ���飩
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


