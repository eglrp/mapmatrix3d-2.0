//---------------------------------------------------------------------------
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <io.h>
#include <Sys/stat.h>

#include "qcomm.h"


void _getLastError()//获取系统错误信息
{
	char szBuf[128] = "";
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,NULL);
	sprintf_s(szBuf,127,"GetLastError:(%ld) %s",dw,lpMsgBuf);
	LocalFree(lpMsgBuf);

	_logop(szBuf);
}

void test(int &ia,int &ib)
{

}

struct timeb _getCurMillionSeconds()
{
	struct timeb curtime;
	ftime(&curtime);
	return curtime;
}

//获取当前毫秒级字符串
char* _getCurrentStrTimeb(char*pca,int flag)
{
	struct timeb curtime = _getCurMillionSeconds();

	if(flag==0)
	{
		sprintf_s(pca,15,"%ld",curtime.time);

		char strMil[5] = "";
		sprintf_s(strMil,4,"%03d",curtime.millitm);
		strcat_s(pca,15,strMil);
	}
	else if(flag==1)
	{
		struct tm*t;
		struct tm tt;
		t = &tt;
		localtime_s(t,&curtime.time);

		sprintf_s(pca,29,"%d-%d-%d_%d-%d-%d_%03d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,
			t->tm_hour,t->tm_min,t->tm_sec,curtime.millitm);
	}

	return pca;
}

struct timeb _curMsTime(int flagSet,char*ptext)
{
	static struct timeb tm1;
	static struct timeb tm2;
	if(flagSet==1)
		ftime(&tm1);
	else
	{
		ftime(&tm2);

		char ca[MAX_LOG_LENGTH]="";
		if(ptext==NULL)
			sprintf_s(ca,MAX_LOG_LENGTH,"设置时段长度为：%.3f秒.",((tm2.time-tm1.time)*1000+(tm2.millitm-tm1.millitm))/1000.0f );
		else
			sprintf_s(ca,MAX_LOG_LENGTH,"%s 时间为：%.3f秒.",ptext,((tm2.time-tm1.time)*1000+(tm2.millitm-tm1.millitm))/1000.0f );

		_logop(ca);

		tm1 = tm2;
	}

	return tm1;
}


int  _log(char* ptext,int level)
{
	char ca[MAX_LOG_LENGTH+1]="";
	struct timeb tmm;
	ftime(&tmm);
	struct tm*t;
	struct tm tt;
	t = &tt;
	localtime_s(t,&tmm.time);

	FILE* handle = 0;
	
	//1 open or create the file;
	errno_t ret = fopen_s(&handle,"run.log","a+");
	if( ret!= 0 )
	{
		//printf("Error! When open file of run log!");
                //g_cs1.Leave();
		return -2;
	}

	//create log text;
	sprintf_s(ca,MAX_LOG_LENGTH,"\n[%d-%d-%d %d:%d:%d.%03d] ",t->tm_year+1900,t->tm_mon+1,t->tm_mday,
		t->tm_hour,t->tm_min,t->tm_sec,tmm.millitm);
	//2 write log;
	fwrite(ca,strlen(ca),1,handle);	//写入时间
	fwrite(ptext,strlen(ptext),1,handle);//写入日志内容

	fclose(handle);

	//3 if size>500K,rename and delete;
	struct stat sst;
	if(!stat("run.log",&sst))
	{

		if(sst.st_size/1024>3000)
		{
			if (_access("run.log.bak", 0) != -1)
			{
                                remove("run.log.bak");
			}

			rename("run.log","run.log.bak");
		}
	}

        //g_cs1.Leave();
        
	return 0;
}

int  _logop(char* ptext,int level)
{
	char ca[256]="";
	struct timeb tmm;
	ftime(&tmm);
	struct tm*t;
	struct tm tt;
	t = &tt;
	localtime_s(t,&tmm.time);

	FILE* handle = 0;
	
	//1 open or create the file;
	errno_t ret = fopen_s(&handle,"runop.log","a+");
	if( ret != 0 )
	{
		//printf("Error! When open file of run log!");
                //g_cs1.Leave();
		return -2;
	}

	//create log text;
	sprintf_s(ca,MAX_LOG_LENGTH,"\n[%d-%d-%d %d:%d:%d.%03d] ",t->tm_year+1900,t->tm_mon+1,t->tm_mday,
		t->tm_hour,t->tm_min,t->tm_sec,tmm.millitm);
	//2 write log;
	fwrite(ca,strlen(ca),1,handle);	
	fwrite(ptext,strlen(ptext),1,handle);

	fclose(handle);

	//3 if size>500K,rename and delete;
	struct stat sst;
	if(!stat("runop.log",&sst))
	{
		if(sst.st_size/1024>1800)//1M大小
		{
			if (_access("runop.log.bak", 0) != -1)
			{
                  remove("runop.log.bak");
			}

			rename("runop.log","runop.log.bak");
		}
	}
        
	return 0;
}