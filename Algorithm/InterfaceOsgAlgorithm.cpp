#include "StdAfx.h"
#include "InterfaceOsgAlgorithm.h"
#include "osgAlgorithm.h"

InterfaceOsgAlgorithm* InterfaceOsgAlgorithmFactory::create()
{
	return new osgAlgorithm();
}