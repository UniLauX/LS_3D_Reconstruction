// ZImage.h: interface for the ZImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZIMAGE_H__DC54134A_615B_45C9_A501_86FA5F16FCC3__INCLUDED_)
#define AFX_ZIMAGE_H__DC54134A_615B_45C9_A501_86FA5F16FCC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <new>

template <class T>
class ZImage  
{
public:
	ZImage()
		:m_map(0),m_iWidth(0),m_iHeight(0)
	{
	};
	ZImage(int width,int height,int channel=1)
	{
		m_iChannel = channel;
		m_iWidth = width;
		m_iHeight = height;
		m_iEffectWidth = (channel*width*sizeof(T)+3)/4*4; //保证行是4Byte的整数倍
		m_iImageSize = m_iEffectWidth * m_iHeight;
		m_map = new char[m_iImageSize];
	}	
	ZImage(const ZImage& other)
		:m_map(NULL),m_iWidth(0),m_iHeight(0),m_iChannel(0)
	{
		Create(other.m_iWidth,other.m_iHeight,other.m_iChannel);
		memcpy(m_map,other.m_map,m_iImageSize);
	}

	virtual ~ZImage()
	{
		delete [] m_map;
		m_map = 0;
	}

	ZImage& operator = (const ZImage& other)		
	{
		Create(other.m_iWidth,other.m_iHeight,other.m_iChannel);
		memcpy(m_map,other.m_map,m_iImageSize);

		return *this;
	}

	void Create(int width,int height,int channel=1)
	{
		if(m_iWidth != width || m_iHeight != height || m_iChannel != channel){			
			delete [] m_map;
			m_iChannel = channel;
			m_iWidth = width;
			m_iHeight = height;
			m_iEffectWidth = (channel*width*sizeof(T)+3)/4*4; //保证行是4Byte的整数倍
			m_iImageSize = m_iEffectWidth * m_iHeight;
			m_map = new char[m_iImageSize];
		}
	}

	void Create(int width,int height,int channel,void* iSrc)
	{
		Create(width,height,channel);
		if(iSrc != m_map)
			memcpy(m_map,iSrc,m_iImageSize);
	}

	void CreateAndInit(int width,int height,int channel=1, T v=T(0))
	{
		delete [] m_map;
		m_iChannel = channel;
		m_iWidth = width;
		m_iHeight = height;
		m_iEffectWidth = (channel*width*sizeof(T)+3)/4*4; //保证行是4Byte的整数倍
		m_iImageSize = m_iEffectWidth * m_iHeight;
		m_map = new char[m_iImageSize];

		Init(v);
	}
	void Init(T v = T(0)){
		int i,j;
		T* p;
		char* rowStart = m_map;
		int iRowSize = m_iWidth*m_iChannel;
		for(j=0;j<m_iHeight;j++){
			for(i=0;i<iRowSize;i++){				
				p = &((T*)rowStart)[i];
				new (p) T(v);
				//*p = v;
			}
			rowStart += m_iEffectWidth;
		}
	}

	void Clear()
	{
		delete [] m_map;
		m_map = 0;
		m_iChannel = 0;
		m_iWidth = m_iHeight = 0;
		m_iEffectWidth = 0;
		m_iImageSize = 0;		
	}

	void MakeZero()
	{
		int i,j;
		T* p;
		char* rowStart = m_map;
		int iRowSize = m_iWidth*m_iChannel;
		for(j=0;j<m_iHeight;j++){			
			for(i=0;i<iRowSize;i++){
				p = &((T*)rowStart)[i];
				//::new (p) T(0);
				*p = T(0);
			}
			rowStart += m_iEffectWidth;
		}
	}

	void* PixelAddress(int x, int y, int band=0) 
	{
		return (void *) &m_map[ y * m_iEffectWidth + (x * m_iChannel + band)* sizeof(T)];
	}
	const void* PixelAddress(int x, int y, int band=0) const
	{
		return (void *) &m_map[ y * m_iEffectWidth + (x * m_iChannel + band)* sizeof(T)];
	}

	T& at( int x, int y, int band=0 )
	{
		return *(T *) PixelAddress(x, y, band);
	}

	const T& at( int x, int y, int band=0 ) const
	{
		return *(T *) PixelAddress(x, y, band);
	}

