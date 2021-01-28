#pragma once
#include <iostream>
#include <string>
class FileNameParser
{
public:
	FileNameParser(void);
	~FileNameParser(void);

public:
	static std::string findFileNameDir(const std::string &fileName);    //����Ϊstaticһ�����ܹ���������������
	static std::string findFileNameName(const std::string &fileName);
	static std::string findFileNameNameWitoutExt(const std::string &fileName);
    static std::string findFileNameExt(const std::string & fileName);
    static std::string findFileNamePathNameWithoutExt(const std::string &fileName);
	static int findFileNameNumber(const std::string &fileName);                //��֡ͼ�������
    static std::string increaseFileNameNumber(const std::string&fileName, int incr);

};


