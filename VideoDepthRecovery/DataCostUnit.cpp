#include "DataCostUnit.h"


DataCostUnit::DataCostUnit(void)
{
}


DataCostUnit::~DataCostUnit(void)
{
}



//����ֵ����ʼ��
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
		//�ж�DataCost��С�Ƿ����
		std::cout<<"Error: can't not be seted by another DataCostUnit,because the size is not equal"<<std::endl;
		exit(0);   //�������˳�����
	}
	for(int i=0;i<m_iSize;i++)
	{
		m_pfData[i*m_iStep]=DataCostRef[i];
	}
		
}

//����������index)���±������[]֮��Ĺ�ϵ
float & DataCostUnit::operator[](int index)
{
	//��������������Χ
    if(index>=m_iSize)
	{
		std::cout<<"Error: the index of DataCostUnit is out of range. (index: "<<index<<std::endl;
		exit(0);   //�������˳�����
	}

	return m_pfData[index*m_iStep];
}

