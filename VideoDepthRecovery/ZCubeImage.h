#pragma once

#include <vector>
template<class T>
class ZCubeImage
{
public:
	ZCubeImage()
		:m_map(NULL),m_iWidth(0),m_iHeight(0)
	{
	};
	ZCubeImage(int width,int height,int channel=1)
	{
		m_iChannel = channel;
		m_iWidth = width;
		m_iHeight = height;
		m_iEffectWidth = (channel*width*sizeof(T)+3)/4*4; //保证行是4Byte的整数倍
		m_iImageSize = m_iEffectWidth * m_iHeight;

		m_map.resize(m_iHeight);
		for(int i=0; i<m_iHeight; ++i){
			m_map[i] = new char[m_iEffectWidth];
		}
	}	
	ZCubeImage(const ZCubeImage& other)
		:m_map(NULL),m_iWidth(0),m_iHeight(0),m_iChannel(0)
	{
		Create(other.m_iWidth,other.m_iHeight,other.m_iChannel);
		memcpy(m_map,other.m_map,m_iImageSize);
	}

	virtual ~ZCubeImage()
	{
		Clear();
	}

	ZCubeImage& operator = (const ZCubeImage& other)		
	{
		Create(other.m_iWidth,other.m_iHeight,other.m_iChannel);

		for(int i=0; i<other->GetHeight(); ++i)
			memcpy(m_map[i],other.m_map[i],other.m_iEffectWidth);

		return *this;
	}

	void Create(int width,int height,int channel=1)
	{
		if(m_iWidth != width || m_iHeight != height || m_iChannel != channel){			
			Clear();

			m_iChannel = channel;
			m_iWidth = width;
			m_iHeight = height;
			m_iEffectWidth = (channel*width*sizeof(T)+3)/4*4; //保证行是4Byte的整数倍
			m_iImageSize = m_iEffectWidth * m_iHeight;
			
			m_map.resize(m_iHeight);
			for(int i=0; i<m_iHeight; ++i){
				m_map[i] = new char[m_iEffectWidth];
			}
		}
	}

	void CreateAndInit(int width,int height,int channel=1,const T v=T(0))
	{
		Create(width,height,channel);

		int i,j;
		T* p;
		char* rowStart = 0;
		int iRowSize = m_iWidth*m_iChannel;
		for(j=0;j<m_iHeight;j++){
			rowStart = m_map[j];

			for(i=0;i<iRowSize;i++){				
				p = &((T*)rowStart)[i];
				::new (p) T(v);
				//*p = v;
			}			
		}
	}

	void Clear()
	{
		for(int i=0; i<m_map.size(); ++i){
			delete[] m_map[i];
		}
		m_map.clear();
		m_iChannel = 0;
		m_iWidth = m_iHeight = 0;
		m_iEffectWidth = 0;
		m_iImageSize = 0;		
	}

	void MakeZero()
	{
		int i,j;
		T* p;
		char* rowStart = 0;
		int iRowSize = m_iWidth*m_iChannel;

		for(j=0;j<m_iHeight;j++){
			rowStart = m_map[j];

			for(i=0;i<iRowSize;i++){
				p = &((T*)rowStart)[i];
				//::new (p) T(0);
				*p = T(0);
			}			
		}
	}

	void* PixelAddress(int x, int y, int band=0)
	{
		return (void *) &m_map[y][(x * m_iChannel + band)* sizeof(T)];
	}

	T& at( int x, int y, int band=0 )
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

	int GetChannel()
	{
		return m_iChannel;
	}

	int GetWidth()
	{
		return m_iWidth;
	}

	int GetHeight()
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

	char* GetMap(int iRow)
	{
		return m_map[iRow];
	}
private:
	std::vector<char*>	m_map;
	
	int m_iChannel;
	int m_iWidth, m_iHeight;
	int m_iEffectWidth;
	int m_iImageSize;
public:
	typedef T value_type;
};


typedef ZCubeImage<unsigned char> ZCubeByteImage;
typedef ZCubeImage<int> ZCubeIntImage;
typedef ZCubeImage<float> ZCubeFloatImage;
typedef ZCubeImage<double> ZCubeDoubleImage;
