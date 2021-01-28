#include "FrameSelector_MatchPoints.h"


//构造函数
FrameSelector_MatchPoints::FrameSelector_MatchPoints(void)
{
}

//析构函数
FrameSelector_MatchPoints::~FrameSelector_MatchPoints(void)
{
}

void FrameSelector_MatchPoints:: Init(int maxCount)
{
	m_iMaxCount=maxCount;
}

std::vector<int> FrameSelector_MatchPoints::GetReferenceFrames( int currentID, int frameCount )
{
	throw std::exception("The method or operation is not implemented.");
}

std::vector<int> FrameSelector_MatchPoints::GetReferenceFrames( VDRVideoFrame &framei, int frameCount )
{

	/*
	// 此函数尚未写完
	std::vector<std::pair<int, int> > freameWeight(frameCount);
	for(auto i = 0U; i<freameWeight.size(); i++){
		freameWeight.at(i).first = i;
		freameWeight.at(i).second = 0;
	}

	throw std::exception("The method or operation is not implemented.");
	*/

	std::vector<std::pair<int, int> > freameWeight(frameCount);
	for(auto i = 0U; i<freameWeight.size(); i++){
		freameWeight.at(i).first = i;
		freameWeight.at(i).second = 0;
	}

	for(auto i = 0U; i < framei.m_pvMatchPoints->size(); ++i){
		auto &plinkeri = framei.m_pvMatchPoints->at(i).m_pMatchLinker;
		if(plinkeri->GetCount() <= frameCount * 0.2)
			continue;
		for(auto j = 0U; j<plinkeri->GetCount(); ++j){
			auto &trackPoint = plinkeri->Point(j);
			if(trackPoint.m_iFrameNo != framei.m_iID)
				++freameWeight[trackPoint.m_iFrameNo].second;
		}
	}
	assert(!freameWeight.empty());

	std::sort(freameWeight.begin(), freameWeight.end(), [](const std::pair<int, int>& p1, const std::pair<int, int>& p2){return p1.second > p2.second;});
	std::vector<int> res;

	for(auto i = 0; i + 1 < freameWeight.size() && i < m_iMaxCount; ++i){
		res.push_back(freameWeight[i].first);
	}

	std::cout << std::left << std::setw(20) <<"Reference Frames: ";
	for(auto i=0U; i<res.size(); ++i)
		std::cout << std::setw(4) << res[i];
	std::cout << std::endl;

	return res;

}

