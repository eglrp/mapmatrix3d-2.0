#include "stdafx.h"
#include "LogShower.h"
#include "qcomm.h"


void CLogShower::ShowLogFinishSelectScope()
{
	_logop("完成选择裁切范围");
}

void CLogShower::ShowLogStartOutputHalfInScopeOsgb()
{
	_logop("开始输出不完全在范围内范围");
}

void CLogShower::ShowLogFinishOutputHalfInScopeOsgb()
{
	_logop("完成输出不完全在范围内范围");
}

void CLogShower::ShowLogStartOutputTotalInScopeOsgb()
{
	_logop("开始输出完全在范围内范围");
}

void CLogShower::ShowLogFinishOutputTotalInScopeOsgb()
{
	_logop("完成输出完全在范围内范围");
}