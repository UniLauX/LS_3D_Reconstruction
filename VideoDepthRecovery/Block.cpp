#include "Block.h"


//构造函数（无参数）初始化
Block::Block(void):m_X(0),m_Y(0),m_iWidth(0),m_iHeight(0),m_pInnerBlock(nullptr)
{
}

//构造函数（有参数）初始化
Block::Block(int x,int y,int width,int height, std::shared_ptr<Block>pInnerBlock)
	:m_X(x),m_Y(y),m_iWidth(width),m_iHeight(height),m_pInnerBlock(pInnerBlock)
{

}

//析构函数
Block::~Block(void)
{
}


//得到图像边缘点坐标，存放在 edgePoints中
void Block::GetEdgePoints(std::vector<int>& edgePoints)const
{
     int edgePointCount=m_iWidth*2+m_iHeight*2-4;    //图像边缘点个数
     edgePoints.resize(edgePointCount*2);     //edgePoints中连续两个元素分别存储x坐标和y坐标

	 int px,py,pointIndex=0;
	 for(int y=0;y<m_iHeight;y++)
	 {
	// 最左边一列点（包含最上、最下）
		 px=m_X+0;
		 py=m_Y+y;
		 edgePoints[pointIndex++]=px;
		 edgePoints[pointIndex++]=py;

	//最右边一列点（包含最上、最下）
		 px=m_X+m_iWidth-1;
		 py=m_Y+y;
		 edgePoints[pointIndex++]=px;
		 edgePoints[pointIndex++]=py;

	 }

	 for(int x=1;x<m_iWidth;x++)
	 {
	 //最上面一行点（不包含最左、最右）
		 px=m_X+x;
		 py=m_Y+0;
		 edgePoints[pointIndex++]=px;
		 edgePoints[pointIndex++]=py;


     //最下面一行点（不包含最左、最右）
		 px=m_X+x;
		 py=m_Y+m_iHeight-1;
		 edgePoints[pointIndex++]=px;
		 edgePoints[pointIndex++]=py;
	 }
}




