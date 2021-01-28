#pragma once
#include "ActsParserBase.h"
#include "fileParser.h"
class ActbFileParser:public fileParser ,public ActsParserBase
{
public:
	ActbFileParser(void);
	~ActbFileParser(void);
public:
	static ActbFileParser * GetInstance()
	{
		static ActbFileParser instance;
		return &instance;
	}

	bool LoadProject(const std::string & fileName);    //º”‘ÿœÓƒø

	//virtual bool SaveProject(const std::string & fileName,const std::string &imageName, const std::vector<int> &index) override;
	bool SaveProject(const std::string & fileName,const std::string &imageName, const std::vector<int> &index);

};

