//---------------------------------------------------------------------------

#ifndef QCOMM_DEFINE
#define QCOMM_DEFINE

#define MAX_LOG_LENGTH	255

#include <Sys/timeb.h>

#ifndef DEPENDENCYAPI
#define DEPENDENCYAPI __declspec(dllexport)
#endif

struct timeb _curMsTime(int flagSet=1,char*ptext = NULL);//计算离上一次调用的间隔时间，1：重新设置；2：计算间隔

int  _log(char* ptext,int level=0);//运行日志

DEPENDENCYAPI int  _logop(char* ptext,int level=0);//操作日志

void test(int &ia,int &ib);

struct timeb _getCurMillionSeconds();//获取当前系统时间（timeb 精确到毫秒）

char* _getCurrentStrTimeb(char*pca,int flag=0);//获取当前毫秒级字符串


void _getLastError();//获取系统错误信息
//---------------------------------------------------------------------------
#endif
