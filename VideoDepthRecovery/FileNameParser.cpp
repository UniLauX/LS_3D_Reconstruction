#include "FileNameParser.h"


FileNameParser::FileNameParser(void)
{
}


FileNameParser::~FileNameParser(void)
{
}

std::string FileNameParser::findFileNameDir(const std::string &fileName)
{
	std::string::size_type pos1=fileName.rfind('/');    //size_type=unsigned，用于保存string或vector的长度,此处pos1用于查询'/'在字符串中的索引下标
	std::string::size_type pos2=fileName.rfind('\\');
	
	//std::cout<<pos1<<":"<<pos2<<std::endl;               
	         
	if(pos1==std::string::npos&&pos2!=std::string::npos)      //std::string::npos=4294967295,表示字符在字符串中未找到
	{
		return fileName.substr(0,pos2+1);  //截断字符串索引下标为0到pos2
	}
    
	if(pos1!=std::string::npos&&pos2==std::string::npos)
	{
	 /*
       std::string str="hello.dafdd.dadasfda.dda.";
	     str.append(1,'/');
		 str+='\\';
		 std::cout<<str<<std::endl;
	  // std::cout<<str.size()<<":"<<str.length()<<std::endl;
	  	
	  std::string::size_type pos=str.rfind('.');
	  std::string::size_type pos1=str.find_last_of('.');
	   std::cout<<str.substr(0,pos)<<std::endl;
	   std::cout<<str.substr(0,pos1)<<std::endl;
     */
        return fileName.substr(0,pos1+1);
		
	}
     
    if(pos1!=std::string::npos&&pos2!=std::string::npos)          //此种情况较少
	{
		if(pos2>pos1)
			return fileName.substr(0,pos2+1);
		else
			return fileName.substr(0,pos1+1);
	}
	return std::string();             //返回空串
}

std::string FileNameParser::findFileNameName(const std::string &fileName)
{
	std::string::size_type pos1=fileName.rfind('/');
	std::string::size_type pos2=fileName.rfind('\\');
	if(pos1==std::string::npos&&pos2!=std::string::npos)
	{
		return fileName.substr(pos2+1);
	}
	if(pos1!= std::string::npos&&pos2==std::string::npos)
	{
		return fileName.substr(pos1+1);
	}
	if(pos1!=std::string::npos&&pos2!=std::string::npos)       //C:\\A/b.txt
	{
		if(pos1>pos2)
		  return fileName.substr(pos1+1);
		else
		  return fileName.substr(pos2=1);
	}

	return fileName;
}

//得到无后缀的图像名称
std::string FileNameParser::findFileNameNameWitoutExt(const std::string &fileName)
{
    std::string::size_type pos1=fileName.rfind('/');
	std::string::size_type pos2=fileName.rfind('\\');
	std::string::size_type pos3=fileName.rfind('.');
    
    if(pos3==std::string::npos)
	{
	      pos3=fileName.size();
	}

    if(pos1==std::string::npos&&pos2!=std::string::npos)
	{
		 return fileName.substr(pos2+1,pos3-pos2-1);            //substr的另一种用法，第二个参数表示字符串中字符个数
	}

	if(pos1!=std::string::npos&&pos2==std::string::npos)
	{
		return fileName.substr(pos1+1,pos3-pos1-1);
	}

	if(pos1!=std::string::npos&&pos2!=std::string::npos)
	{
		if(pos2>pos1)
		 return fileName.substr(pos2+1,pos3-pos2-1);
		else
	     return fileName.substr(pos1+1,pos3-pos1-1);
	}

	return fileName.substr(0,pos3);
}

std::string FileNameParser::findFileNameExt(const std::string & fileName)
{
      std::string::size_type pos=fileName.find_last_of('.');     //find_last_of 与 rfind 效果相同     
	  if(pos!=std::string::npos)
	  {
		  return fileName.substr(pos+1);
	  }
	  return std::string();
}

std::string FileNameParser::findFileNamePathNameWithoutExt(const std::string &fileName)
{
	std::string::size_type pos=fileName.find_first_of('.');
	if(pos!=std::string::npos)
	{
		return fileName.substr(0,pos);
	}
	  return fileName;
}

int FileNameParser::findFileNameNumber(const std::string &fileName)
{
     std::string sNumber;
	 int i,j,length,iNumber;
     length=(int)fileName.length();   //fileName.length()=fileName.size()
	 i=length;
	 while(fileName[i--]!='.'&&i>=0);
	 j=i+1;                                    //j在‘.’的位置上

	 while(isdigit(fileName[i])&&i>=0)
		 i--;                         //回退到最开始一个数字之前
	 i++;

	 if(i==j)
	 {
		 iNumber=-1;
	 }
	    
	 else
	 {
		 sNumber=fileName.substr(i,j-i);
		 iNumber=std::stoi(sNumber.c_str());
	 }

	 return iNumber;
}


std::string FileNameParser::increaseFileNameNumber(const std::string&fileName, int incr)
{
	std::string sNumber,fileNameExt,resultFileName;
	int i,j,length,iNumber,formatWidth;
	char strBuffer[260];
	length=(int)fileName.size();
	i=length;
    fileNameExt=findFileNameExt(fileName);

	while(fileName[i--]!='.'&&i>=0);
	j=i+1;


	while(isdigit(fileName[i])&&i>=0)
		i--;
     i++;                              //数字开始的位置

	 if(i!=0)
	  resultFileName=fileName.substr(0,i);         //图像文件所在的目录名


	 formatWidth=j-i;             //数字宽度
	   if(i==j)
	   {
		   iNumber=incr;         //如果读取文件没有数字，则设置iNumber=incr
	   }
	   else
	   {
		   sNumber=fileName.substr(i,j-i);
		   iNumber=std::stoi(sNumber.c_str());
		   iNumber+=incr;
	   }

	   if(formatWidth<log10((double)iNumber))
		   formatWidth=(int)(log10((double)iNumber));  //针对从99变到100（2位变3位）及类似情况

	   switch(formatWidth)
	   {
	   case 2:
		   sprintf(strBuffer,"%.2d",iNumber);      //将iNumber转为2位字符串（格式：右对齐方式，左边补零）
	   case 3:
		   sprintf(strBuffer, "%.3d", iNumber);
		   break;
	   case 4:
		   sprintf(strBuffer, "%.4d", iNumber);
		   break;
	   case 5:
		   sprintf(strBuffer, "%.5d", iNumber);
		   break;
	   case 6:
		   sprintf(strBuffer, "%.6d", iNumber);
		   break;
	   case 7:
		   sprintf(strBuffer, "%.7d", iNumber);
		   break;
	   case 8:
		   sprintf(strBuffer, "%.8d", iNumber);
		   break;
	   case 9:
		   sprintf(strBuffer, "%.9d", iNumber);
		   break;
	   case 10:
		   sprintf(strBuffer, "%.10d",iNumber);
		   break;
	   default:
		   sprintf(strBuffer, "%d",iNumber);
		   break;
	   }

	sNumber=strBuffer;
	/*    注释代码等同于之后两行代码
	resultFileName.append(sNumber.begin(),sNumber.end());
	resultFileName.append(1,'.');                                    // 等同于 resultFileName+='.';
	resultFileName.append(fileNameExt.begin(),fileNameExt.end());
	*/
	resultFileName+=sNumber;
	resultFileName+="."+fileNameExt;
	return resultFileName;

}

