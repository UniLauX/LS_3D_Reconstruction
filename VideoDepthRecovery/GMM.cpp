#include "GMM.h"

/*
GMM::GMM(void)
{
}
*/
GMM::GMM( cv::Mat& _model )  
{  
	//һ�����صģ�Ψһ��Ӧ����˹ģ�͵Ĳ�����������˵һ����˹ģ�͵Ĳ�������  
	//һ������RGB����ͨ��ֵ����3����ֵ��3*3��Э�������һ��Ȩֵ  
	const int modelSize = 3/*mean*/ + 9/*covariance*/ + 1/*component weight*/;  
	if( _model.empty() )  
	{  
		//һ��GMM����componentsCount����˹ģ�ͣ�һ����˹ģ����modelSize��ģ�Ͳ���  
		_model.create( 1, modelSize*componentsCount, CV_64FC1 );  
		_model.setTo(cv::Scalar(0));  
	}  
	else if( (_model.type() != CV_64FC1) || (_model.rows != 1) || (_model.cols != modelSize*componentsCount) )  
		CV_Error( CV_StsBadArg, "_model must have CV_64FC1 type, rows == 1 and cols == 13*componentsCount" );  

	model = _model;  

	//ע����Щģ�Ͳ����Ĵ洢��ʽ��������componentsCount��coefs����3*componentsCount��mean��  
	//��3*3*componentsCount��cov��  
	coefs = model.ptr<double>(0);  //GMM��ÿ�����صĸ�˹ģ�͵�Ȩֵ������ʼ�洢ָ��  
	mean = coefs + componentsCount; //��ֵ������ʼ�洢ָ��  
	cov = mean + 3*componentsCount;  //Э���������ʼ�洢ָ��  

	for( int ci = 0; ci < componentsCount; ci++ )  
		if( coefs[ci] > 0 )  
			//����GMM�е�ci����˹ģ�͵�Э�������Inverse������ʽDeterminant  
			//Ϊ�˺������ÿ���������ڸø�˹ģ�͵ĸ��ʣ�Ҳ�������������  
			CalcInverseCovAndDeterm( ci );   
}  



GMM::~GMM(void)
{
}

void GMM::AddSample( int ci, const cv::Vec3d color )  
{  
	sums[ci][0] += color[0]; sums[ci][1] += color[1]; sums[ci][2] += color[2];  
	prods[ci][0][0] += color[0]*color[0]; prods[ci][0][1] += color[0]*color[1]; prods[ci][0][2] += color[0]*color[2];  
	prods[ci][1][0] += color[1]*color[0]; prods[ci][1][1] += color[1]*color[1]; prods[ci][1][2] += color[1]*color[2];  
	prods[ci][2][0] += color[2]*color[0]; prods[ci][2][1] += color[2]*color[1]; prods[ci][2][2] += color[2]*color[2];  
	sampleCounts[ci]++;  
	totalSampleCount++;  
}  


//����Э�������Inverse������ʽDeterminant  
void GMM::CalcInverseCovAndDeterm( int ci )  
{  
	if( coefs[ci] > 0 )  
	{  
		//ȡ��ci����˹ģ�͵�Э�������ʼָ��  
		double *c = cov + 9*ci;  
		double dtrm =  
			covDeterms[ci] = c[0]*(c[4]*c[8]-c[5]*c[7]) - c[1]*(c[3]*c[8]-c[5]*c[6])   
			+ c[2]*(c[3]*c[7]-c[4]*c[6]);  

		//��C++�У�ÿһ�����õ��������Ͷ�ӵ�в�ͬ������, ʹ��<limits>����Ի�  
		//����Щ�����������͵���ֵ���ԡ���Ϊ�����㷨�Ľضϣ�����ʹ�ã���a=2��  
		//b=3ʱ 10*a/b == 20/b������������ô���أ�  
		//���С������epsilon�����������ˣ�С����ͨ��Ϊ���ø����������͵�  
		//����1����Сֵ��1֮������ʾ����dtrm���������С��������ô������Ϊ�㡣  
		//������ʽ��֤dtrm>0��������ʽ�ļ�����ȷ��Э����Գ�������������ʽ����0����  
		
		CV_Assert( dtrm > std::numeric_limits<double>::epsilon() );  
		//���׷��������  
		inverseCovs[ci][0][0] =  (c[4]*c[8] - c[5]*c[7]) / dtrm;  
		inverseCovs[ci][1][0] = -(c[3]*c[8] - c[5]*c[6]) / dtrm;  
		inverseCovs[ci][2][0] =  (c[3]*c[7] - c[4]*c[6]) / dtrm;  
		inverseCovs[ci][0][1] = -(c[1]*c[8] - c[2]*c[7]) / dtrm;  
		inverseCovs[ci][1][1] =  (c[0]*c[8] - c[2]*c[6]) / dtrm;  
		inverseCovs[ci][2][1] = -(c[0]*c[7] - c[1]*c[6]) / dtrm;  
		inverseCovs[ci][0][2] =  (c[1]*c[5] - c[2]*c[4]) / dtrm;  
		inverseCovs[ci][1][2] = -(c[0]*c[5] - c[2]*c[3]) / dtrm;  
		inverseCovs[ci][2][2] =  (c[0]*c[4] - c[1]*c[3]) / dtrm;  
	}  
}  


