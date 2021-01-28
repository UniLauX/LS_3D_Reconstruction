#include "ImgPartiton.h"


ImgPartiton::ImgPartiton(void)
{
}


ImgPartiton::~ImgPartiton(void)
{
}


//�ֿ����(�����ڲ�����⣩
void ImgPartiton::GetBlockStrategy(double oriImgWidth,double oriImgHeight,int dspLayer,double overlap,int &BlockCountX,int &BlockCountY)
{
	double maxSize=960.0*540.0*100/1.8;      //�����

	BlockCountX=1;
	BlockCountY=1;

	double oriBlockWidth = oriImgWidth;
	double oriBlockHeight = oriImgHeight;
	while(oriBlockWidth*(1.0+overlap*std::min(BlockCountX-1,2))            //���ص�����չ���������
		 *oriBlockHeight*(1.0+overlap*std::min(BlockCountY-1,2))
		 *dspLayer>maxSize
		)
	{
	    if(oriBlockWidth>=oriBlockHeight)
		{
			++BlockCountX;
			oriBlockWidth = oriImgWidth / BlockCountX;
		}
		else
		{
			++BlockCountY;
			oriBlockHeight = oriBlockHeight / BlockCountY;
		}
	}
}


//�����
Block ImgPartiton::GetBlockAt(int BlockIdX,int BlockIdY,int iImgWidth,int iImgHeight)
{

	//�жϿ��index�Ƿ��ں��ʵķ�Χ��

	std::cout<<"BlockCountX: "<<m_iBlockCountX<<"  BlockCountY: "<<m_iBlockCountY<<std::endl;

	if(BlockIdX<0||BlockIdX>=m_iBlockCountX||BlockIdY<0||BlockIdY>=m_iBlockCountY)
	{
	   std::cout<<"Error: Block Index is out of range"<<std::endl;
		throw std::exception("Error: Block Index is out of range.");
	}


	double blockWidth=(double)iImgWidth/m_iBlockCountX;     //ÿ��Ŀ��
	double blockHeight=(double)iImgHeight/m_iBlockCountY;   //ÿ��ĸ߶�



	//***   Inner Block   *** //
    int innerX=BlockIdX!=0? blockWidth*BlockIdX+0.5 : 0;       //�ڲ�x����=����*���
	int innerY=BlockIdY!=0? blockHeight*BlockIdY+0.5 : 0;       //�ڲ�y����=��߶�*���
	int innerWidth=blockWidth+0.5;   
	int innerHeight=blockHeight+0.5; 

    if(innerX+innerWidth>iImgWidth)
		innerWidth=iImgWidth-innerX;
	if(innerY+innerHeight>iImgHeight)
		innerHeight=iImgHeight-innerY;

	//***   Expand Block  ***//

	int expandWidth=blockWidth*m_dOverlap+0.5;
	int expandHeiht=blockHeight*m_dOverlap+0.5;

    int X=static_cast<int>(innerX-expandWidth)<0 ? 0 : static_cast<int>(innerX-expandWidth);
	int Y=static_cast<int>(innerY-expandHeiht)<0 ? 0 : static_cast<int>(innerY-expandHeiht);

	//ȷ��X_right������ͼ��ˮƽ��Χ
	int X_right=static_cast<int>(innerX+innerWidth-1+expandWidth)>iImgWidth? 
		iImgWidth-1:static_cast<int>(innerX+innerWidth-1+expandWidth);

	//ȷ��Y_bottom������ͼ��ֱ��Χ
	int Y_bottom=static_cast<int>(innerY+innerHeight-1+expandHeiht)>iImgHeight?
		iImgHeight-1:static_cast<int>(innerY+innerHeight-1+expandHeiht);

      return Block(X,Y,X_right-X+1,Y_bottom-Y+1,std::make_shared<Block>(innerX,innerY,innerWidth,innerHeight));
}


void ImgPartiton::print()
{
	std::cout<<"ImgPartition info"<<std::endl;
}