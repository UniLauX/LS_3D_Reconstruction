#pragma once
#include "Block.h"
#include "DataCostUnit.h"
#include "WmlGVector.h"
#include "WmlVector2.h"
#include "WmlGMatrix.h"


typedef struct
{
    int EdgePointIndex;  //边缘点的索引值
    int NeighborDsp;    //邻居点的disparity
}EdgeNeighborInfo;

class LevenMarOptimizer
{
public:

	//构造函数
	//LevenMarOptimizer(void);
    LevenMarOptimizer(const std::vector<blk::Point> &SegmPoints, int iterationCount, int LayerCount, float Disc_k);
	//析构函数
	~LevenMarOptimizer(void);

	//设置某个Index上（不同layer上）的dataCostUnit值
	void SetDataCostAt(int index, DataCostUnit& dataCosti);


	//添加边缘点邻居的disparity
	void AddEdgeNeighborInfo( int PointIndex, int NeighborDsp );

	//利用LevenMar求a_i,b_i
	void Optimize(Wml::GVectord& x, double& MinValue);

	double GetAbsValue(const Wml::GVectord& x);



private:

	//利用约束条件
	double GetValue(Wml::GVectord& x);

	float GetDataCostValueAt(int index, float d);

	//（利用x向量）设置a_i,b_i,c_i的值
	void SetVar(const Wml::GVectord& x);


	//得到点x,y上的梯度方向
	void GetGradient(const Wml::GVectord& x,Wml::GVectord& Direction);

     //类似于 GetDataCostValueAt函数
	float GetDataCostGradForX(int index, float d);


	void GetHesse(const Wml::GVectord& x, Wml::GMatrixd& mtx);

private:
	int m_iMaxIterationCount;   //迭代此处
	int m_iPointCount;          //segment点数
	int m_iLayerCount;          //层数
	float m_fDiscK;             // 论文公式（4.1）中的截断值

	const std::vector<blk::Point> &m_SegmPoints;       //segement点集
	std::vector<float*> m_DataCost;               //DataCost
	std::vector<float*> m_DataCostGradForX;

	std::vector<EdgeNeighborInfo> m_EdgeNeighborInfos;
	//std::list<float> m_constWs;

	float m_a, m_b, m_c;     //plane parameters[a_i,b_i,c_i]


};

