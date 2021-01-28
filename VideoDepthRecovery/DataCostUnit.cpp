#include "DataCostUnit.h"


DataCostUnit::DataCostUnit(void)
{
}


DataCostUnit::~DataCostUnit(void)
{
}



//（赋值）初始化
void DataCostUnit::Init(float *data,int step,int size)
{
	m_pfData=data;
	m_iStep=step;
	m_iSize=size;   
}

void DataCostUnit::SetValue(DataCostUnit & DataCostRef)
{
	if(m_iSize!=DataCostRef.m_iSize)
	{
		//判断DataCost大小是否符合
		std::cout<<"Error: can't not be seted by another DataCostUnit,because the size is not equal"<<std::endl;
		exit(0);   //非正常退出程序
	}
	for(int i=0;i<m_iSize;i++)
	{
		m_pfData[i*m_iStep]=DataCostRef[i];
	}
		
}

//定义索引（index)与下标运算符[]之间的关系
float & DataCostUnit::operator[](int index)
{
	//索引超出正常范围
    if(index>=m_iSize)
	{
		std::cout<<"Error: the index of DataCostUnit is out of range. (index: "<<index<<std::endl;
		exit(0);   //非正常退出程序
	}

	return m_pfData[index*m_iStep];
}

