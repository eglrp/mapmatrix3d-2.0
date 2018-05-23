#pragma once
#include "publicDefined.h"
#include "InterfaceUserEventCallBack.h"

class OPSAPI CCallBackProgressBar
{
public:
	CCallBackProgressBar(InterfaceSigEventCallBack* ISigEventCallBack, int PosStart, int Interval)
	{
		iSigEventCallBack = ISigEventCallBack;

		posStart = PosStart;

		interval = Interval;
	}

	void SetProgress(int progress)
	{
		iSigEventCallBack->EmitSigSetProgressFun(progress);
	}
	
protected:
	InterfaceSigEventCallBack* iSigEventCallBack;

	int posStart;

	int interval;

};

