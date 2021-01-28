
#pragma once

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace std;
using namespace cv;
const Scalar BLUE = Scalar(255,0,0); // Background color
const Scalar RED = Scalar(0,0,255); //Foreground color
const Scalar LIGHTBLUE = Scalar(255,255,160);//ProbBackground color
const Scalar PINK = Scalar(230,130,255); //ProbBackground color

const Scalar GREEN = Scalar(0,255,0); //Rectangle color

//��comMask��ֵ���Ƶ�BinMask��
static void getBinMask( const Mat& comMask, Mat& binMask )
{
	if( comMask.empty() || comMask.type()!=CV_8UC1 )
		CV_Error( CV_StsBadArg, "comMask is empty or has incorrect type (not CV_8UC1)" );
	if( binMask.empty() || binMask.rows!=comMask.rows || binMask.cols!=comMask.cols )
		binMask.create( comMask.size(), CV_8UC1 );
	binMask = comMask & 1;
}
class GCApplication
{
public:
	enum{ NOT_SET = 0, IN_PROCESS = 1, SET = 2 }; // NOT_SET��δ����IN_PROCESS��������SET���Ѵ��� 
	static const int radius = 2; //ͼ���е��ʾȦ�İ뾶
	static const int thickness = -1; // ͼ���е��ʾȦ�Ĵ�ϸ


	//��ʼ������ͼ�͸�����
	//maskͼGrabCut�����ж�Ӧ�ڶ������������ͼƬ����Щ����ǰ������Щ���ڱ���
	//maskͼֻ�ɴ���������ֵ���ֱ�Ϊ��GC_BGD��GC_FGD��GC_PR_BGD��GC_PR_FGD
	//mask��ʼ��Ϊ����������ֵΪGC_BGD
	void reset();

	//��ʼ�����ں�ͼƬ
	//����ȡ��ͼƬ�ʹ������������е�˽�б���image��winName�У������ڴ洢
	//��ʼ������ͼ�Լ�������
	void setImageAndWinName( const Mat& _image, const string& _winName );

	//��ʾͼƬ
	//��� fgdPxls, bgdPxls, prFgdPxls, prBgdPxls�����ǿգ�����ͼƬ����ʾ��ǵĵ�
	//��� rectState �Ѿ���ʾ����ǣ���Ҳ��ͼƬ����ʾ��ǵľ���
	void showImage() const;

	//void mouseClick( int event, int x, int y, int flags, void* param );
	int nextIter();
	int getIterCount() const { return iterCount; }

	//new-add
	bool SetRect(cv::Rect& rectangle);

	void SetfgdModelPixel(cv::Point p, bool isPr );


   //private: ORG 
public: //New Change

	//ͨ�����α��Mask
	void setRectInMask();

	void setLblsInMask( cv::Point p, bool isPr );

	const string* winName;  //��������
	const Mat* image;      //ͼ������
	Mat mask;             //maskͼ��
	Mat bgdModel, fgdModel;  

	//rectState, lblsState, prLblsState���������ֱ��ʾ���α�ǵ�״̬��
	//��������ǵ�״̬������Ҽ���ǵ�״̬���ֱ�������״̬��NOT_SET��δ����
	//IN_PROCESS��������SET���Ѵ���
	uchar rectState, lblsState, prLblsState; //���ο�����״̬��ǣ�ǰ����������״̬��ǣ����ܵ�ǰ������״̬���
	bool isInitialized;   //whether the image is processed

	Rect rect;
	//�ڵ�һ�ξ��ηָ�󣬵ڶ��α��maskֵʱ������ֵ���ֵĵ㶼�ֱ𱣴���
	//fgdPxls, bgdPxls, prFgdPxls, prBgdPxls�ĸ�������
	vector<cv::Point> fgdPxls, bgdPxls, prFgdPxls, prBgdPxls;
	

	int iterCount;  //iter count

};



