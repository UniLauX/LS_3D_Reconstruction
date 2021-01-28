#include "Block.h"


//���캯�����޲�������ʼ��
Block::Block(void):m_X(0),m_Y(0),m_iWidth(0),m_iHeight(0),m_pInnerBlock(nullptr)
{
}

//���캯�����в�������ʼ��
Block::Block(int x,int y,int width,int height, std::shared_ptr<Block>pInnerBlock)
	:m_X(x),m_Y(y),m_iWidth(width),m_iHeight(height),m_pInnerBlock(pInnerBlock)
{

}

//��������
Block::~Block(void)
{
}


//�õ�ͼ���Ե�����꣬����� edgePoints��
void Block::GetEdgePoints(std::vector<int>& edgePoints)const
{
     int edgePointCount=m_iWidth*2+m_iHeight*2-4;    //ͼ���Ե�����
     edgePoints.resize(edgePointCount*2);     //edgePoints����������Ԫ�طֱ�洢x�����y����

	 int px,py,pointIndex=0;
	 for(int y=0;y<m_iHeight;y++)
	 {
	// �����һ�е㣨�������ϡ����£�
		 px=m_X+0;
		 py=m_Y+y;
		 edgePoints[pointIndex++]=px;
		 edgePoints[pointIndex++]=py;

	//���ұ�һ�е㣨�������ϡ����£�
		 px=m_X+m_iWidth-1;
		 py=m_Y+y;
		 edgePoints[pointIndex++]=px;
		 edgePoints[pointIndex++]=py;

	 }

	 for(int x=1;x<m_iWidth;x++)
	 {
	 //������һ�е㣨�������������ң�
		 px=m_X+x;
		 py=m_Y+0;
		 edgePoints[pointIndex++]=px;
		 edgePoints[pointIndex++]=py;


     //������һ�е㣨�������������ң�
		 px=m_X+x;
		 py=m_Y+m_iHeight-1;
		 edgePoints[pointIndex++]=px;
		 edgePoints[pointIndex++]=py;
	 }
}




