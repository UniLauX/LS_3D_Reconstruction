#pragma once
#include <iostream>
class DataCostUnit
{
public:
	//������ ���캯��
	DataCostUnit(void);

	//������ ��������
	~DataCostUnit(void);

	//������  (��ֵ����ʼ��
	void Init(float *data,int step,int size);


	//�����±����(������֮��Ĺ�ϵ��
	float & operator [](int index);


	//��ָ�����m_pfData ��ֵ
	void SetValue(DataCostUnit & DataCostRef);

private:
	float *m_pfData;    //ָ��DataCostUnit�����ָ��
	int m_iStep;        // ����(m_iSlice:���С)
	int m_iSize;        // DataCostUnit����Ĵ�С��m_iDspLevels)

};

