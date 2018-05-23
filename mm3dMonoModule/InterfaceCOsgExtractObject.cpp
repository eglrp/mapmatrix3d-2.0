#include "stdafx.h"
#include "InterfaceCOsgExtractObject.h"
#include "osgExtractObject.h"

InterfaceCOsgExtractObject* InterfaceCOsgExtractObjectFactory::create()
{
	return new COsgExtractObject();
}
