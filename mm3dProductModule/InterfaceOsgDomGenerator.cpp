
#include "StdAfx.h"
#include "InterfaceOsgDomGenerator.h"
#include "osgDomGenerator.h"

InterfaceOsgDomGenerator* InterfaceOsgDomGeneratorFactory::create()
{
	return new osgDomGenerator();
}