void GMM::InitLearning()
{  
	for( int ci = 0; ci < componentsCount; ci++)  
	{  
		sums[ci][0] = sums[ci][1] = sums[ci][2] = 0;  
		prods[ci][0][0] = prods[ci][0][1] = prods[ci][0][2] = 0;  
		prods[ci][1][0] = prods[ci][1][1] = prods[ci][1][2] = 0;  
		prods[ci][2][0] = prods[ci][2][1] = prods[ci][2][2] = 0;  
		sampleCounts[ci] = 0;  
	}  
	totalSampleCount = 0;  
}  

void GMM::EndLearning()
{
	const double variance = 0.01;
	for( int ci = 0; ci < componentsCount; ci++ )
	{
		int n = sampleCounts[ci];
		if( n == 0 )
			coefs[ci] = 0;
		else
		{
			coefs[ci] = (double)n/totalSampleCount;

			double* m = mean + 3*ci;
			m[0] = sums[ci][0]/n; m[1] = sums[ci][1]/n; m[2] = sums[ci][2]/n;

			double* c = cov + 9*ci;
			c[0] = prods[ci][0][0]/n - m[0]*m[0]; c[1] = prods[ci][0][1]/n - m[0]*m[1]; c[2] = prods[ci][0][2]/n - m[0]*m[2];
			c[3] = prods[ci][1][0]/n - m[1]*m[0]; c[4] = prods[ci][1][1]/n - m[1]*m[1]; c[5] = prods[ci][1][2]/n - m[1]*m[2];
			c[6] = prods[ci][2][0]/n - m[2]*m[0]; c[7] = prods[ci][2][1]/n - m[2]*m[1]; c[8] = prods[ci][2][2]/n - m[2]*m[2];

			double dtrm = c[0]*(c[4]*c[8]-c[5]*c[7]) - c[1]*(c[3]*c[8]-c[5]*c[6]) + c[2]*(c[3]*c[7]-c[4]*c[6]);
			if( dtrm <= std::numeric_limits<double>::epsilon() )
			{
				// Adds the white noise to avoid singular covariance matrix.
				c[0] += variance;
				c[4] += variance;
				c[8] += variance;
			}
			CalcInverseCovAndDeterm(ci);
		}
	}
}

int GMM::WhichComponent( const Vec3d color ) const
{
	int k = 0;
	double max = 0;

	for( int ci = 0; ci < componentsCount; ci++ )
	{
		double p = (*this)( ci, color );
		if( p > max )
		{
			k = ci;
			max = p;
		}
	}
	return k;
}

//
//double GMM::ProbBelongtoFgd( const Vec3d color ) const
//{
//		double fgdProb=0.0;
//		for(int ci=0;ci<componentsCount;ci++)
//		{
//			double p = (*this)( ci, color );
//			if( p > 0)
//			{
//				fgdProb+=p;
//			}
//			/*
//			if( coefs[ci] > 0 )
//			{
//				CV_Assert( covDeterms[ci] > std::numeric_limits<double>::epsilon() );
//				Vec3d diff = color;
//				double* m = mean + 3*ci;
//				diff[0] -= m[0]; diff[1] -= m[1]; diff[2] -= m[2];
//				double mult = diff[0]*(diff[0]*inverseCovs[ci][0][0] + diff[1]*inverseCovs[ci][1][0] + diff[2]*inverseCovs[ci][2][0])
//					+ diff[1]*(diff[0]*inverseCovs[ci][0][1] + diff[1]*inverseCovs[ci][1][1] + diff[2]*inverseCovs[ci][2][1])
//					+ diff[2]*(diff[0]*inverseCovs[ci][0][2] + diff[1]*inverseCovs[ci][1][2] + diff[2]*inverseCovs[ci][2][2]);
//				sepProb = 1.0f/sqrt(covDeterms[ci]) * exp(-0.5f*mult);
//			}
//		   */
//		}
//		  return fgdProb;
//
//}




//Don't know how to calculate and get the result.
double GMM::operator()( int ci, const Vec3d color ) const
{
	double res = 0;
	if( coefs[ci] > 0 )
	{
		CV_Assert( covDeterms[ci] > std::numeric_limits<double>::epsilon() );
		Vec3d diff = color;
		double* m = mean + 3*ci;
		diff[0] -= m[0]; diff[1] -= m[1]; diff[2] -= m[2];
		double mult = diff[0]*(diff[0]*inverseCovs[ci][0][0] + diff[1]*inverseCovs[ci][1][0] + diff[2]*inverseCovs[ci][2][0])
			+ diff[1]*(diff[0]*inverseCovs[ci][0][1] + diff[1]*inverseCovs[ci][1][1] + diff[2]*inverseCovs[ci][2][1])
			+ diff[2]*(diff[0]*inverseCovs[ci][0][2] + diff[1]*inverseCovs[ci][1][2] + diff[2]*inverseCovs[ci][2][2]);
		res = 1.0f/sqrt(covDeterms[ci]) * exp(-0.5f*mult);
	}
	return res;
}

//
double GMM::operator()( const Vec3d color ) const
{
	double res = 0;
	for( int ci = 0; ci < componentsCount; ci++ )
		res += coefs[ci] * (*this)(ci, color );     //�ǿ��Ǽ�Ȩ�ͣ�
	return res;
}




