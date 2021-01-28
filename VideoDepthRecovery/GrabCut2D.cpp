#include "GrabCut2D.h"
#include "GCGraph.h"

GrabCut2D::GrabCut2D(void)
{
}


GrabCut2D::~GrabCut2D(void)
{
}

void GrabCut2D::GrabCut( cv::InputArray _img, cv::InputOutputArray _mask, cv::Rect rect, cv::InputOutputArray _bgdModel,cv::InputOutputArray _fgdModel, int iterCount, int mode )
{
	std::cout<<"self write 2D grabcut..."<<std::endl;
	Mat img = _img.getMat();    //copy mat
	Mat& mask = _mask.getMatRef(); //get reference of the Mat
	Mat& bgdModel = _bgdModel.getMatRef(); //get reference of the Mat
	Mat& fgdModel = _fgdModel.getMatRef(); //get reference of the Mat
	
	if( img.empty() )
		CV_Error( CV_StsBadArg, "image is empty" );
	if( img.type() != CV_8UC3 )
		CV_Error( CV_StsBadArg, "image must have CV_8UC3 type" );

	//initialize model param(covariance,mean,ci etc.)
	GMM bgdGMM(bgdModel),fgdGMM(fgdModel);
	Mat compIdxs( img.size(), CV_32SC1 );

	if( mode == GC_INIT_WITH_RECT || mode == GC_INIT_WITH_MASK )
	{
		if( mode == GC_INIT_WITH_RECT )
			GMM::InitMaskWithRect( mask, img.size(), rect );
		else // flag == GC_INIT_WITH_MASK
			GMM::CheckMask( img, mask );
		GMM::InitGMMs( img, mask, bgdGMM, fgdGMM );
	}
	if( iterCount <= 0)
		return;

	if( mode == GC_EVAL )
		GMM::CheckMask( img, mask );

	const double gamma = 50;
	const double lambda = 9*gamma;
	const double beta = GMM::CalcBeta( img );

//	std::cout<<"2D beta:"<<beta<<std::endl;
	cv::Mat leftW, upleftW, upW, uprightW;
    GMM::CalcNWeights( img, leftW, upleftW, upW, uprightW, beta, gamma );

	for( int i = 0; i < iterCount; i++ )
	{
		GCGraph<double> graph;
		GMM::AssignGMMsComponents( img, mask, bgdGMM, fgdGMM, compIdxs );  //may have bug
		GMM::LearnGMMs( img, mask, compIdxs, bgdGMM, fgdGMM );
  
	//输出可能的前景/背景概率
	 //   cv::Point p;
		//for( p.y = 0; p.y < img.rows; p.y++ )
		//{
		//	for( p.x = 0; p.x < img.cols; p.x++)
		//	{
		//		// add node
		//		Vec3b color = img.at<Vec3b>(p);

		//		// set t-weights
		//		double fromSource, toSink;
		//		if( mask.at<uchar>(p) == GC_PR_BGD || mask.at<uchar>(p) == GC_PR_FGD )
		//		{
		//			//fromSource = -log( bgdGMM(color) );
		//			//toSink = -log( fgdGMM(color) );
		//			fromSource = bgdGMM(color) ;
		//			toSink = fgdGMM(color) ;
		//			//std::cout<<"fgdProb: "<<fromSource<<",bgdProb: "<<toSink<<std::endl;
		//		  std::cout<<"objPct: "<<fromSource/(fromSource+toSink)<<std::endl;
		//		 //可以尝试如果 fromSource> toSink.然后将相应的点投到maskImg上看结果。

		//		}
		//	}
		//}
		//
		GMM::ConstructGCGraph(img, mask, bgdGMM, fgdGMM, lambda, leftW, upleftW, upW, uprightW, graph );
	
		GMM::EstimateSegmentation( graph, mask );
	
	}


}
