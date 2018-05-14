#include "StdAfx.h"
#include "InterfaceMFCExcute.h"
#include "Executor.h"

InterfaceMFCExcute* InterfaceMFCExcuteFactory::create()
{
	return new MFCExcute();
}
