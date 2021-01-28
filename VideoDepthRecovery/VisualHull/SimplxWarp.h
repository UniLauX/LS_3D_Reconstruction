#ifndef  KXL_Simplx 
#define  KXL_Simplx

/*
我们要优化的问题是
max  sum d + （ci*xi）  （0=<i<N）
st.  aij * xi<=bj  (0=<j<M1)
      aik * xi>=bk (M1=<k<M1 + M2)
	  ait * xi = bt (M1 + M2=<t<M1+M2+M3)
	  其中
	  bj，bk，bt均大于等于0
	  /////////////////////////
	  也就是把问题写成标准线性规划问题后有 M1个小于式子，接着有M2个大约等于式子，
	  最后是M3个等于式子.令M = M1 +M2 + M3，变量个数N

	  目标函数objFunc 长度是 N + 1,依次存放 d ，c0，c1,c2,c3...
	  系数矩阵coffMat大小是M * N，B大小是M，
	  Nvar是变量个数
	  MEqua各种等式的个数，等于M
	  iCase = 0得到极大值，
				=-1，极值无穷大
				=1   没有极值

	函数返回值是目标函数的极大值
*/
double maxObjFunc( double*  coffMatA,double* B,
				  double*  objFunc,const int MEqua,const int NVar,const int M1,const int M2,int& iCase);

#endif