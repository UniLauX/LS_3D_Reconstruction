#include "SimplxWarp.h"
#include "./nrsimplx/nr.h"


/*
for each plane  Pi  we can construct a linear equation like
Pi * X <=0, that is 
xi * x + yi * y + zi * z + wi <=0
these are all the constraints

we want to get the max and min x,y,z.
for example the max  x.
下面我们要把它转化成线性规划的标准形式
1>所有变量非负数。所以 x =  x0 - x1,y = y0 - y1,z = z0 - z1,其中等号右边的量全大于0.
这样规划问题变成：
max  p = x0 - x1
st.   xi * x0 - xi * x1 + yi * y0  - yi * y1+ zi * z0 - zi * z1 + wi <=0
2>所有的不等式或者等式的右边的值大于 0 ，这个要根据wi的值判断
3>M1表示不等式中 小于号式子多少个，M2大于号，M3等于号，这里根据2>判断

//由于精度问题， 将所有的不等式中的0替换成 ESP.
///////////////////////////////////////////////////
下面是构造矩阵：
首先将所有的不等式中加入松弛变量 qi（i<约束条件的个数 M = M1 + M2 + M3）

对于小于号等式（也就是wi小于0）
xi * x0 - xi * x1 + yi * y0  - yi * y1+ zi * z0 - zi * z1  <= -wi   ---->

xi * x0 - xi * x1 + yi * y0  - yi * y1+ zi * z0 - zi * z1 + qi = -wi -->
加入人工变量
pi  = - wi - xi * x0 + xi * x1 - yi * y0  + yi * y1- zi * z0 + zi * z1 -qi  ---<1>
@@@@@@@@@@@@@@@@@@@@@@@@

对于大于号等式（也就是wi大于0）也就是原来不等式等式两边乘以 -1
-xi * x0 + xi * x1 - yi * y0  + yi * y1- zi * z0 + zi * z1  >= wi   ---->

-xi * x0 + xi * x1 - yi * y0  + yi * y1- zi * z0 + zi * z1 - qi= wi -->
加入人工变量
pi  = wi + xi * x0 - xi * x1 + yi * y0  - yi * y1+ zi * z0 - zi * z1 +qi    --<2>

/////////////////////////////////////////////////////
构造新的目标函数 newp = sum( -pi ),除非pi等于0，
否则加入人工变量前后不可能等价，而我们是希望他们等价的.
求解过程分两个阶段：
1利用新的目标函数 构造初始基本可行向量
2利用原来的目标函数求解
//////////////////////////////////////////////////////
现在构造求解矩阵
矩阵的行是目标函数值p,以及pi和newp，所以总共的有M + 2行.
矩阵的列是构造出的pi等式中的常数，原来的变量x0,x1,y0,y1,z0,z1
（我们不用管人工变量，因为他的求解函数中要求先放入所有的小于等于，然后放入所有的大于等于，
而小于等于式子中的qi系数一定是-1，大于等于式子中的qi的系数是1）,所以总共有N + 1列，变量个数加1.

根据所有的pi式子将矩阵填上。
//////////////////////////////////////////////////////////////////////

*/
double maxObjFunc( double*  coffMatA,double* B,
				  double*  objFunc,const int MEqua,const int NVar,const int M1,const int M2,int& iCase)
{
	const int NVarPlus = NVar + 1;
	const int MEquaPlus = MEqua + 2;
	DP*  tmpMat = new DP [ NVarPlus* MEquaPlus ];
	//首先填充目标函数行，第0行
	for (int i = 0;i<NVarPlus;i++)
		tmpMat[ i ] = objFunc[ i ];

	for (int i = 0;i<NVarPlus;i++)
		tmpMat[ i  + NVarPlus * (MEquaPlus - 1) ] = 0.0;

	//根据人工变量式子填充矩阵的
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