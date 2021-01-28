#include "fileParser.h"


fileParser::fileParser(void)
{
	//��ʼ������
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
	infile.open(m_sFileName,std::ios::in|std::ios::binary);   //���ļ�

	if(infile.good()==false)      //good�����������Ƿ�����
	{
		std::cerr<<"unable to open the file for reading\n";
		return 0;
	}
/*
	    seekg�����Ƕ������ļ���λ������������������һ��������ƫ�������ڶ��������ǻ���ַ��
		���ڵ�һ��������������������ֵ�����ı�ʾ���ƫ�ƣ����ı�ʾ��ǰƫ�ơ����ڶ������������ǣ�
		ios����beg����ʾ�������Ŀ�ʼλ��
		ios����cur����ʾ�������ĵ�ǰλ��
		ios����end����ʾ�������Ľ���λ��
		tellg������������Ҫ�������������ص�ǰ��λָ���λ�ã�Ҳ�������������Ĵ�С��
*/
	infile.seekg(0,std::ios::end);
	m_iFileSize=infile.tellg();

	infile.seekg(0,std::ios::beg);
	
	if(m_pBuffer!=NULL)     //���
		Free();

	m_pBuffer=new char[m_iFileSize];
	if(m_pBuffer==NULL)
	{
		std::cerr<<"Insufficient memory\n";
		return 0;
	}
	if(infile.read(m_pBuffer,m_iFileSize)==false)              //���ļ��쳣
	{
		std::cerr<<"Error during reading\n";
		return 0;
	}

	 infile.close();                //�رն�����

	return 1;

}

int fileParser::ReadLine()     //��ȡһ��
{
	m_pBufferLine[0]='\0';
	int i=0;
	
	//��ȡ��������
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
	
	//�ӵ�ǰ��ʼ��������ǰ�ַ���Ѱ������ġ�\n��
	while(m_iIndexBuffer<m_iFileSize&&m_pBuffer[m_iIndexBuffer++]!='\n');
	//m_iIndexBufferΪȫ�ֱ���

	//�ѵ��ļ�ĩβ
	if(m_iIndexBuffer>=m_iFileSize)
	{
		m_iIndexBuffer=m_iFileSize-1;
		return 0;
	}

// ������һ��
	tempeIndex=m_iIndexBuffer;
	m_pBufferLine[0]='\0';
	
	//��ȡ����һ�С�������
	do 
	{
		m_pBufferLine[i++]=m_pBuffer[m_iIndexBuffer++];
	} while (m_pBuffer[m_iIndexBuffer-1]!='\n'&&i<MAX_LINE_VRML&&m_iIndexBuffer<m_iFileSize);


	m_pBufferLine[i-1]='\0';
	m_iIndexBuffer=tempeIndex;         // ����

	return 1;
}

//��ȡ���������ո�֮����ַ���
int fileParser::ReadWord()
{
	/*
	m_pBufferWord[0]='\0';
	int i=0;

	
	//������һ���ǿ��ַ�λ��
	while((m_pBuffer[m_iIndexBuffer]=='\n'||
		m_pBuffer[m_iIndexBuffer]=='\t'||
		m_pBuffer[m_iIndexBuffer]=='\r'||
		m_pBuffer[m_iIndexBuffer]==' ')&&
		m_iIndexBuffer<m_iFileSize)

		m_iIndexBuffer++;

	//����Ƿ��ļ�ĩβ��check eof)
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

//Ѱ���ַ������ҵ���m_iIndexBufferָ���һ���ַ�����λ�ã�����1��δ�ҵ�������0
int fileParser::OffsetToString(const char* stringFind)
{
	while(m_iIndexBuffer<m_iFileSize)
	{
		ReadLine();

	/*strstr(str1,str2)
	���ַ���str1�в����Ƿ����ַ���str2������У���str1�е�str2λ���𣬷���str1��str2��ʼλ�õ�ָ�룬���û�У�����null��
	*/
		char *adr=strstr(m_pBufferLine,stringFind);
		if(adr!=NULL)
		{
			m_iIndexBuffer=m_iIndexBuffer-strlen(m_pBufferLine)-1+(adr-m_pBufferLine);         //??   ���п�ʼ��ַ+�ַ���ƫ�Ƶ�ַ
			return 1;
		}
	}
	 return 0;
}

//Ѱ���ַ���(start��end�����޶��ַ���������λ�ã�
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

