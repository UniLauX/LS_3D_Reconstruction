#pragma once
#include <highgui.h>
#include <cv.h>
#include <imgproc/imgproc.hpp>
#include <iostream>
#include "GCGraph.h"

using namespace cv;
class GMM
{
public:
	static const int componentsCount = 5;  
	//GMM(void);
	GMM( cv::Mat& _model );  
	~GMM(void);

   //增加样本，即为前景或者背景GMM的第ci个高斯模型的像素集（这个像素集是来用估  
   //计计算这个高斯模型的参数的）增加样本像素。计算加入color这个像素后，像素集  
  //中所有像素的RGB三个通道的和sums（用来计算均值），还有它的prods（用来计算协方差），  
  //并且记录这个像素集的像素个数和总的像素个数（用来计算这个高斯模型的权值）。  
void AddSample( int ci, const cv::Vec3d color );  
//GMM参数学习前的初始化，主要是对要求和的变量置零  
void GMM::InitLearning();  
void GMM::EndLearning();
void CalcInverseCovAndDeterm( int ci );   

int GMM::WhichComponent( const Vec3d color ) const; //check当前像素颜色属于哪个高斯分量模型

double GMM::operator()( int ci, const Vec3d color ) const; //返回像素颜色color属于高斯分量ci的概率值

double operator()( const Vec3d color ) const;

//double GMM::ProbBelongtoFgd( const Vec3d color ) const;   //自定义函数，计算像素颜色color属于前景的概率值




/*
  Initialize mask using rectangular.
*/
static void InitMaskWithRect( Mat& mask, Size imgSize, Rect rect )
{
    mask.create( imgSize, CV_8UC1 );
    mask.setTo( GC_BGD );

    rect.x = max(0, rect.x);
    rect.y = max(0, rect.y);
    rect.width = min(rect.width, imgSize.width-rect.x);
    rect.height = min(rect.height, imgSize.height-rect.y);

    (mask(rect)).setTo( Scalar(GC_PR_FGD) );
}

/*
  Check size, type and element values of mask matrix.
 */
static void CheckMask( const Mat& img, const Mat& mask )
{
    if( mask.empty() )
        CV_Error( CV_StsBadArg, "mask is empty" );
    if( mask.type() != CV_8UC1 )
        CV_Error( CV_StsBadArg, "mask must have CV_8UC1 type" );
    if( mask.cols != img.cols || mask.rows != img.rows )
        CV_Error( CV_StsBadArg, "mask must have as many rows and cols as img" );
    for( int y = 0; y < mask.rows; y++ )
    {
        for( int x = 0; x < mask.cols; x++ )
        {
            uchar val = mask.at<uchar>(y,x);
            if( val!=GC_BGD && val!=GC_FGD && val!=GC_PR_BGD && val!=GC_PR_FGD )
                CV_Error( CV_StsBadArg, "mask element value must be equel"
                    "GC_BGD or GC_FGD or GC_PR_BGD or GC_PR_FGD" );
        }
    }
}

/*
  Initialize GMM background and foreground models using kmeans algorithm.
*/
static void InitGMMs( const Mat& img, const Mat& mask, GMM& bgdGMM, GMM& fgdGMM )
{
	
    const int kMeansItCount = 10;
    const int kMeansType = KMEANS_PP_CENTERS;

    Mat bgdLabels, fgdLabels;
    vector<Vec3f> bgdSamples, fgdSamples;
    cv::Point p;
    for( p.y = 0; p.y < img.rows; p.y++ )
    {
        for( p.x = 0; p.x < img.cols; p.x++ )
        {
            if( mask.at<uchar>(p) == GC_BGD || mask.at<uchar>(p) == GC_PR_BGD )
                bgdSamples.push_back( (Vec3f)img.at<Vec3b>(p) );
            else // GC_FGD | GC_PR_FGD
                fgdSamples.push_back( (Vec3f)img.at<Vec3b>(p) );
        }
    }

    CV_Assert( !bgdSamples.empty() && !fgdSamples.empty() );
    Mat _bgdSamples( (int)bgdSamples.size(), 3, CV_32FC1, &bgdSamples[0][0] );
    kmeans( _bgdSamples, GMM::componentsCount, bgdLabels,
            TermCriteria( CV_TERMCRIT_ITER, kMeansItCount, 0.0), 0, kMeansType );
    Mat _fgdSamples( (int)fgdSamples.size(), 3, CV_32FC1, &fgdSamples[0][0] );
    kmeans( _fgdSamples, GMM::componentsCount, fgdLabels,
            TermCriteria( CV_TERMCRIT_ITER, kMeansItCount, 0.0), 0, kMeansType );
     
	
    bgdGMM.InitLearning();
    for( int i = 0; i < (int)bgdSamples.size(); i++ )
        bgdGMM.AddSample( bgdLabels.at<int>(i,0), bgdSamples[i] );
	bgdGMM.EndLearning();


    fgdGMM.InitLearning();
    for( int i = 0; i < (int)fgdSamples.size(); i++ )
        fgdGMM.AddSample( fgdLabels.at<int>(i,0), fgdSamples[i] );
    fgdGMM.EndLearning();

}

/*
  Initialize GMM background and foreground models using kmeans algorithm.
  new-add-将前背景采样点集分别作为参数
*/
//static void InitGMMs( const Mat& img, const Mat& mask, GMM& bgdGMM, GMM& fgdGMM，vector<Vec3f>& bgdSamples,vector<Vec3f>& fgdSamples )
//{
//	
//    const int kMeansItCount = 10;
//    const int kMeansType = KMEANS_PP_CENTERS;
//
//    Mat bgdLabels, fgdLabels;
//   // vector<Vec3f> bgdSamples, fgdSamples;
//    cv::Point p;
//    for( p.y = 0; p.y < img.rows; p.y++ )
//    {
//        for( p.x = 0; p.x < img.cols; p.x++ )
//        {
//            if( mask.at<uchar>(p) == GC_BGD || mask.at<uchar>(p) == GC_PR_BGD )
//                bgdSamples.push_back( (Vec3f)img.at<Vec3b>(p) );
//            else // GC_FGD | GC_PR_FGD
//                fgdSamples.push_back( (Vec3f)img.at<Vec3b>(p) );
//        }
//    }
//
//    CV_Assert( !bgdSamples.empty() && !fgdSamples.empty() );
//    Mat _bgdSamples( (int)bgdSamples.size(), 3, CV_32FC1, &bgdSamples[0][0] );
//    kmeans( _bgdSamples, GMM::componentsCount, bgdLabels,
//            TermCriteria( CV_TERMCRIT_ITER, kMeansItCount, 0.0), 0, kMeansType );
//    Mat _fgdSamples( (int)fgdSamples.size(), 3, CV_32FC1, &fgdSamples[0][0] );
//    kmeans( _fgdSamples, GMM::componentsCount, fgdLabels,
//            TermCriteria( CV_TERMCRIT_ITER, kMeansItCount, 0.0), 0, kMeansType );
//     
//	
//    bgdGMM.InitLearning();
//    for( int i = 0; i < (int)bgdSamples.size(); i++ )
//        bgdGMM.AddSample( bgdLabels.at<int>(i,0), bgdSamples[i] );
//	bgdGMM.EndLearning();
//
//
//    fgdGMM.InitLearning();
//    for( int i = 0; i < (int)fgdSamples.size(); i++ )
//        fgdGMM.AddSample( fgdLabels.at<int>(i,0), fgdSamples[i] );
//    fgdGMM.EndLearning();
//
//}


/*
  Calculate beta - parameter of GrabCut algorithm.
  beta = 1/(2*avg(sqr(||color[i] - color[j]||)))
*/
static double CalcBeta( const Mat& img )
{
    double beta = 0;
    for( int y = 0; y < img.rows; y++ )
    {
        for( int x = 0; x < img.cols; x++ )
        {
            Vec3d color = img.at<Vec3b>(y,x);
            if( x>0 ) // left
            {
                Vec3d diff = color - (Vec3d)img.at<Vec3b>(y,x-1);
                beta += diff.dot(diff);
            }
            if( y>0 && x>0 ) // upleft
            {
                Vec3d diff = color - (Vec3d)img.at<Vec3b>(y-1,x-1);
                beta += diff.dot(diff);
            }
            if( y>0 ) // up
            {
                Vec3d diff = color - (Vec3d)img.at<Vec3b>(y-1,x);
                beta += diff.dot(diff);
            }
            if( y>0 && x<img.cols-1) // upright
            {
                Vec3d diff = color - (Vec3d)img.at<Vec3b>(y-1,x+1);
                beta += diff.dot(diff);
            }
        }
    }
    if( beta <= std::numeric_limits<double>::epsilon() )
        beta = 0;
    else
        beta = 1.f / (2 * beta/(4*img.cols*img.rows - 3*img.cols - 3*img.rows + 2) ); //not so clearly

    return beta;
}


/*
  Calculate weights of noterminal vertices of graph.
  beta and gamma - parameters of GrabCut algorithm.
 */
//related to the distance between neighbor pixels
static void CalcNWeights( const Mat& img, Mat& leftW, Mat& upleftW, Mat& upW, Mat& uprightW, double beta, double gamma )
{
    const double gammaDivSqrt2 = gamma / std::sqrt(2.0f);   //seems not the same as the paper
    leftW.create( img.rows, img.cols, CV_64FC1 );
    upleftW.create( img.rows, img.cols, CV_64FC1);
    upW.create( img.rows, img.cols, CV_64FC1 );
    uprightW.create( img.rows, img.cols, CV_64FC1 );

    for( int y = 0; y < img.rows; y++ )
    {
        for( int x = 0; x < img.cols; x++ )
        {
            Vec3d color = img.at<Vec3b>(y,x);
            if( x-1>=0 ) // left
            {
                Vec3d diff = color - (Vec3d)img.at<Vec3b>(y,x-1);
                leftW.at<double>(y,x) = gamma * exp(-beta*diff.dot(diff));
            }
            else
                leftW.at<double>(y,x) = 0;

            if( x-1>=0 && y-1>=0 ) // upleft
            {
                Vec3d diff = color - (Vec3d)img.at<Vec3b>(y-1,x-1);
                upleftW.at<double>(y,x) = gammaDivSqrt2 * exp(-beta*diff.dot(diff));
            }
            else
                upleftW.at<double>(y,x) = 0;
            if( y-1>=0 ) // up
            {
                Vec3d diff = color - (Vec3d)img.at<Vec3b>(y-1,x);
                upW.at<double>(y,x) = gamma * exp(-beta*diff.dot(diff));
            }
            else
                upW.at<double>(y,x) = 0;
            if( x+1<img.cols && y-1>=0 ) // upright
            {
                Vec3d diff = color - (Vec3d)img.at<Vec3b>(y-1,x+1);
                uprightW.at<double>(y,x) = gammaDivSqrt2 * exp(-beta*diff.dot(diff));
            }
            else
                uprightW.at<double>(y,x) = 0;
        }
    }
   
}


/*
  Assign GMMs components for each pixel.
*/
static void AssignGMMsComponents( const Mat& img, const Mat& mask, const GMM& bgdGMM, const GMM& fgdGMM, Mat& compIdxs )
{
    cv::Point p;
    for( p.y = 0; p.y < img.rows; p.y++ )
    {
        for( p.x = 0; p.x < img.cols; p.x++ )
        {
            Vec3d color = img.at<Vec3b>(p);
            compIdxs.at<int>(p) = mask.at<uchar>(p) == GC_BGD || mask.at<uchar>(p) == GC_PR_BGD ?
                bgdGMM.WhichComponent(color) : fgdGMM.WhichComponent(color);
			//这样会不会存在一个问题：即前景和背景有相同的component序号？如果是，如何区分？
        }
    }
}

/*
  Learn GMMs parameters.
*/
static void LearnGMMs( const Mat& img, const Mat& mask, const Mat& compIdxs, GMM& bgdGMM, GMM& fgdGMM )
{
    bgdGMM.InitLearning();
    fgdGMM.InitLearning();
	
    cv::Point p;
    for( int ci = 0; ci < GMM::componentsCount; ci++ )
    {
        for( p.y = 0; p.y < img.rows; p.y++ )
        {
            for( p.x = 0; p.x < img.cols; p.x++ )
            {
                if( compIdxs.at<int>(p) == ci )
                {
                    if( mask.at<uchar>(p) == GC_BGD || mask.at<uchar>(p) == GC_PR_BGD )
                        bgdGMM.AddSample( ci, img.at<Vec3b>(p) );
                    else
                        fgdGMM.AddSample( ci, img.at<Vec3b>(p) );
                }
            }
        }
    }

    bgdGMM.EndLearning();
   
    fgdGMM.EndLearning();   //bug here
	
}


////
// // Learn GMMs parameters.
//  //new add-将前背景采样集分别提出来作为参数
////
//static void LearnGMMs( const Mat& img, const Mat& mask, const Mat& compIdxs, GMM& bgdGMM, GMM& fgdGMM )
//{
//    bgdGMM.InitLearning();
//    fgdGMM.InitLearning();
//    cv::Point p;
//    for( int ci = 0; ci < GMM::componentsCount; ci++ )
//    {
//        for( p.y = 0; p.y < img.rows; p.y++ )
//        {
//            for( p.x = 0; p.x < img.cols; p.x++ )
//            {
//                if( compIdxs.at<int>(p) == ci )
//                {
//                    if( mask.at<uchar>(p) == GC_BGD || mask.at<uchar>(p) == GC_PR_BGD )
//                        bgdGMM.AddSample( ci, img.at<Vec3b>(p) );
//                    else
//                        fgdGMM.AddSample( ci, img.at<Vec3b>(p) );
//                }
//            }
//        }
//    }
//    bgdGMM.EndLearning();
//    fgdGMM.EndLearning();
//}
//


/*
  Construct GCGraph
*/
static void ConstructGCGraph( const Mat& img, const Mat& mask, const GMM& bgdGMM, const GMM& fgdGMM, double lambda,
                       const Mat& leftW, const Mat& upleftW, const Mat& upW, const Mat& uprightW,
                       GCGraph<double>& graph )
{
    int vtxCount = img.cols*img.rows,
        edgeCount = 2*(4*img.cols*img.rows - 3*(img.cols + img.rows) + 2); //why?
    graph.create(vtxCount, edgeCount);
    cv::Point p;
    for( p.y = 0; p.y < img.rows; p.y++ )
    {
        for( p.x = 0; p.x < img.cols; p.x++)
        {
            // add node
            int vtxIdx = graph.addVtx();
            Vec3b color = img.at<Vec3b>(p);

            // set t-weights
            double fromSource, toSink;
            if( mask.at<uchar>(p) == GC_PR_BGD || mask.at<uchar>(p) == GC_PR_FGD )
            {
                fromSource = -log( bgdGMM(color) );
                toSink = -log( fgdGMM(color) );
            }
            else if( mask.at<uchar>(p) == GC_BGD )
            {
                fromSource = 0;
                toSink = lambda;
            }
            else // GC_FGD
            {
                fromSource = lambda;
                toSink = 0;
            }
            graph.addTermWeights( vtxIdx, fromSource, toSink );

            // set n-weights
            if( p.x>0 )
            {
                double w = leftW.at<double>(p);
                graph.addEdges( vtxIdx, vtxIdx-1, w, w );
            }
            if( p.x>0 && p.y>0 )
            {
                double w = upleftW.at<double>(p);
                graph.addEdges( vtxIdx, vtxIdx-img.cols-1, w, w );
            }
            if( p.y>0 )
            {
                double w = upW.at<double>(p);
                graph.addEdges( vtxIdx, vtxIdx-img.cols, w, w );
            }
            if( p.x<img.cols-1 && p.y>0 )
            {
                double w = uprightW.at<double>(p);
                graph.addEdges( vtxIdx, vtxIdx-img.cols+1, w, w );
            }
        }
    }
}

/*
  Estimate segmentation using MaxFlow algorithm
*/
static void EstimateSegmentation( GCGraph<double>& graph, Mat& mask )
{
    graph.maxFlow();
    cv::Point p;
    for( p.y = 0; p.y < mask.rows; p.y++ )
    {
        for( p.x = 0; p.x < mask.cols; p.x++ )
        {
            if( mask.at<uchar>(p) == GC_PR_BGD || mask.at<uchar>(p) == GC_PR_FGD )
            {
                if( graph.inSourceSegment( p.y*mask.cols+p.x /*vertex index*/ ) )
                    mask.at<uchar>(p) = GC_PR_FGD;
                else
                    mask.at<uchar>(p) = GC_PR_BGD;
            }
        }
    }
}



////自定义函数，得到（x,y）像素颜色属于前景/背景的概率
//static void GetProb(const Mat& img,const Mat& mask,const GMM& bgdGMM, const GMM& fgdGMM,int x,int y)
//{
//	double probValue=0.0;
//	cv::Point p;
//	p.x=x;
//	p.y=y;
//	Vec3d color=img.at<Vec3b>(p);
//	probValue=mask.at<uchar>(p) == GC_BGD || mask.at<uchar>(p) == GC_PR_BGD ?
//	bgdGMM.ProbBelongtoFgd(color) : fgdGMM.ProbBelongtoFgd(color);
//  
//	std::cout<<"probValue:"<<probValue<<std::endl;
//	//need change and continue coding!
//}

//private:
public:
	cv::Mat model;  
	double* coefs;  
	double* mean;  
	double* cov;  

	double inverseCovs[componentsCount][3][3]; //协方差的逆矩阵  
	double covDeterms[componentsCount];  //协方差的行列式  

	double sums[componentsCount][3];  //颜色通道求和
	double prods[componentsCount][3][3];//协方差  
	int sampleCounts[componentsCount];//  单个组件样本个数
	int totalSampleCount;            //所有样本个数总和

};

