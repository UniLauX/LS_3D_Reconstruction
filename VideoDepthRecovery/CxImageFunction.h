#pragma once
#include <string>
#include "cximage.h"
class CxImageFunction
{
public:
	CxImageFunction(void);
	~CxImageFunction(void);

public:
	static int GetImageFileType(const std::string &name);
private:
	static std::string FindExtension(const std::string &name);
    static int FindFormat(const std::string &ext);
};

