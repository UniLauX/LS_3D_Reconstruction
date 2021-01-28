
#include <math.h>
#include <algorithm>
#include <iostream>
#include "LSVideoFrame.h"
#include "SoftRasterize.h"

//NEW-ADD
SoftRasterize::SoftRasterize(float* minDepth,float* maxDepth,int width,int height,LSVideoFrame* camMat)
	:mMinDepth(minDepth),mMaxDepth(maxDepth),mWidth(width),mHeight(height),
	m_pFrame(camMat)
{

}



SoftRasterize::SoftRasterize(LSVideoFrame* pFrame){
	m_pFrame = pFrame;
}

SoftRasterize::~SoftRasterize(){
}

void SoftRasterize::start(std::vector<IdxTraingle> &mTriangleList, std::vector<Wml::Vector3f>& mVertexList)
{
	for(int iTri = 0;iTri<(int)mTriangleList.size();iTri++ ){
		 VertexID*  triangleIdx = mTriangleList[iTri].mVertexID;

		 Wml::Vector3f& D3V0 = mVertexList[ triangleIdx[0] ];
		 Wml::Vector3f& D3V1 = mVertexList[ triangleIdx[1] ];
		 Wml::Vector3f& D3V2 = mVertexList[ triangleIdx[2] ];

		 Wml::Vector3d doubleD3V0 = Wml::Vector3d( D3V0[0],D3V0[1],D3V0[2] );
		 Wml::Vector3d doubleD3V1 = Wml::Vector3d( D3V1[0],D3V1[1],D3V1[2] );
		 Wml::Vector3d doubleD3V2 = Wml::Vector3d( D3V2[0],D3V2[1],D3V2[2] );

		 Wml::Vector3d  doubleD2ZV0;
		 m_pFrame->GetImgCoordFrmWorldCoord(doubleD2ZV0[0], doubleD2ZV0[1], doubleD2ZV0[2], doubleD3V0);
		 doubleD2ZV0[2] = 1.0/doubleD2ZV0[2];
		 Wml::Vector3f  D2ZV0( (float)doubleD2ZV0[0],(float) doubleD2ZV0[1],(float) doubleD2ZV0[2] );

		 
		 Wml::Vector3d  doubleD2ZV1;
		 //mCamMat->ProjXYZtoUVZ( doubleD3V1,doubleD2ZV1 );
		 m_pFrame->GetImgCoordFrmWorldCoord(doubleD2ZV1[0], doubleD2ZV1[1], doubleD2ZV1[2], doubleD3V1);
		 doubleD2ZV1[2] = 1.0/doubleD2ZV1[2];
		 Wml::Vector3f  D2ZV1( (float)doubleD2ZV1[0],(float) doubleD2ZV1[1],(float) doubleD2ZV1[2] );

		 
		 Wml::Vector3d  doubleD2ZV2;
		 //mCamMat->ProjXYZtoUVZ( doubleD3V2,doubleD2ZV2 );   //假设相机参数转换过程正确
		 m_pFrame->GetImgCoordFrmWorldCoord(doubleD2ZV2[0], doubleD2ZV2[1], doubleD2ZV2[2], doubleD3V2);
		 doubleD2ZV2[2] = 1.0/doubleD2ZV2[2];
		 Wml::Vector3f  D2ZV2( (float)doubleD2ZV2[0],(float) doubleD2ZV2[1],(float) doubleD2ZV2[2] );
		 
		// _triangleRasterize(&D2ZV0, &D2ZV1, &D2ZV2); //对应disparity
		 _triangleRasterize2(&D2ZV0,&D2ZV1,&D2ZV2);   //对应depth
	}
	
}

