#include "StdAfx.h"
#include "InterfaceOsgFinder.h"
#include "COsgFinder.h"

InterfaceOsgFinder* InterfaceOsgFinderFactory::create()
{
	return new COsgFinder();
}

