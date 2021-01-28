#pragma once
#include <string>
#include <fstream>
#include <iostream>


#define  MAX_LINE_VRML 10000              //�����ַ������
#define  MAX_WORD_VRML 1000               //�������ʰ����ַ������
class fileParser
{
public:
	fileParser(void);
	~fileParser(void);

protected:
    int m_iFileSize;                  //�ļ���С
	char *m_pBuffer;                  //�洢�����ļ�
	int m_iIndexBuffer;               //�ļ�����
	std::string m_sFileName;           
	char m_pBufferLine[MAX_LINE_VRML];  //�洢����
    char m_pBufferWord[MAX_WORD_VRML];  //�洢�����ո�֮����ַ���

public:
    void Free();                           //�ͷ��ڴ�ռ�
	virtual int Run(const char* fileName);
    virtual int ReadFile(const char* fileName);//��ȡ�����ļ�
	int ReadLine();                 //��ȡ��������
    int NextLine();                 //��ȡ��һ������
	int ReadWord();                 //��ȡ���ڿո�֮���ַ�������

	int OffsetToString(const char* stringFind);//��λstringFind�Ŀ�ʼλ��

	int OffsetToString(const char* stringFind,int start,int end); //�ڸ�����index��Χstart��end֮�䶨λstringFind��λ��


};

