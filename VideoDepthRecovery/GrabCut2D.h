#pragma once

//#include "gcgraph.h"
//not used!!
#include <highgui.h>
#include <cv.h>
#include <iostream>
#include "GMM.h"
using namespace cv;
class GrabCut2D
{
public:

	GrabCut2D(void);
	~GrabCut2D(void);

public: 
    void GrabCut( cv::InputArray _img, cv::InputOutputArray _mask, cv::Rect rect,
		cv::InputOutputArray _bgdModel,cv::InputOutputArray _fgdModel,
		int iterCount, int mode );


};

