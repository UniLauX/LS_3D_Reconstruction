#pragma once
#include "Block.h"
#include "DataCostUnit.h"
#include "WmlGVector.h"
#include "WmlVector2.h"
#include "WmlGMatrix.h"


typedef struct
{
    int EdgePointIndex;  //��Ե�������ֵ
    int NeighborDsp;    //�ھӵ��disparity
}EdgeNeighborInfo;

class LevenMarOptimizer
{
public:

	//���캯��
	//LevenMarOptimizer(void);
    LevenMarOptimizer(const std::vector<blk::Point> &SegmPoints, int iterationCount, int LayerCount, float Disc_k);
	//��������
	~LevenMarOptimizer(void);

	//����ĳ��Index�ϣ���ͬlayer�ϣ���dataCostUnitֵ
	void SetDataCostAt(int index, DataCostUnit& dataCosti);


	//��ӱ�Ե���ھӵ�disparity
	void AddEdgeNeighborInfo( int PointIndex, int NeighborDsp );

	//����LevenMar��a_i,b_i
	void Optimize(Wml::GVectord& x, double& MinValue);

	double GetAbsValue(const Wml::GVectord& x);



private:

	//����Լ������
	double GetValue(Wml::GVectord& x);

	float GetDataCostValueAt(int index, float d);

	//������x����������a_i,b_i,c_i��ֵ
	void SetVar(const Wml::GVectord& x);


	//�õ���x,y�ϵ��ݶȷ���
	void GetGradient(const Wml::GVectord& x,Wml::GVectord& Direction);

     //������ GetDataCostValueAt����
	float GetDataCostGradForX(int index, float d);


	void GetHesse(const Wml::GVectord& x, Wml::GMatrixd& mtx);

private:
	int m_iMaxIterationCount;   //�����˴�
	int m_iPointCount;          //segment����
	int m_iLayerCount;          //����
	float m_fDiscK;             // ���Ĺ�ʽ��4.1���еĽض�ֵ

	const std::vector<blk::Point> &m_SegmPoints;       //segement�㼯
	std::vector<float*> m_DataCost;               //DataCost
	std::vector<float*> m_DataCostGradForX;

	std::vector<EdgeNeighborInfo> m_EdgeNeighborInfos;
	//std::list<float> m_constWs;

	float m_a, m_b, m_c;     //plane parameters[a_i,b_i,c_i]


};

