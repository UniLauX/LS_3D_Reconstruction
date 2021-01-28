#pragma once
#include "MeanShift/BgImage.h"
#include "MeanShift/msImageProcessor.h"
#include "cximage.h"
#include "ZImage.h"
#include "CxImageFunction.h"
#include <vector>
#include <string>
#include "Block.h"
#include <iostream>
#include <map>
class Point;
using namespace blk;
class Block;

class MeanShiftSeg
{
public:
	//构造函数
	MeanShiftSeg(void);
	//析构函数
	~MeanShiftSeg(void);

	//调用MeanShift进行分割，结果存在savaPath路径上
	void Segment(CxImage &cxImage, int sigmaS, float sigmaR, int minRegion, const std::string &savePath);
	void Segment(ZByteImage &zImage, int sigmaS, float sigmaR, int minRegion, const std::string &savePath);

	//将要分割的图像的像素点放入regions数据结构中
	void GetRegions(std::vector< std::vector<blk::Point> >& regions) const;
	void GetRegions(std::vector< std::vector<blk::Point> >& regions, const Block &block) const;

	//得到坐标（x,y)的分割（结果）
	int GetSegmMapAt(int x, int y) const;

private:
	//由源（CxImage)图像得到分割（结果）图，存储到savaPath路径上
	void GetSegmMap(CxImage &cxImage, const std::string &savePath);
	//由源（ZImage)图像得到分割（结果）图，存储到savaPath路径上
	void GetSegmMap(ZByteImage &zImage, const std::string &savePath);

	// CxImage转换为BgImage
	void CxToBgImage(CxImage &cxImg, BgImage &bgImg);

	//ZImage转换为BgImage
	void CxToBgImage(ZByteImage &zImg, BgImage &bgImg);

private:
	int m_iWidth;  //图像宽度
	int m_iHeight; //图像高度
	ZIntImage m_SegMap;   //分割（结果）图

	std::shared_ptr<msImageProcessor> iProc;  //处理图像（对象的）的指针
};
