#include "PointSetNormal.h"
#include "mkl_lapack.h"

/*
PointSetNormal::PointSetNormal(void)
{
}
PointSetNormal::~PointSetNormal(void)
{
}
*/

bool SVDPtsSetNormal(std::vector<Wml::Vector3f>& inlinerPointSet,Wml::Vector3f& ansNormal)
{
	int ptnum = inlinerPointSet.size();

	Wml::Vector3f barycenter(0.0f,0.0f,0.0f);
	for(int iPoint = 0;iPoint<ptnum;iPoint++)
		barycenter += inlinerPointSet[ iPoint ];

	//avg
	barycenter /= (float) ptnum;

	int matRow = ptnum;
	int matCol = 3;

	float* mat = new float [matRow * matCol];

	for(int idx = 0;idx<ptnum;idx++)
	{
		Wml::Vector3f tmpVec = inlinerPointSet[ idx] - barycenter;
		mat[idx*3] = tmpVec[0];
		mat[idx*3 + 1] = tmpVec[1];
		mat[idx*3 + 2] = tmpVec[2];
	}


	Wml::Vector3f abc;
	int info = solveSVD(mat,matRow,matCol,abc);


	if(info==0)
	{		
		float len = abc.Length();
		if(len>0.9 && len<1.1)
		{
			abc.Normalize();
			ansNormal = Wml::Vector3f(abc[0],abc[1],abc[2]);
			ansNormal.Normalize();
		}
		else printf("len: %f\n",len);
	}

	delete mat;
	mat = 0;

	return (info==0);

	return 0;

}


int	solveSVD(float* mat,int matRow,int matCol,Wml::Vector3f& abc)
{
	float* matT = new float [matRow*matCol];

	for(int iRow = 0;iRow<matCol;iRow++)
		for(int jCol = 0;jCol<matRow;jCol++)
			matT[iRow*matRow + jCol] = mat[iRow + jCol * matCol];

	char jobu  = 'N';
	char jobvt = 'O';
	int info;
	int m       = matRow;
	int n       = matCol;
	int lda     = m;
	int ldu     = 1;
	int ldvt    = 1;

	float *s    = new float[n];
	int lwork   = (5 * m) * 10;  
	float *work = new float[lwork];	
	
	//printf("begin svd\n");
	sgesvd(&jobu, &jobvt, &m, &n, matT, &lda, s, NULL, &ldu, NULL, &ldvt, work, &lwork, &info);
	//printf("solved svd\n");
	if(info == 0)
	{
		int minEigIndex = matCol  - 1;
		abc[0]  = matT[minEigIndex];
		minEigIndex += matRow;
		abc[1]  = matT[minEigIndex ];
		minEigIndex += matRow;
		abc[2]  = matT[minEigIndex ];
	}

	delete s;
	s = 0;
	delete work;
	work = 0;
	delete matT;
	matT= 0;
	
	return info;
}


