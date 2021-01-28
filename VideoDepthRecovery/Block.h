/***************************************
 �˺�������Ҫ�����жϣ�x,y)������Ƿ���ͼ��������
*********************************************/


#pragma once
#include <vector>
#include <memory>
namespace blk
{


class Point
{
public:
	Point():X(0),Y(0){}                 //���캯�����и�ֵ
	Point(int x,int y):X(x),Y(y){}
	int X;
	int Y;
};
}

class Block
{
public:

	//���� ���캯��
	Block(void);                                                  
	Block(int x,int y,int width,int height,
		std::shared_ptr<Block>pInnerBlock=std::shared_ptr<Block>());
    
	//������ �ж�(x,y)�Ƿ���ͼ����
	bool IsInBlock(int x,int y)const
	{
		return x>=m_X&&y>=m_Y&& x<m_X+m_iWidth&&y<m_Y+m_iHeight;
	}
	
	//���� ��������
	~Block(void);

	//����  �õ�ͼ���Ե��
	void GetEdgePoints(std::vector<int>& edgePoints)const;


public:
	int m_X;              //���Ͻ�����
	int m_Y;

	int m_iWidth;         //ͼ���ȡ��߶�
	int m_iHeight;

	std::shared_ptr<Block>m_pInnerBlock;     //ֻ��Block���������ָ��
};

