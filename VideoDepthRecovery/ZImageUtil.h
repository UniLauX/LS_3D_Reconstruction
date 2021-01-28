#pragma once
#include "ZImage.h"
#include "ximage.h"
#include "wmlvector3.h"


template < class T > 
void ZImageToCxImage(ZImage<T>& zImg, CxImage& cxImg, float scale = 1.0F)
{
	int iWidth = zImg.GetWidth();
	int iHeight = zImg.GetHeight();	
	int iChannel = zImg.GetChannel();

	cxImg.Create(iWidth,iHeight,24);

	if(iChannel==4)
		cxImg.AlphaCreate();

	RGBQUAD color;
	for(int j=0;j<iHeight;++j)
		for(int i=0;i<iWidth;++i){
			if(iChannel>=3){
				color.rgbRed = max(0,min(255,zImg.at(i,j,0) * scale));
				color.rgbGreen = max(0,min(255,zImg.at(i,j,1) * scale));
				color.rgbBlue = max(0,min(255,zImg.at(i,j,2) * scale));
				if(iChannel == 4){
					float val = zImg.at(i,j,3);
					color.rgbReserved = min(255,val * scale);
				}
				else
					color.rgbReserved = 255;
			}
			else{
				color.rgbRed = color.rgbGreen = color.rgbBlue = max(0,min(255,zImg.at(i,j,0) * scale));
			}
			cxImg.SetPixelColor(i,iHeight-1-j,color,true);
		}
}

//template < class T > 
//void ZImageToCxImageTwoScale(ZImage<T>& zImg, CxImage& cxImg, float NormalScale, int offsetX, int offsetY, int blockWidth, int blockHeight, float RegionScale)
//{
//	int iWidth = zImg.GetWidth();
//	int iHeight = zImg.GetHeight();	
//	int iChannel = zImg.GetChannel();
//
//	cxImg.Create(iWidth,iHeight,24);
//
//	if(iChannel==4)
//		cxImg.AlphaCreate();
//
//	float scale;
//	RGBQUAD color;
//	for(int j=0;j<iHeight;++j)
//		for(int i=0;i<iWidth;++i){
//			if(i>=offsetX && i-offsetX<blockWidth && j>=offsetY && j-offsetY<blockHeight)
//				scale = RegionScale;
//			else
//				scale = NormalScale;
//
//			if(iChannel>=3){
//				color.rgbRed = max(0,min(255,zImg.at(i,j,0) * scale));
//				color.rgbGreen = max(0,min(255,zImg.at(i,j,1) * scale));
//				color.rgbBlue = max(0,min(255,zImg.at(i,j,2) * scale));
//				if(iChannel == 4){
//					float val = zImg.at(i,j,3);
//					color.rgbReserved = min(255,val * scale);
//				}
//				else
//					color.rgbReserved = 255;
//			}
//			else{
//				color.rgbRed = color.rgbGreen = color.rgbBlue = max(0,min(255,zImg.at(i,j,0) * scale));
//			}
//			cxImg.SetPixelColor(i,iHeight-1-j,color,true);
//		}
//}

template < class T > 
void CxImageToZImage(CxImage& cxImg, ZImage<T>& zImg, float scale = 1.0F)
{
	int iWidth = cxImg.GetWidth();
	int iHeight = cxImg.GetHeight();	

	int iChannel = zImg.GetChannel();

	for(int j=0;j<iHeight;++j)
		for(int i=0;i<iWidth;++i){
			RGBQUAD color = cxImg.GetPixelColor(i,iHeight-1-j,true);
			if(iChannel == 3){
				zImg.at(i,j,0) = (T)(scale * color.rgbRed);
				zImg.at(i,j,1) = (T)(scale * color.rgbGreen);
				zImg.at(i,j,2) = (T)(scale * color.rgbBlue);		
			}
			else
				//zImg.at(i, j, 0) = (T)(scale * cxImg.GetPixelGray(i, iHeight-1-j));
				
				zImg.at(i, j, 0) = (T)((color.rgbRed + color.rgbGreen + color.rgbBlue)/3.0*scale);
			//	zImg.at(i, j, 0) = (T)(((int)color.rgbRed + (int)color.rgbGreen + (int)color.rgbBlue)/3); //ORG
		}
}


template < class A, class B > 
void ZImageToZImage(ZImage<A>& srcImg, ZImage<B>& dstImg)
{
	int iWidth = srcImg.GetWidth();
	int iHeight = srcImg.GetHeight();	
	int iChannel = srcImg.GetChannel();

	dstImg.Create(iWidth,iHeight,iChannel);

	for(int j=0;j<iHeight;++j)
		for(int i=0;i<iWidth;++i){

			for(int c = 0; c<iChannel; ++c){
				dstImg.at(i,j,c) = srcImg.at(i,j,c);
			}

		}
}

template <class REAL>
inline double ZInterpolationAt(ZImage<REAL>& img, double x, double y)
{
	int iWidth = img.GetWidth();
	int iHeight = img.GetHeight();

	int ix0 = floor(x), ix1 = ceil(x);
	int iy0 = floor(y), iy1 = ceil(y);
	float dx = x-ix0, dy = y-iy0;

	float r00=img.at(ix0, iy0);
	float r01=img.at(ix0, iy1);
	float r10=img.at(ix1, iy0);
	float r11=img.at(ix1, iy1);

	return (r00 * (1.0-dx) + r10 * dx) * (1.0-dy) + (r01 * (1.0-dx) + r11 * dx) * dy;
}

template <class REAL>
inline void ZGetColor(ZImage<REAL>& img, float x, float y, REAL color[])
{
	int iWidth = img.GetWidth();
	int iHeight = img.GetHeight();
	int iC = img.GetChannel();

	int ix0 = floor(x), ix1 = ceil(x);
	int iy0 = floor(y), iy1 = ceil(y);
	float dx = x-ix0, dy = y-iy0;

	for(int c=0; c<iC; ++c){
		float r00=img.at(ix0,iy0,c);
		float r01=img.at(ix0,iy1,c);
		float r10=img.at(ix1,iy0,c);
		float r11=img.at(ix1,iy1,c);

		color[c] = (r00 * (1.0-dx) + r10 * dx) * (1.0-dy) + 
			(r01 * (1.0-dx) + r11 * dx) * dy;
	}
}

template<class T, class F>
inline void InterpolateColor(ZImage<T>& img, float x, float y, Wml::Vector3<F>& color)
{
	int ix0 = floor(x), ix1 = ceil(x);
	int iy0 = floor(y), iy1 = ceil(y);
	float dx = x-ix0, dy = y-iy0;

	T color00[3], color01[3], color10[3], color11[3];

	memcpy(color00,img.PixelAddress(ix0,iy0), sizeof(T)*3);
	memcpy(color01,img.PixelAddress(ix0,iy1), sizeof(T)*3);
	memcpy(color10,img.PixelAddress(ix1,iy0), sizeof(T)*3);
	memcpy(color11,img.PixelAddress(ix1,iy1), sizeof(T)*3);


	color[0] = (color00[0] * (1.0-dx) + color10[0] * dx) * (1.0-dy) + 
		(color01[0] * (1.0-dx) + color11[0] * dx) * dy;
	color[1] = (color00[1] * (1.0-dx) + color10[1] * dx) * (1.0-dy) + 
		(color01[1] * (1.0-dx) + color11[1] * dx) * dy;
	color[2] = (color00[2] * (1.0-dx) + color10[2] * dx) * (1.0-dy) + 
		(color01[2] * (1.0-dx) + color11[2] * dx) * dy;

}
