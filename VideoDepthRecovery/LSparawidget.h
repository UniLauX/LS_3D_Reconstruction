#ifndef LSPARAWIDGET_H
#define LSPARAWIDGET_H

#include <QWidget>
#include "ui_lsparawidget.h"

#include <iostream>


class LSParaWidget : public QWidget
{
	Q_OBJECT

public:
	LSParaWidget(QWidget *parent = 0);
	~LSParaWidget();
	void Inialize();  //initialize the system parameters


private:
	//Ui::LSParaWidget ui;
	std::shared_ptr<Ui::LSParaWidget> ui;

public:
	//get a static instance,for the reference using
	static LSParaWidget *GetInstance()
	{
		static LSParaWidget instance;
		return &instance;
	}


public:

	// get visualhull parameters
	void GetVisualPara(int& iDetailLevels, int& iImgApron){
		iDetailLevels = m_iDetailLevels;
		iImgApron = m_iImgApron;
	}


	//get AutoVisualHull parameters
	void GetAutoVisualPara(int& iMaxIntervalVoxel,int& iImgApron)
	{
		iMaxIntervalVoxel=m_iMaxIntervalVoxel;
		iImgApron=m_iImgApron;
	}


	//get constrain sigma for depth init
	float GetVisualHullConstrianSigma(){
		return m_fVisualHullConstrianSigma;
	}

	 //set disparity range for different frames
	void SetDspRange(std::vector<double> dDspMin,std::vector<double> dDspMax){

		int frameNum=dDspMin.size();
		m_dDspMin.resize(frameNum);
		m_dDspMax.resize(frameNum);

	   for(int iFrame=0;iFrame<frameNum;iFrame++)
	   {
		m_dDspMin[iFrame] = dDspMin[iFrame];
		m_dDspMax[iFrame] = dDspMax[iFrame];
	   }
	}

	//get disparity parameters for different frames
	void GetDspPara(int& iDspLevel, std::vector<double>& dDspMin, std::vector<double>& dDspMax){
		iDspLevel = m_iDspLevel;
		dDspMin = m_dDspMin;
		dDspMax = m_dDspMax;
	}

	// get dispiratity level( here is 101)
	int GetDspLevel(){
		return m_iDspLevel;
	}

	//Get reference frame index for calculating depth
	void GetReferenceFrameIndex(std::vector<std::pair<int, int> >& FwFrameIndex, std::vector<std::pair<int, int> >& BwFrameIndex, 
		int CurrentCameraId, int CurrentIndex, int StartIndex, int EndIndex);


	//new add---Optical flow infomation
	void GetOptReferenceFrameIndex(std::vector<std::pair<int, int> >& FwFrameIndex, std::vector<std::pair<int, int> >& BwFrameIndex, 
		int CurrentCameraId, int CurrentFrameId, int StartFrmIndex, int EndFrmIndex);




	//project(for depth recovery), get color sigma
	float GetColorSigma(){    
		return m_fColorSigma;
	}

	float GetDiffTrunc()
	{
		return m_fDiffTruncated;
	}
	// get color penalty factor
	float GetMissPenalty(){
		return m_fColorMissPenalty;
	}

	//set datacost weight
	void SetDataCostWeight(float fDataCostWeight){
		m_fDataCostWeight = fDataCostWeight;
	}

	//get datacost weight
	float GetDataCostWeight(){
		return m_fDataCostWeight;
	}

	//get true datacost weight
	float GetTrueDataCostWeight(){
		return (m_iDspLevel - 1) * m_fDataCostWeight / 100.0F;
	}
	
	// weather output temp result
	bool GetIfGenerateTempResult(){
		return m_bGenerateTempResult;
	}
	
	//for BP, get true DiscK
	float GetTrueDiscK(){
		return m_fDiscK / 100.0 * (m_iDspLevel - 1);
	}

	// for plane fitting
	double GetTruetSegErrRateThreshold()
	{
		return m_dSegErrRateThreshold * GetTrueDataCostWeight() / GetTrueDiscK();
	}


