#pragma once
#include <string>
#include <fstream>
#include <iostream>


#define  MAX_LINE_VRML 10000              //单行字符最大数
#define  MAX_WORD_VRML 1000               //检索单词包含字符最大数
class fileParser
{
public:
	fileParser(void);
	~fileParser(void);

protected:
    int m_iFileSize;                  //文件大小
	char *m_pBuffer;                  //存储整个文件
	int m_iIndexBuffer;               //文件索引
	std::string m_sFileName;           
	char m_pBufferLine[MAX_LINE_VRML];  //存储单行
    char m_pBufferWord[MAX_WORD_VRML];  //存储两个空格之间的字符串

public:
    void Free();                           //释放内存空间
	virtual int Run(const char* fileName);
    virtual int ReadFile(const char* fileName);//读取整个文件
	int ReadLine();                 //读取本行内容
    int NextLine();                 //读取下一行内容
	int ReadWord();                 //读取相邻空格之间字符串内容

	int OffsetToString(const char* stringFind);//定位stringFind的开始位置

	int OffsetToString(const char* stringFind,int start,int end); //在给定的index范围start和end之间定位stringFind的位置


};

