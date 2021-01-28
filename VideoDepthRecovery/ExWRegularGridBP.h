#pragma once

#pragma once

#include "ZImage.h"
#include "ZCubeImage.h"
#include <math.h>
#include <Windows.h>

namespace NumericalAlgorithm{

	class CExWRegularGridBP
	{
	public:
		CExWRegularGridBP(void);
		~CExWRegularGridBP(void);

		friend class CExWRegularGridBPWorkUnit;

	private:

		static void Bp_CP(ZCubeFloatImage& uI, ZCubeFloatImage& dI, ZCubeFloatImage& rI, ZCubeFloatImage& lI, 
			ZCubeFloatImage& dataCost, ZIntImage& offsetImg, ZFloatImage& wImg, ZFloatImage& truncImg);
		static void Dt(float* f, float w);
		static void msg(float* s1, float* s2, float* s3, float* s4,float* dst, int offset, float w, float trunc);
	public:
		//����������DataCost�����ص�Label��ÿ������Label��Offset��
		//ÿ����������Χ�ھӸ������Ե�Weight��ͨ������1��
		//�Լ�ÿ�����صĽض�ֵ
		static void Solve(ZCubeFloatImage& dataCost, ZIntImage& labelImg, ZIntImage& offsetImg, ZFloatImage& wImg, ZFloatImage& truncImg);

		//static float m_DW;
		//static float m_DiscK;
		static int	 VALUES;
		static int m_iMaxIter;
		static int m_iThreadCount;
	};

}