#include "NCCCommonUse.h"
#include <math.h>
#include <assert.h>



namespace NCCCommonUse
{

	float vecDot(std::vector<float>& vec0,std::vector<float>& vec1)
	{
		float sum = 0.0f;
		assert( vec0.size() == vec1.size() );	
		for(int i = 0;i<(int)vec0.size();i++)
			sum+=vec0[i] * vec1[i];
		return sum;
	}


void normalizeRGBTex(std::vector<float>& texRegion,std::vector<float>& ans)
{
	const int size = (int)texRegion.size();
	const int size_3 = size / 3;
	float avg[3];
	avg[0] = avg[1] = avg[2] = 0.0f;

	for(int pixels = 0;pixels<size_3;pixels++)
	{
		int offset = pixels*3;
		avg[0] +=texRegion[offset];
		avg[1] +=texRegion[offset + 1];
		avg[2] +=texRegion[offset + 2];
	}
	avg[0] /= size_3;avg[1] /= size_3;avg[2] /= size_3;


	float sqrtAvg = 0.0f;

	for(int pixels = 0;pixels<size_3;pixels++)
	{
		int offset = pixels * 3;
		const float dif0 = texRegion[offset] - avg[0];
		ans[offset] = dif0;
		const float dif1 = texRegion[offset + 1] - avg[1];
		ans[offset +1] = dif1;
		const float dif2 = texRegion[offset + 2] - avg[2];
		ans[offset + 2] = dif2;
		sqrtAvg += ( dif0 * dif0 + dif1 * dif1 + dif2 * dif2 ) ;
	}


	sqrtAvg = sqrt( sqrtAvg );
	if( sqrtAvg<1e-2f )  
	{	
		sqrtAvg = 1.0f;
		for(int pixels = 0;pixels<size_3;pixels++)
		{
			int offset = pixels * 3;
			ans[offset] = 0.0f;
			ans[offset +1] = 0.0f;
			ans[offset + 2] = 0.0f;
		}
		int rnd = rand() % size;
		ans[rnd] = 1.0f;
		return ;
	}

	float invSqrtAvg = 1.0f / sqrtAvg;

	for(int pixels = 0;pixels<size;pixels++)
		ans[ pixels ] *= invSqrtAvg;

#ifdef _DEBUG
	float total  = 0.0f;
	for(int pixels = 0;pixels<size;pixels++)
		total +=texRegion[ pixels ]* texRegion[ pixels ] ;
	assert(total>0.99f);
#endif

}

}//end namespace