//NEW-ADD
void SoftRasterize::_triangleRasterize(Wml::Vector3f* V0,Wml::Vector3f* V1,Wml::Vector3f* V2)
{
	//////////////////////////////////////////////////////////////////////////
	Wml::Vector3f  edge01 = *V1 - *V0;
	Wml::Vector3f  edge02 = *V2 - *V0;
	Wml::Vector3f normal = edge01.Cross( edge02 );
	normal.Normalize();
	if(normal.Z()>1e-8f)
	{
		normal = -normal;
		std::swap(V1,V2);
	}
	else		if(normal.Z()>-1e-8f)  //fabs( normal.Z() ) <1-6f ;
		return;
	float D = -normal.Dot( *V0 );

	const float InvNormalZ = 1.0f / normal.Z(); 
	//////////////////////////////////////////////////////////////////////////

	// 28.4 fixed-point coordinates
	const int Y1 = (int)(16.0f * V0->Y() );
	const int Y2 = (int)(16.0f * V1->Y() );
	const int Y3 = (int)(16.0f * V2->Y() );

	const int X1 = (int)(16.0f * V0->X() );
	const int X2 = (int)(16.0f * V1->X() );
	const int X3 = (int)(16.0f * V2->X() );

	// Deltas
	const int DX12 = X1 - X2;
	const int DX23 = X2 - X3;
	const int DX31 = X3 - X1;

	const int DY12 = Y1 - Y2;
	const int DY23 = Y2 - Y3;
	const int DY31 = Y3 - Y1;

	// Fixed-point deltas
	const int FDX12 = DX12 << 4;
	const int FDX23 = DX23 << 4;
	const int FDX31 = DX31 << 4;

	const int FDY12 = DY12 << 4;
	const int FDY23 = DY23 << 4;
	const int FDY31 = DY31 << 4;

	// Bounding rectangle
	int minx  = ( std::min( std::min  (X1, X2), X3 ) + 0xF) >> 4;
	int maxx = ( std::max( std::max (X1, X2), X3 ) + 0xF) >> 4;
	int miny  = (std::min( std::min  (Y1, Y2), Y3) + 0xF) >> 4;
	int maxy = (std::max( std::max  (Y1, Y2), Y3) + 0xF) >> 4;

	// Half-edge constants
	int C1 = DY12 * X1 - DX12 * Y1;
	int C2 = DY23 * X2 - DX23 * Y2;
	int C3 = DY31 * X3 - DX31 * Y3;

	// Correct for fill convention
	if(DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++;
	if(DY23 < 0 || (DY23 == 0 && DX23 > 0)) C2++;
	if(DY31 < 0 || (DY31 == 0 && DX31 > 0)) C3++;

	int CY1 = C1 + DX12 * (miny << 4) - DY12 * (minx << 4);
	int CY2 = C2 + DX23 * (miny << 4) - DY23 * (minx << 4);
	int CY3 = C3 + DX31 * (miny << 4) - DY31 * (minx << 4);

	//clip
	if( miny>=mHeight )  return;
	if( maxy<0 ) return;
	if(miny<0)
	{
		CY1 += FDX12 * (-miny);
		CY2 += FDX23 * (-miny);
		CY3 += FDX31 * (-miny);
		miny = 0;
	}
	const int clipMinX = std::max( minx,0 );
	if( clipMinX>=mWidth)  return;
	if( maxx<0 ) return;


	for(int y = miny; y < maxy; y++)
	{
		//clip 
		if(y>=mHeight)			return;

		int CX1 = CY1;
		int CX2 = CY2;
		int CX3 = CY3;

		//clip
		if(minx<0)
		{
			CX1 += FDY12 * (minx);
			CX2 += FDY23 * (minx);
			CX3 += FDY31 * (minx);
		}

		float CZ = -( normal[0 ]* (float)clipMinX + normal[1] * (float)y + D );

		for(int x = clipMinX; x < maxx; x++)
		{
			//clip
			if( x>=mWidth )  break;

			if(CX1 > 0 && CX2 > 0 && CX3 > 0)
			{
				float viewZ = CZ * InvNormalZ;
				int idx = x + y * mWidth; 
				if( mMinDepth[ idx ] >viewZ )
					mMinDepth[ idx ] = viewZ;
				if( mMaxDepth[ idx ] <viewZ )
					mMaxDepth[ idx ] = viewZ;
			}

			CX1 -= FDY12;
			CX2 -= FDY23;
			CX3 -= FDY31;
			CZ -=normal[0];
		}

		CY1 += FDX12;
		CY2 += FDX23;
		CY3 += FDX31;
	}
}




/* ORG
//LQL代码，用disparity设置
void SoftRasterize::_triangleRasterize(Wml::Vector3f* V0,Wml::Vector3f* V1,Wml::Vector3f* V2){
  
	//////////////////////////////////////////////////////////////////////////
	Wml::Vector3f  edge01 = *V1 - *V0;
	Wml::Vector3f  edge02 = *V2 - *V0;
	Wml::Vector3f normal = edge01.Cross( edge02 );
	normal.Normalize();
	if(normal.Z()>1e-8f){
		normal = -normal;
		std::swap(V1,V2);
	}
	else if(normal.Z()>-1e-8f)  //fabs( normal.Z() ) <1-6f ;
		return;
	float D = -normal.Dot( *V0 );

	const float InvNormalZ = 1.0f / normal.Z(); 
	//////////////////////////////////////////////////////////////////////////

	// 28.4 fixed-point coordinates
	const int Y1 = (int)(16.0f * V0->Y() );
	const int Y2 = (int)(16.0f * V1->Y() );
	const int Y3 = (int)(16.0f * V2->Y() );

	const int X1 = (int)(16.0f * V0->X() );
	const int X2 = (int)(16.0f * V1->X() );
	const int X3 = (int)(16.0f * V2->X() );

	// Deltas
	const int DX12 = X1 - X2;
	const int DX23 = X2 - X3;
	const int DX31 = X3 - X1;

	const int DY12 = Y1 - Y2;
	const int DY23 = Y2 - Y3;
	const int DY31 = Y3 - Y1;

	// Fixed-point deltas
	const int FDX12 = DX12 << 4;
	const int FDX23 = DX23 << 4;
	const int FDX31 = DX31 << 4;

	const int FDY12 = DY12 << 4;
	const int FDY23 = DY23 << 4;
	const int FDY31 = DY31 << 4;

	// Bounding rectangle
	int minx  = ( min(  min  (X1, X2), X3 ) + 0xF) >> 4;
	int maxx = ( max(  max (X1, X2), X3 ) + 0xF) >> 4;
	int miny  = ( min(  min  (Y1, Y2), Y3) + 0xF) >> 4;
	int maxy = ( max( max  (Y1, Y2), Y3) + 0xF) >> 4;

	// Half-edge constants
	int C1 = DY12 * X1 - DX12 * Y1;
	int C2 = DY23 * X2 - DX23 * Y2;
	int C3 = DY31 * X3 - DX31 * Y3;

	// Correct for fill convention
	if(DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++;
	if(DY23 < 0 || (DY23 == 0 && DX23 > 0)) C2++;
	if(DY31 < 0 || (DY31 == 0 && DX31 > 0)) C3++;

	int CY1 = C1 + DX12 * (miny << 4) - DY12 * (minx << 4);
	int CY2 = C2 + DX23 * (miny << 4) - DY23 * (minx << 4);
	int CY3 = C3 + DX31 * (miny << 4) - DY31 * (minx << 4);

	//clip
	int height = m_pFrame->GetImgHeight();
	int width = m_pFrame->GetImgWidth();

	if( miny >= height )  
		return;
	if( maxy < 0 ) 
		return;
	if( miny < 0 ){
		CY1 += FDX12 * (-miny);
		CY2 += FDX23 * (-miny);
		CY3 += FDX31 * (-miny);
		miny = 0;
	}

	const int clipMinX = max( minx,0 );
	if( clipMinX >= width)  
		return;
	if( maxx<0 ) 
		return;

	for(int y = miny; y < maxy; y++){
		//clip 
		if(y >= height)
			return;

		int CX1 = CY1;
		int CX2 = CY2;
		int CX3 = CY3;

		//clip
		if(minx<0){
			CX1 += FDY12 * (minx);
			CX2 += FDY23 * (minx);
			CX3 += FDY31 * (minx);
		}


		float CZ = -( normal[0 ]* (float)clipMinX + normal[1] * (float)y + D );

		for(int x = clipMinX; x < maxx; x++){
			//clip
			if( x >= width )  
				break;

			if(CX1 > 0 && CX2 > 0 && CX3 > 0){
				float viewZ = CZ * InvNormalZ;
				float viewDsp = 1.0F / viewZ;
				if( m_pFrame->m_pMinDspImg->GetPixel(x, y, 0) > viewDsp )
					m_pFrame->m_pMinDspImg->SetPixel(x, y, 0, viewDsp);
				if( m_pFrame->m_pMaxDspImg->GetPixel(x, y, 0) < viewDsp )
					m_pFrame->m_pMaxDspImg->SetPixel(x, y, 0, viewDsp);
			}

			CX1 -= FDY12;
			CX2 -= FDY23;
			CX3 -= FDY31;
			CZ -= normal[0];
		}
		CY1 += FDX12;
		CY2 += FDX23;
		CY3 += FDX31;
	}


}
*/


//和KXL代码相同，用Depth设置
void SoftRasterize::_triangleRasterize2(Wml::Vector3f* V0,Wml::Vector3f* V1,Wml::Vector3f* V2){

	//////////////////////////////////////////////////////////////////////////
	Wml::Vector3f  edge01 = *V1 - *V0;
	Wml::Vector3f  edge02 = *V2 - *V0;
	Wml::Vector3f normal = edge01.Cross( edge02 );
	normal.Normalize();
	if(normal.Z()>1e-8f){
		normal = -normal;
		std::swap(V1,V2);
	}
	else if(normal.Z()>-1e-8f)  //fabs( normal.Z() ) <1-6f ;
		return;
	float D = -normal.Dot( *V0 );

	const float InvNormalZ = 1.0f / normal.Z(); 
	//////////////////////////////////////////////////////////////////////////

	// 28.4 fixed-point coordinates
	const int Y1 = (int)(16.0f * V0->Y() );
	const int Y2 = (int)(16.0f * V1->Y() );
	const int Y3 = (int)(16.0f * V2->Y() );

	const int X1 = (int)(16.0f * V0->X() );
	const int X2 = (int)(16.0f * V1->X() );
	const int X3 = (int)(16.0f * V2->X() );

	// Deltas
	const int DX12 = X1 - X2;
	const int DX23 = X2 - X3;
	const int DX31 = X3 - X1;

	const int DY12 = Y1 - Y2;
	const int DY23 = Y2 - Y3;
	const int DY31 = Y3 - Y1;

	// Fixed-point deltas
	const int FDX12 = DX12 << 4;
	const int FDX23 = DX23 << 4;
	const int FDX31 = DX31 << 4;

	const int FDY12 = DY12 << 4;
	const int FDY23 = DY23 << 4;
	const int FDY31 = DY31 << 4;

	// Bounding rectangle
	int minx  = ( std::min( std::min  (X1, X2), X3 ) + 0xF) >> 4;
	int maxx = ( std::max(  std::max (X1, X2), X3 ) + 0xF) >> 4;
	int miny  = ( std::min(std::min  (Y1, Y2), Y3) + 0xF) >> 4;
	int maxy = ( std::max( std::max  (Y1, Y2), Y3) + 0xF) >> 4;

	// Half-edge constants
	int C1 = DY12 * X1 - DX12 * Y1;
	int C2 = DY23 * X2 - DX23 * Y2;
	int C3 = DY31 * X3 - DX31 * Y3;

	// Correct for fill convention
	if(DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++;
	if(DY23 < 0 || (DY23 == 0 && DX23 > 0)) C2++;
	if(DY31 < 0 || (DY31 == 0 && DX31 > 0)) C3++;

	int CY1 = C1 + DX12 * (miny << 4) - DY12 * (minx << 4);
	int CY2 = C2 + DX23 * (miny << 4) - DY23 * (minx << 4);
	int CY3 = C3 + DX31 * (miny << 4) - DY31 * (minx << 4);

	//clip
	int height = m_pFrame->GetImgHeight();
	int width = m_pFrame->GetImgWidth();

	if( miny >= height )  
		return;
	if( maxy < 0 ) 
		return;
	if( miny < 0 ){
		CY1 += FDX12 * (-miny);
		CY2 += FDX23 * (-miny);
		CY3 += FDX31 * (-miny);
		miny = 0;
	}

	const int clipMinX = std::max( minx,0 );
	if( clipMinX >= width)  
		return;
	if( maxx<0 ) 
		return;

	for(int y = miny; y < maxy; y++){
		//clip 
		if(y >= height)
			return;

		int CX1 = CY1;
		int CX2 = CY2;
		int CX3 = CY3;

		//clip
		if(minx<0){
			CX1 += FDY12 * (minx);
			CX2 += FDY23 * (minx);
			CX3 += FDY31 * (minx);
		}


		float CZ = -( normal[0 ]* (float)clipMinX + normal[1] * (float)y + D );

		for(int x = clipMinX; x < maxx; x++){
			//clip
			if( x >= width )  
				break;

			if(CX1 > 0 && CX2 > 0 && CX3 > 0){
				float viewZ = CZ * InvNormalZ;
				//float viewDsp = 1.0F / viewZ;
				if( m_pFrame->m_pDepthMin->GetPixel(x, y, 0) > viewZ )
					m_pFrame->m_pDepthMin->SetPixel(x, y, 0, viewZ);
				if( m_pFrame->m_pDepthMax->GetPixel(x, y, 0) < viewZ )
					m_pFrame->m_pDepthMax->SetPixel(x, y, 0, viewZ);
			}

			CX1 -= FDY12;
			CX2 -= FDY23;
			CX3 -= FDY31;
			CZ -= normal[0];
		}
		CY1 += FDX12;
		CY2 += FDX23;
		CY3 += FDX31;
	}


}