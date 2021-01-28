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
	//���캯��
	MeanShiftSeg(void);
	//��������
	~MeanShiftSeg(void);

	//����MeanShift���зָ�������savaPath·����
	void Segment(CxImage &cxImage, int sigmaS, float sigmaR, int minRegion, const std::string &savePath);
	void Segment(ZByteImage &zImage, int sigmaS, float sigmaR, int minRegion, const std::string &savePath);

	//��Ҫ�ָ��ͼ������ص����regions���ݽṹ��
	void GetRegions(std::vector< std::vector<blk::Point> >& regions) const;
	void GetRegions(std::vector< std::vector<blk::Point> >& regions, const Block &block) const;

	//�õ����꣨x,y)�ķָ�����
	int GetSegmMapAt(int x, int y) const;

private:
	//��Դ��CxImage)ͼ��õ��ָ�����ͼ���洢��savaPath·����
	void GetSegmMap(CxImage &cxImage, const std::string &savePath);
	//��Դ��ZImage)ͼ��õ��ָ�����ͼ���洢��savaPath·����
	void GetSegmMap(ZByteImage &zImage, const std::string &savePath);

	// CxImageת��ΪBgImage
	void CxToBgImage(CxImage &cxImg, BgImage &bgImg);

	//ZImageת��ΪBgImage
	void CxToBgImage(ZByteImage &zImg, BgImage &bgImg);

private:
	int m_iWidth;  //ͼ����
	int m_iHeight; //ͼ��߶�
	ZIntImage m_SegMap;   //�ָ�����ͼ

	std::shared_ptr<msImageProcessor> iProc;  //����ͼ�񣨶���ģ���ָ��
};
