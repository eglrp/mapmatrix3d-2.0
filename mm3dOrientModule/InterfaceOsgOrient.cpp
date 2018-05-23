#include "StdAfx.h"
#include "InterfaceOsgOrient.h"
#include "AbsoluteOrientationor.h"

InterfaceOsgOrient* InterfaceOsgOrientFactory::create()
{
	return new CAbsoluteOrientationor();
}