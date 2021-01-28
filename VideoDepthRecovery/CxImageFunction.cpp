#include "CxImageFunction.h"


CxImageFunction::CxImageFunction(void)
{
}

CxImageFunction::~CxImageFunction(void)
{
}

//返回对应于Cximage格式的文件类型，如CXIMAGE_FORMAT_JPG
int CxImageFunction::GetImageFileType(const std::string &name)
{
	 return FindFormat(FindExtension(name));
}
                         //找文件的后缀名
std::string CxImageFunction::FindExtension(const std::string &name)
{
     int len=name.length();
	 int i;
	 for(i=len-1;i>=0;i--)         //有后缀名
	 {
		 if(name[i]=='.')
		 return name.substr(i+1);       
	 }
	 return std::string("");         //无后缀名，返回空串
}

//将文件后缀名字符串与CxImage中文件格式对应起来 
int CxImageFunction::FindFormat(const std::string &ext)
{
	int type=0;
	if(ext==_T("bmp"))   type=CXIMAGE_FORMAT_BMP;
#if CXIMAGE_SUPPORT_JPG
	else if(ext==_T("jpg")||ext==_T("jpeg")) type=CXIMAGE_FORMAT_JPG;
#endif
#if CXIMAGE_SUPPORT_GIF
	else if (ext == _T("gif"))				type = CXIMAGE_FORMAT_GIF;
#endif
#if CXIMAGE_SUPPORT_PNG
	else if (ext == _T("png"))				type = CXIMAGE_FORMAT_PNG;
#endif
#if CXIMAGE_SUPPORT_MNG
	else if (ext==_T("mng")||ext==_T("jng"))	type = CXIMAGE_FORMAT_MNG;
#endif
#if CXIMAGE_SUPPORT_ICO
	else if (ext == _T("ico"))				type = CXIMAGE_FORMAT_ICO;
#endif
#if CXIMAGE_SUPPORT_TIF
	else if (ext==_T("tiff")||ext==_T("tif"))	type = CXIMAGE_FORMAT_TIF;
#endif
#if CXIMAGE_SUPPORT_TGA
	else if (ext==_T("tga"))				type = CXIMAGE_FORMAT_TGA;
#endif
#if CXIMAGE_SUPPORT_PCX
	else if (ext==_T("pcx"))				type = CXIMAGE_FORMAT_PCX;
#endif
#if CXIMAGE_SUPPORT_WBMP
	else if (ext==_T("wbmp"))				type = CXIMAGE_FORMAT_WBMP;
#endif
#if CXIMAGE_SUPPORT_WMF
	else if (ext==_T("wmf")||ext==_T("emf"))	type = CXIMAGE_FORMAT_WMF;
#endif
#if CXIMAGE_SUPPORT_J2K
	else if (ext==_T("j2k")||ext==_T("jp2"))	type = CXIMAGE_FORMAT_J2K;
#endif
#if CXIMAGE_SUPPORT_JBG
	else if (ext==_T("jbg"))				type = CXIMAGE_FORMAT_JBG;
#endif
#if CXIMAGE_SUPPORT_JP2
	else if (ext==_T("jp2")||ext==_T("j2k"))	type = CXIMAGE_FORMAT_JP2;
#endif
#if CXIMAGE_SUPPORT_JPC
	else if (ext==_T("jpc")||ext==_T("j2c"))	type = CXIMAGE_FORMAT_JPC;
#endif
#if CXIMAGE_SUPPORT_PGX
	else if (ext==_T("pgx"))				type = CXIMAGE_FORMAT_PGX;
#endif
#if CXIMAGE_SUPPORT_RAS
	else if (ext==_T("ras"))				type = CXIMAGE_FORMAT_RAS;
#endif
#if CXIMAGE_SUPPORT_PNM
	else if (ext==_T("pnm")||ext==_T("pgm")||ext==_T("ppm")) type = CXIMAGE_FORMAT_PNM;
#endif
	else type = CXIMAGE_FORMAT_UNKNOWN;

	return type;
}