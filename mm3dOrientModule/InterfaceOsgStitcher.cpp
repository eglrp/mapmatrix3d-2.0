#include "StdAfx.h"
#include "InterfaceOsgStitcher.h"
#include "osgStitcher.h"

InterfaceOsgStitcher* InterfaceOsgStitcherFactory::create()
{
	return new COsgStitcher();
}