#pragma once
#include "DataCostUnit.h"
#include "ZImage.h"
class DataCost
{
public:
	//声明： 构造函数
	DataCost(void);
	DataCost(int width,int height,int dspLevel,bool setZero=false);

    //声明：  析构函数
	~DataCost(void);

	//声明： 创建所有Level图像元素存储空间（float类型)，并置其初值为0
	 void Create(int width,int height,int dspLevel,bool setZero=false );


	//声明：  得到level_i上（x,y)坐标位置元素（值)
	 float GetValueAt(int x,int y,int level_i);

    //声明：   At(x,y,level_i)得到level_i上（x,y)坐标位置元素（值)
	 float &At(int x,int y,int level_i);


	 //声明：  获得（x,y)坐标上的DataCostUnit值
	void  GetDataCostUnit(int x,int y,DataCostUnit &dataCostUnit);

	//得到dataCost对象（指针）地址
	float *GetBits()
	{
		return m_pfData;
	}

	//得到图像宽度
	int GetWidth()
	{
		return m_iWidth;
	}

	
	//得到disparity的层数
	int GetDspLevels()
	{
		return m_iDspLevels;
	}

	//在labelImg上设置每个坐标点最小disparity对应的level
	void SetLabelImg(int offsetX,int offsetY,int blockWidth,int blockHeight,ZIntImage &labelImg);




private:

	int m_iWidth;       //图像宽度
	int m_iHeight;      //图像高度
	int m_iDspLevels;   //disparity的层数（Level)

	float * m_pfData;    //DataCostUnit对象指针
	int m_iSlice;        //一层中的元素个数 （m_iSlice=m_iWidth*m_iHeight）
	int m_iTotal;        //所有层中的元素总数（m_iTotal=m_iSlice*m_iDspLevels)

};