	// get discK
	float GetDiscK(){
		return m_fDiscK;
	}
	//set discK
	void SetDiscK(float value){
		m_fDiscK = value;
	}

     
	//depthExpansion (for DE)
	int GetSubSample(){
		return m_iSubSample;
	}
	int GetSubIter(){
		return m_iSubIter;
	}

	//get dsparity sigma
	float GetDspSigma(){
		return m_fDspSigma;
	}
	//set dsparity sigma
	void SetDspSigma(float value){
		m_fDspSigma = value;
	}

	// refine execute number
	int GetRefinePassCount(){
		return m_iRefinePassCount;
	}


	// get poisson samples
	float GetPoissonSamples(){
		return m_fPoissonSamples;
	}

	
	//reference frames parameters


	//reference frames parameters ( for different cams in the same frames)
	int GetCamRefStart()
	{
		return  m_iRefStart; 
	}

	int GetCamRefStep()
	{
		return m_iRefStep;
	}
	
	int GetCamRefCount()
	{
		return m_iRefCount;
	}
	//reference frames parameters ( for different frames in the same cam)
	int GetFrmRefStart()
	{
		return  m_iFrmRefStart; 
	}

	int GetFrmRefStep()
	{
		return m_iFrmRefStep;
	}

	int GetFrmRefCount()
	{
		return m_iFrmRefCount;
	}


	double GetColorDiffThreshold()
	{
		return m_dColorDiffThreshold;
	}

	// get frameCount
	float GetFrameCount(){
		return m_iFrameCount;
	}
	//set frameCount
	void SetFrameCount(int frameCount){
		m_iFrameCount = frameCount;
	}

	// get cameraNumber
	float GetCameraNumber(){
		return m_iCameraNumber;
	}
	//set cameraNumber
	void SetCameraNumber(int cameraNumber){
		m_iCameraNumber = cameraNumber;
	}





public:
	//dsp parameters
	int m_iDspLevel;        //disparity level
	std::vector<double> m_dDspMin;   //min dsp
	std::vector<double> m_dDspMax;   //max dsp

	//Mean Shift Segmentation
	int m_iSegSpatial;
	float m_fSegColor;
	int m_iSegMinsize;
	int m_iPlaneFittingSize;
	double m_dSegErrRateThreshold;  // for planefitting


	//Visual Hull
	int m_iDetailLevels;     // cube cut level( here assign 9)
	int m_iImgApron;         // mask enrich pixels(here assign 30)


	//AutoVisualHull
	double m_dColorDiffThreshold;   // for background subtraction (here assign 5)
    int m_iMaxIntervalVoxel;     // voxel number in the largest edge of boundingBox among x,y,z axis	

	// depth to mesh
	float m_fVisualHullConstrianSigma;   // constrain for depth Init(here assign 0.95)

	//neighboor cams reference frames
	int m_iRefStart;   //reference start frame(cam)
	int m_iRefStep;    //reference end frame(cam)
	int m_iRefCount;   //reference frame count(cam)



	//new add-- for optical flow calculation(temporal consistence)
	int m_iFrmRefStart;   //reference start frame(frame)
	int m_iFrmRefStep;    //reference end frame(frame)
	int m_iFrmRefCount;   //reference frame count(frame)


	
	//temporary output result
	bool m_bGenerateTempResult;   // weather output temporary result
    
    //block
	int m_iBlockCountX;   //block numbers in X ray
	int m_iBlockCountY;  //block numbers in Y  ray
	float m_fOverlap;    //overlap between two nearby block 

	//project
	float m_fColorSigma;   //sigma_c

	float m_fDiffTruncated; //color difference truncate value 
	//

	//double m_dProjectSigma;
	float m_fDspSigma;        //sigma_d
	float m_fColorMissPenalty; // penalty factor
	

	//datacost weight
	float m_fDataCostWeight;

	//bp
	float m_fDiscK;

	//depth expansion
	int m_iSubSample;
	int m_iSubIter;


	//depth to mesh
	float m_fPoissonSamples;    //use for possion recon

	//refine(BO)
	int m_iRefinePassCount;


	//input data related
	int m_iFrameCount;
	int m_iCameraNumber;
};

#endif // LSPARAWIDGET_H
