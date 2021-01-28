#include "fileParser.h"


fileParser::fileParser(void)
{
	//初始化参数
	m_pBuffer=NULL;
	m_pBufferLine[0]='\0';
	m_pBufferWord[0]='\0';
	m_iIndexBuffer=0;

}


fileParser::~fileParser(void)
{
	Free();
}

void fileParser::Free()
{
	if(m_pBuffer!=NULL)
	{
		delete [] m_pBuffer;

	     m_pBuffer=NULL;
	}
	m_iIndexBuffer=0;
}

int fileParser::Run(const char* fileName)
{
	return 1;
}

int fileParser::ReadFile(const char* fileName)
{
	m_sFileName=fileName;

	std::fstream infile;
	infile.open(m_sFileName,std::ios::in|std::ios::binary);   //读文件

	if(infile.good()==false)      //good函数检测读入是否正常
	{
		std::cerr<<"unable to open the file for reading\n";
		return 0;
	}
/*
	    seekg（）是对输入文件定位，它有两个参数：第一个参数是偏移量，第二个参数是基地址。
		对于第一个参数，可以是正负数值，正的表示向后偏移，负的表示向前偏移。而第二个参数可以是：
		ios：：beg：表示输入流的开始位置
		ios：：cur：表示输入流的当前位置
		ios：：end：表示输入流的结束位置
		tellg（）函数不需要带参数，它返回当前定位指针的位置，也代表着输入流的大小。
*/
	infile.seekg(0,std::ios::end);
	m_iFileSize=infile.tellg();

	infile.seekg(0,std::ios::beg);
	
	if(m_pBuffer!=NULL)     //清空
		Free();

	m_pBuffer=new char[m_iFileSize];
	if(m_pBuffer==NULL)
	{
		std::cerr<<"Insufficient memory\n";
		return 0;
	}
	if(infile.read(m_pBuffer,m_iFileSize)==false)              //读文件异常
	{
		std::cerr<<"Error during reading\n";
		return 0;
	}

	 infile.close();                //关闭读入流

	return 1;

}

int fileParser::ReadLine()     //读取一行
{
	m_pBufferLine[0]='\0';
	int i=0;
	
	//读取本行内容
	do 
	 m_pBufferLine[i++]=m_pBuffer[m_iIndexBuffer++];
	while(m_pBuffer[m_iIndexBuffer-1]!='\n'&&i<MAX_LINE_VRML&&m_iIndexBuffer<m_iFileSize);

	m_pBufferLine[i-2]='\0';
	return 1;
}

   
int fileParser::NextLine()
{
	int i=0;
	int tempeIndex=0;
	
	//从当前开始，包括当前字符，寻找最近的‘\n’
	while(m_iIndexBuffer<m_iFileSize&&m_pBuffer[m_iIndexBuffer++]!='\n');
	//m_iIndexBuffer为全局变量

	//已到文件末尾
	if(m_iIndexBuffer>=m_iFileSize)
	{
		m_iIndexBuffer=m_iFileSize-1;
		return 0;
	}

// 进入下一行
	tempeIndex=m_iIndexBuffer;
	m_pBufferLine[0]='\0';
	
	//读取“下一行”的内容
	do 
	{
		m_pBufferLine[i++]=m_pBuffer[m_iIndexBuffer++];
	} while (m_pBuffer[m_iIndexBuffer-1]!='\n'&&i<MAX_LINE_VRML&&m_iIndexBuffer<m_iFileSize);


	m_pBufferLine[i-1]='\0';
	m_iIndexBuffer=tempeIndex;         // ？？

	return 1;
}

//读取相邻两个空格之间的字符串
int fileParser::ReadWord()
{
	/*
	m_pBufferWord[0]='\0';
	int i=0;

	
	//跳到下一个非空字符位置
	while((m_pBuffer[m_iIndexBuffer]=='\n'||
		m_pBuffer[m_iIndexBuffer]=='\t'||
		m_pBuffer[m_iIndexBuffer]=='\r'||
		m_pBuffer[m_iIndexBuffer]==' ')&&
		m_iIndexBuffer<m_iFileSize)

		m_iIndexBuffer++;

	//检查是否文件末尾（check eof)
	if(m_iIndexBuffer>=m_iFileSize)
	{
	    std::cout<<"m_iIndexBuffer is out of range!"<<std::endl;
		return 0;
	}

	do 
	{
		m_pBufferWord[i++]=m_pBuffer[m_iIndexBuffer++];
	} while (m_pBuffer[m_iIndexBuffer]=='\n'&&
		m_pBuffer[m_iIndexBuffer]=='\t'&&
		m_pBuffer[m_iIndexBuffer]=='\r'&&
		m_pBuffer[m_iIndexBuffer]==' '&&
		i<MAX_WORD_VRML&&
		m_iIndexBuffer<m_iFileSize);

	m_pBufferWord[i-1]='\0';
	return 1;
	*/
	m_pBufferWord[0] = '\0';
	int i=0;

	// Jump to next valid character
	while((m_pBuffer[m_iIndexBuffer] == '\n' || 
		m_pBuffer[m_iIndexBuffer] == '\t' || 
		m_pBuffer[m_iIndexBuffer] == '\r' || 
		m_pBuffer[m_iIndexBuffer] == ' ') &&
		m_iIndexBuffer < m_iFileSize)
		m_iIndexBuffer++;

	// Check eof
	if(m_iIndexBuffer >= m_iFileSize)
		return 0;

	do
	m_pBufferWord[i++] = m_pBuffer[m_iIndexBuffer++];
	while(m_pBuffer[m_iIndexBuffer-1] != '\n' && 
		m_pBuffer[m_iIndexBuffer-1] != '\t' && 
		m_pBuffer[m_iIndexBuffer-1] != '\r' && 
		m_pBuffer[m_iIndexBuffer-1] != ' ' && 
		i < MAX_WORD_VRML &&
		m_iIndexBuffer < m_iFileSize);

	m_pBufferWord[i-1] = '\0';

	return 1;
}

//寻找字符串：找到，m_iIndexBuffer指向第一个字符所在位置，返回1；未找到，返回0
int fileParser::OffsetToString(const char* stringFind)
{
	while(m_iIndexBuffer<m_iFileSize)
	{
		ReadLine();

	/*strstr(str1,str2)
	从字符串str1中查找是否有字符串str2，如果有，从str1中的str2位置起，返回str1中str2起始位置的指针，如果没有，返回null。
	*/
		char *adr=strstr(m_pBufferLine,stringFind);
		if(adr!=NULL)
		{
			m_iIndexBuffer=m_iIndexBuffer-strlen(m_pBufferLine)-1+(adr-m_pBufferLine);         //??   本行开始地址+字符串偏移地址
			return 1;
		}
	}
	 return 0;
}

//寻找字符串(start和end用来限定字符串的搜索位置）
int fileParser::OffsetToString(const char*stringFind,int start,int end)
{
     m_iIndexBuffer=start;
	 while(m_iIndexBuffer<m_iFileSize&&m_iIndexBuffer<=end)
	 {
		 ReadLine();
	     char * adr=strstr(m_pBufferLine,stringFind);
		 if(adr!=NULL)
		 {
			 m_iIndexBuffer=m_iIndexBuffer-strlen(m_pBufferLine)-1+(adr-m_pBufferLine);
			 return 1;
		 }
		 
	 }
	return 0;
}

