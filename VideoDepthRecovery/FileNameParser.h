#pragma once
#include <iostream>
#include <string>
class FileNameParser
{
public:
	FileNameParser(void);
	~FileNameParser(void);

public:
	static std::string findFileNameDir(const std::string &fileName);    //定义为static一遍类能够在其他类中引用
	static std::string findFileNameName(const std::string &fileName);
	static std::string findFileNameNameWitoutExt(const std::string &fileName);
    static std::string findFileNameExt(const std::string & fileName);
    static std::string findFileNamePathNameWithoutExt(const std::string &fileName);
	static int findFileNameNumber(const std::string &fileName);                //单帧图像的数字
    static std::string increaseFileNameNumber(const std::string&fileName, int incr);

};


