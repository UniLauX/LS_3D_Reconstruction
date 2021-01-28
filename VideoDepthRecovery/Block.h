/***************************************
 此函数的主要作用判断（x,y)坐标点是否在图像坐标内
*********************************************/


#pragma once
#include <vector>
#include <memory>
namespace blk
{


class Point
{
public:
	Point():X(0),Y(0){}                 //构造函数进行赋值
	Point(int x,int y):X(x),Y(y){}
	int X;
	int Y;
};
}

class Block
{
public:

	//声明 构造函数
	Block(void);                                                  
	Block(int x,int y,int width,int height,
		std::shared_ptr<Block>pInnerBlock=std::shared_ptr<Block>());
    
	//声明： 判断(x,y)是否在图像内
	bool IsInBlock(int x,int y)const
	{
		return x>=m_X&&y>=m_Y&& x<m_X+m_iWidth&&y<m_Y+m_iHeight;
	}
	
	//声明 析构函数
	~Block(void);

	//声明  得到图像边缘点
	void GetEdgePoints(std::vector<int>& edgePoints)const;


public:
	int m_X;              //左上角坐标
	int m_Y;

	int m_iWidth;         //图像宽度、高度
	int m_iHeight;

	std::shared_ptr<Block>m_pInnerBlock;     //只想Block对象的智能指针
};

