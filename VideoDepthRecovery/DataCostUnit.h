#pragma once
#include <iostream>
class DataCostUnit
{
public:
	//声明： 构造函数
	DataCostUnit(void);

	//声明： 析构函数
	~DataCostUnit(void);

	//声明：  (赋值）初始化
	void Init(float *data,int step,int size);


	//定义下标操作(与索引之间的关系）
	float & operator [](int index);


	//给指针变量m_pfData 赋值
	void SetValue(DataCostUnit & DataCostRef);

private:
	float *m_pfData;    //指向DataCostUnit对象的指针
	int m_iStep;        // 步长(m_iSlice:层大小)
	int m_iSize;        // DataCostUnit对象的大小（m_iDspLevels)

};

