//#include "stdafx.h"
#include "PossionShExec.h"
#include <cstdio>
#include <atlstr.h>

#include <iostream>


bool  PossionShExec(std::string possionPath,std::string mNpts,std::string Plys, float samples, int depth ,int solverDivide)
{
	std::cout<<"access the doPossion Func"<<std::endl;
	const int argnum = 13;

	TCHAR* argv[argnum];

	std::string argvinfor[argnum];
	possionPath = "\"" +  possionPath+ "\"";
	argvinfor[0] = possionPath;
	argvinfor[1] = "--in";
	argvinfor[2] =  "\"" + mNpts + "\"";
	argvinfor[3] = "--out" ;
	argvinfor[4] =  "\"" + Plys + "\"";

	argvinfor[5] = "--depth";
	static TCHAR tmpDepth[4];
	sprintf(tmpDepth, "%d", depth);
	argvinfor[6] = tmpDepth;	

	argvinfor[7] = "--solverDivide";
	static TCHAR tmpSolverDivide[4];
	sprintf(tmpSolverDivide, "%d", solverDivide);
	argvinfor[8] = tmpSolverDivide;

	argvinfor[9] = "--samplesPerNode";
	static TCHAR tmpSample[4];
	sprintf(tmpSample, "%.1f", samples);
	argvinfor[10] = tmpSample;

	argvinfor[11] = "--verbose";

	argvinfor[12] = "--confidence";

	//argvinfor[13] = _T("--threads");
	//static TCHAR tmpTest[4];
	//_stprintf(tmpTest, _T("%d"), 2);
	//argvinfor[14] = tmpTest;

	for(int i = 0;i<argnum;i++)
	{
		argv[i] = new TCHAR [ argvinfor[i].length() + 2 ];
		int j;
		for(j = 0;j<(int)argvinfor[i].size();j++)
			argv[i][j] = argvinfor[i][j];
		argv[i][j] = 0;
	}

	std::string params;
	for(int i = 1;i<argnum;i++)
		params += argvinfor[ i ]  + _T(" ");

	CString  paramList( params.c_str() );
	/*
	STARTUPINFO si = {sizeof(si)};
	PROCESS_INFORMATION pi;
	CreateProcess("D:\\work\\PoissonRecon.exe",paramList.GetBuffer(paramList.GetLength()),
								NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
	WaitForSingleObject(pi.hProcess,INFINITE); 
	*/

	 //以下程序实现调用一个exe，并且在exe结束之后，才继续执行其他的程序
	CString  possionPathCStr = possionPath.c_str();
	SHELLEXECUTEINFO  ShExecInfo  =  {0};  
	ShExecInfo.cbSize  =  sizeof(SHELLEXECUTEINFO);  
	ShExecInfo.fMask  =  SEE_MASK_NOCLOSEPROCESS;  
	ShExecInfo.hwnd  =  NULL;  
	ShExecInfo.lpVerb  =  _T("open");  
	ShExecInfo.lpFile  =  possionPathCStr;   //换成要用的exe                        
	ShExecInfo.lpParameters=paramList;            
	ShExecInfo.lpDirectory  =  NULL;  
	ShExecInfo.nShow  =  SW_SHOWNORMAL;  
	ShExecInfo.hInstApp  =  NULL;              
	ShellExecuteEx(&ShExecInfo);  
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);

	for(int i = 0;i<argnum;i++)
		delete argv[i];

	printf("Finish the doPossion Func\n");
	
	return (int)ShExecInfo.hProcess>32;
}
