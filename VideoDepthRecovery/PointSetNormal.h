#pragma once
#include "WmlVector3.h"
#include <vector>
/*
class PointSetNormal
{
public:
	PointSetNormal(void);
	~PointSetNormal(void);
};
*/
bool SVDPtsSetNormal(std::vector<Wml::Vector3f>& inlinerPointSet,Wml::Vector3f& ansNormal);

int	solveSVD(float* mat,int matRow,int matCol,Wml::Vector3f& abc);