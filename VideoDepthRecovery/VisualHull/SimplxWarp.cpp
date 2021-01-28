#include "SimplxWarp.h"
#include "./nrsimplx/nr.h"


/*
for each plane  Pi  we can construct a linear equation like
Pi * X <=0, that is 
xi * x + yi * y + zi * z + wi <=0
these are all the constraints

we want to get the max and min x,y,z.
for example the max  x.
��������Ҫ����ת�������Թ滮�ı�׼��ʽ
1>���б����Ǹ��������� x =  x0 - x1,y = y0 - y1,z = z0 - z1,���еȺ��ұߵ���ȫ����0.
�����滮�����ɣ�
max  p = x0 - x1
st.   xi * x0 - xi * x1 + yi * y0  - yi * y1+ zi * z0 - zi * z1 + wi <=0
2>���еĲ���ʽ���ߵ�ʽ���ұߵ�ֵ���� 0 �����Ҫ����wi��ֵ�ж�
3>M1��ʾ����ʽ�� С�ں�ʽ�Ӷ��ٸ���M2���ںţ�M3���ںţ��������2>�ж�

//���ھ������⣬ �����еĲ���ʽ�е�0�滻�� ESP.
///////////////////////////////////////////////////
�����ǹ������
���Ƚ����еĲ���ʽ�м����ɳڱ��� qi��i<Լ�������ĸ��� M = M1 + M2 + M3��

����С�ںŵ�ʽ��Ҳ����wiС��0��
xi * x0 - xi * x1 + yi * y0  - yi * y1+ zi * z0 - zi * z1  <= -wi   ---->

xi * x0 - xi * x1 + yi * y0  - yi * y1+ zi * z0 - zi * z1 + qi = -wi -->
�����˹�����
pi  = - wi - xi * x0 + xi * x1 - yi * y0  + yi * y1- zi * z0 + zi * z1 -qi  ---<1>
@@@@@@@@@@@@@@@@@@@@@@@@

���ڴ��ںŵ�ʽ��Ҳ����wi����0��Ҳ����ԭ������ʽ��ʽ���߳��� -1
-xi * x0 + xi * x1 - yi * y0  + yi * y1- zi * z0 + zi * z1  >= wi   ---->

-xi * x0 + xi * x1 - yi * y0  + yi * y1- zi * z0 + zi * z1 - qi= wi -->
�����˹�����
pi  = wi + xi * x0 - xi * x1 + yi * y0  - yi * y1+ zi * z0 - zi * z1 +qi    --<2>

/////////////////////////////////////////////////////
�����µ�Ŀ�꺯�� newp = sum( -pi ),����pi����0��
��������˹�����ǰ�󲻿��ܵȼۣ���������ϣ�����ǵȼ۵�.
�����̷������׶Σ�
1�����µ�Ŀ�꺯�� �����ʼ������������
2����ԭ����Ŀ�꺯�����
//////////////////////////////////////////////////////
���ڹ���������
���������Ŀ�꺯��ֵp,�Լ�pi��newp�������ܹ�����M + 2��.
��������ǹ������pi��ʽ�еĳ�����ԭ���ı���x0,x1,y0,y1,z0,z1
�����ǲ��ù��˹���������Ϊ������⺯����Ҫ���ȷ������е�С�ڵ��ڣ�Ȼ��������еĴ��ڵ��ڣ�
��С�ڵ���ʽ���е�qiϵ��һ����-1�����ڵ���ʽ���е�qi��ϵ����1��,�����ܹ���N + 1�У�����������1.

�������е�piʽ�ӽ��������ϡ�
//////////////////////////////////////////////////////////////////////

*/
double maxObjFunc( double*  coffMatA,double* B,
				  double*  objFunc,const int MEqua,const int NVar,const int M1,const int M2,int& iCase)
{
	const int NVarPlus = NVar + 1;
	const int MEquaPlus = MEqua + 2;
	DP*  tmpMat = new DP [ NVarPlus* MEquaPlus ];
	//�������Ŀ�꺯���У���0��
	for (int i = 0;i<NVarPlus;i++)
		tmpMat[ i ] = objFunc[ i ];

	for (int i = 0;i<NVarPlus;i++)
		tmpMat[ i  + NVarPlus * (MEquaPlus - 1) ] = 0.0;

	//�����˹�����ʽ���������
	for (int iRow = 1;iRow<MEquaPlus - 1;++iRow)
	{
		const int oldRow = iRow - 1;
		DP* curRow = tmpMat + iRow * NVarPlus;
		double * curCoffRow = coffMatA + oldRow * NVar;

		curRow[ 0 ] = B[ oldRow ];
		for(int iCol = 1;iCol<NVarPlus;iCol++)
			curRow[ iCol ] = -curCoffRow[ iCol - 1 ];

	}

	const int M3 = MEqua - M1 - M2;
	Vec_INT izrov(NVar),iposv(MEqua);
	Mat_DP a(tmpMat,MEquaPlus,NVarPlus);

	
    NR::simplx(a,M1,M2,M3,iCase,izrov,iposv);

	double ans =  a[0][0];
	delete  [] tmpMat;
	
	return ans;
}