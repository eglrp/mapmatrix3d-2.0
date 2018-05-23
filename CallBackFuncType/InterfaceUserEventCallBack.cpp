#include "StdAfx.h"
#include "InterfaceUserEventCallBack.h"
#include "SigEventCallBack.h"



InterfaceSigEventCallBack* InterfaceSigEventCallBackFactory::create()
{
	return new SigEventCallBack();
}