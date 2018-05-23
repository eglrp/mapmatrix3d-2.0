#pragma once
#include "InterfaceUserEventCallBack.h"
#include "publicDefined.h"

class OPSAPI CStProgressSetter
{
public:
	CStProgressSetter(InterfaceSigEventCallBack* iSigEventCallBack, int PercentStart, int PercentInterval)
	{
		this->iSigEventCallBack = iSigEventCallBack;
		percentStart = PercentStart;
		percentInterval = PercentInterval;

	}

	CStProgressSetter(const CStProgressSetter& StProgressSetter)
	{
		percentStart = StProgressSetter.percentStart;
		percentInterval = StProgressSetter.percentInterval;
		this->iSigEventCallBack = StProgressSetter.iSigEventCallBack;
	}

	CStProgressSetter()
	{

	}

	virtual void SetProgress(int currentNum, int totalNum);

public:
	InterfaceSigEventCallBack* iSigEventCallBack;
	int percentStart;
	int percentInterval;
};