	T GetPixel( int x, int y, int band = 0 )
	{
		return *(T *) PixelAddress(x, y, band);
	}
	
	void SetPixel(int x, int y, int band, const T& v )
	{
		at(x,y,band) = v;
	}

	T& Pixel(int x, int y, int band)
	{
		return *(T *) PixelAddress(x, y, band);
	}

	int GetChannel() const
	{
		return m_iChannel;
	}

	int GetWidth() const
	{
		return m_iWidth;
	}

	int GetHeight() const
	{
		return m_iHeight;
	}

	int GetEffectWidth()
	{
		return m_iEffectWidth;
	}

	int GetSize()
	{
		return m_iImageSize;
	}

	char* GetMap()
	{
		return m_map;
	}
private:
	char*	m_map;
	const T* m_constmap;
	int m_iChannel;
	int m_iWidth, m_iHeight;
	int m_iEffectWidth;
	int m_iImageSize;
public:
	typedef T value_type;
};


template <class T>
class ZVector  
{
public:
	ZVector()
		:m_map(NULL),m_iSize(0)
	{
	};
	ZVector(int iSize,int channel=1):m_map(NULL),m_iSize(0),m_iChannel(0)
	{
		Create(iSize,channel);
	}	
	ZVector(const ZVector& other)
		:m_iSize(0),m_iChannel(0),m_map(0)
	{
		Create(other.m_iSize,other.m_iChannel);
		memcpy(m_map,other.m_map,GetMapSize());	
	}

	virtual ~ZVector()
	{
		delete [] m_map;
	}

	void Clear()
	{
		delete [] m_map;
		m_map = NULL;		
		m_iChannel = 0;
		m_iSize = 0;
	}

	ZVector& operator = (const ZVector& other)
	{
		Create(other.m_iSize,other.m_iChannel);
		memcpy(m_map,other.m_map,GetMapSize());

		return *this;
	}

	void Create(int iSize,int channel=1)
	{
		if(m_iSize != iSize || m_iChannel != channel){
			delete [] m_map;
			m_iChannel = channel;
			m_iSize = iSize;			
			m_map = new char[GetMapSize()];
		}
	}

	void Create(int iSize,int channel,void* iSrc)
	{
		Create(iSize,channel);
		memcpy(m_map,iSrc,GetMapSize());
	}

	void CreateAndInit(int iSize,int channel=1)
	{
		delete [] m_map;
		m_iChannel = channel;
		m_iSize = iSize;
			
		m_map = new char[GetMapSize()];

		MakeZero();
	}

	void MakeZero()
	{
		int i;
		T* p;
		char* rowStart = m_map;
				
		int iRowSize = m_iSize*m_iChannel;
		for(i=0;i<iRowSize;i++){
			p = &((T*)rowStart)[i];				
			::new (p) T(0);
		}
	}

	void* PixelAddress(int x, int band=0)
	{
		return (void *) &m_map[(x * m_iChannel + band)* sizeof(T)];
	}

	T& at( int x, int band=0 )
	{
		return *(T *) PixelAddress(x, band);
	}

	T GetPixel( int x, int band = 0 )
	{
		return *(T *) PixelAddress(x, band);
	}

	void SetPixel(int x, int band, const T& v )
	{
		at(x,band) = v;
	}

	int GetChannel()
	{
		return m_iChannel;
	}

	int GetSize()
	{
		return m_iSize;
	}

	int GetMapSize()
	{
		return m_iSize * m_iChannel* sizeof(T);
	}

	char* GetMap()
	{
		return m_map;
	}
private:
	char*	m_map;
	const T* m_constmap;
	int m_iChannel;
	int m_iSize;
public:
	typedef T value_type;
};


typedef ZImage<unsigned char>	ZByteImage;
typedef ZImage<int>	ZIntImage;
typedef ZImage<float>	ZFloatImage;
typedef ZImage<double>	ZDoubleImage;
typedef ZImage<bool>	ZBoolImage;

typedef ZVector<unsigned char>	ZByteVector;
typedef ZVector<int>	ZIntVector;
typedef ZVector<float>	ZFloatVector;
typedef ZVector<double>	ZDoubleVector;
typedef ZVector<bool>	ZBoolVector;

#endif // !defined(AFX_ZIMAGE_H__DC54134A_615B_45C9_A501_86FA5F16FCC3__INCLUDED_)

