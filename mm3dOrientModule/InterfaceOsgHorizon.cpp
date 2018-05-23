
#include "StdAfx.h"
#include "InterfaceOsgHorizon.h"
#include "OsgHorizon.h"

InterfaceOsgHorizon* InterfaceOsgHorizonFactory::create()
{
	return new COsgHorizon();
}