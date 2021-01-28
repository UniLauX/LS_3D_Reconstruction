
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

//将comMask的值复制到BinMask中
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
	enum{ NOT_SET = 0, IN_PROCESS = 1, SET = 2 }; // NOT_SET（未处理）IN_PROCESS（处理）、SET（已处理） 
	static const int radius = 2; //图像中点表示圈的半径
	static const int thickness = -1; // 图像中点表示圈的粗细


	//初始化掩码图和各变量
	//mask图GrabCut函数中对应第二个参数，标记图片中哪些属于前景，哪些属于背景
	//mask图只可存入四种数值，分别为：GC_BGD、GC_FGD、GC_PR_BGD、GC_PR_FGD
	//mask初始化为背景，即赋值为GC_BGD
	void reset();

	//初始化窗口和图片
	//将读取的图片和窗口名存入类中的私有变量image和winName中，有利于存储
	//初始化掩码图以及各变量
	void setImageAndWinName( const Mat& _image, const string& _winName );

	//显示图片
	//如果 fgdPxls, bgdPxls, prFgdPxls, prBgdPxls变量非空，则在图片中显示标记的点
	//如果 rectState 已经表示被标记，则也在图片中显示标记的矩形
	void showImage() const;

	//void mouseClick( int event, int x, int y, int flags, void* param );
	int nextIter();
	int getIterCount() const { return iterCount; }

	//new-add
	bool SetRect(cv::Rect& rectangle);

	void SetfgdModelPixel(cv::Point p, bool isPr );


   //private: ORG 
public: //New Change

	//通过矩形标记Mask
	void setRectInMask();

	void setLblsInMask( cv::Point p, bool isPr );

	const string* winName;  //窗口名称
	const Mat* image;      //图像名称
	Mat mask;             //mask图像
	Mat bgdModel, fgdModel;  

	//rectState, lblsState, prLblsState三个变量分别表示矩形标记的状态，
	//鼠标左键标记的状态，鼠标右键标记的状态，分别有三个状态：NOT_SET（未处理）
	//IN_PROCESS（处理）、SET（已处理）
	uchar rectState, lblsState, prLblsState; //矩形框像素状态标记，前景背景像素状态标记，可能的前景背景状态标记
	bool isInitialized;   //whether the image is processed

	Rect rect;
	//在第一次矩形分割后，第二次标记mask值时，四种值出现的点都分别保存在
	//fgdPxls, bgdPxls, prFgdPxls, prBgdPxls四个变量中
	vector<cv::Point> fgdPxls, bgdPxls, prFgdPxls, prBgdPxls;
	

	int iterCount;  //iter count

};



