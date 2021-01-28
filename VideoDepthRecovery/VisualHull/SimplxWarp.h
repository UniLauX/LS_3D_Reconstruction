#ifndef  KXL_Simplx 
#define  KXL_Simplx

/*
����Ҫ�Ż���������
max  sum d + ��ci*xi��  ��0=<i<N��
st.  aij * xi<=bj  (0=<j<M1)
      aik * xi>=bk (M1=<k<M1 + M2)
	  ait * xi = bt (M1 + M2=<t<M1+M2+M3)
	  ����
	  bj��bk��bt�����ڵ���0
	  /////////////////////////
	  Ҳ���ǰ�����д�ɱ�׼���Թ滮������� M1��С��ʽ�ӣ�������M2����Լ����ʽ�ӣ�
	  �����M3������ʽ��.��M = M1 +M2 + M3����������N

	  Ŀ�꺯��objFunc ������ N + 1,���δ�� d ��c0��c1,c2,c3...
	  ϵ������coffMat��С��M * N��B��С��M��
	  Nvar�Ǳ�������
	  MEqua���ֵ�ʽ�ĸ���������M
	  iCase = 0�õ�����ֵ��
				=-1����ֵ�����
				=1   û�м�ֵ

	��������ֵ��Ŀ�꺯���ļ���ֵ
*/
double maxObjFunc( double*  coffMatA,double* B,
				  double*  objFunc,const int MEqua,const int NVar,const int M1,const int M2,int& iCase);

#endif