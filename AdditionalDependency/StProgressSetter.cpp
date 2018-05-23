#include "stdafx.h"
#include "StProgressSetter.h"


void CStProgressSetter::SetProgress(int currentNum, int totalNum)
{
	int percent = (int)((double)(currentNum) / (double)totalNum * percentInterval) + percentStart;

	if (iSigEventCallBack == NULL)
	{
		return;
	}

	iSigEventCallBack->EmitSigSetProgressFun(percent);
}

