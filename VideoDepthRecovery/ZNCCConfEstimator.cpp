#include "ZNCCConfEstimator.h"


ZNCCConfEstimator::ZNCCConfEstimator(LSVideoFrame* refFrame,std::vector<LSVideoFrame* >& cmpFrameSet, 
	ZByteImage* refImage,std::vector<ZByteImage* >& cmpImageSet )
	:mRefFrame( refFrame ),mCmpFrameSet( cmpFrameSet ),
	mRefImage( refImage ),mCmpImageSet( cmpImageSet )
{

}


ZNCCConfEstimator::~ZNCCConfEstimator(void)
{
}

//return value ( -1.0  , 1.0)
float ZNCCConfEstimator::estPixelConf(int x,int y,int halfWinSize)
{
	
	const int width = mRefFrame->GetImgWidth();
	const int height = mRefFrame->GetImgHeight();

	if( x<halfWinSize || y<halfWinSize || 
		x + halfWinSize >=width || y + halfWinSize >=height )
		return 0.0f;


	//ZFloatImage&  refDepth = mRefFrame->m_DepthImg;
	 ZFloatImage&  refDsp = *mRefFrame->m_pDspImg;

	int totalPts = (halfWinSize * 2 + 1) *(halfWinSize * 2 + 1);
	std::vector<Wml::Vector3d>  ptSet( totalPts );
	std::vector<float >  refTex(totalPts * 3);
	int idx = 0;

	for(int yOffset = -halfWinSize;yOffset<=halfWinSize;++yOffset)
		for(int xOffset = -halfWinSize;xOffset<=halfWinSize;++xOffset)
		{
			int  realX = x + xOffset;
			int realY = y + yOffset;

		//	float invZ = refDepth.at( realX,realY );
		//	ptSet[ idx ] = mRefFrame->ProjUVZtoWorldXYZf( realX,realY,1.0 / invZ );
			float invZ = refDsp.at( realX,realY );
			
			//ptSet[ idx ] = mRefFrame->ProjUVZtoWorldXYZf( realX,realY,1.0 / invZ );
		    ptSet[idx]=WorldCoordFrmImgCoord(mRefFrame,Wml::Vector2d(realX,realY),1.0/invZ);
            
			//////////////////////////////////////////////////////////////////////////

			refTex[ idx * 3 ] = mRefImage->at( realX,realY,0 );
			refTex[ idx * 3 + 1 ] = mRefImage->at( realX,realY,1 );
			refTex[ idx * 3  + 2] = mRefImage->at( realX,realY,2 );
			idx++;
		}

		NCCCommonUse::normalizeRGBTex( refTex,refTex );

		std::vector<float > cmpTex( totalPts*3 );
		std::vector<float >  cadidate;
		for(int iFrame = 0;iFrame < mCmpFrameSet.size();iFrame++ )  //此处iFrame仅作为参考帧下标之用，切勿错误理解
		{
			LSVideoFrame* cmpFrame = mCmpFrameSet[ iFrame ];
			ZByteImage* cmpImage = mCmpImageSet[ iFrame ];

			bool allInImage = true;
			//Wml::Vector3d  projUVZ;
			double u,v,dsp;
			idx = 0;
			for(int iPt = 0;iPt<totalPts;++iPt)
			{
				Wml::Vector3d& pt = ptSet[ iPt ];

				//cmpFrame->ProjXYZtoUVZf( pt,projUVZ );

				cmpFrame->GetImgCoordFrmWorldCoord(u,v,dsp,pt);
				Wml::Vector3d  projUVZ(u,v,1.0);

				float projX = projUVZ.X();
				float projY = projUVZ.Y();
				//allInImage = cmpFrame->InImg( projX,projY );
			    allInImage=(projX>=0&&projX<width&&projY>=0&&projY<height);
				if( !allInImage )
					break;

				cmpTex[ idx * 3 ] = cmpImage->at( projX,projY,0 );
				cmpTex[ idx * 3 + 1 ] = cmpImage->at( projX,projY,1 );
				cmpTex[ idx * 3  + 2] = cmpImage->at( projX,projY,2 );
				idx++;
			}

		
			float nccVal = 0.0f;
			if( allInImage )
			{
				NCCCommonUse::normalizeRGBTex( cmpTex,cmpTex );
				nccVal = NCCCommonUse::vecDot( refTex,cmpTex );
			}
			cadidate.push_back( nccVal );

		}
		//////////////////////////////////////////////////////////////////////////
	
		int halfNeigh = cadidate.size() / 2;
		float sum = 0.0f;
		halfNeigh = std::min<int>( std::max<int>(halfNeigh,1),cadidate.size() );
		if( halfNeigh<cadidate.size() )
			std::nth_element( cadidate.begin(),cadidate.begin() + halfNeigh,cadidate.end(),std::greater<float >() );

		for(int i = 0;i<halfNeigh;++i)
			sum += cadidate[ i ];
	 // std::cout<<"sum:"<<sum<<std::endl;  //查程序bug时用到
		if(halfNeigh)
			return sum / halfNeigh;
		else return 0.0f;
	 
}




//LY NEWADD Function
Wml::Vector3d ZNCCConfEstimator::WorldCoordFrmImgCoord(LSVideoFrame* pVFrm, Wml::Vector2d p, double z)
{
	Wml::Vector3d P;

	pVFrm->GetWorldCoordFrmImgCoord(p.X(), p.Y(), 1.0/z, P);
	return P;
}