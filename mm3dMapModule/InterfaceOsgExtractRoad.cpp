#include "stdafx.h"
#include "InterfaceOsgExtractRoad.h"
#include "osgExtractRoad.h"

InterfaceOsgExtractRoad* InterfaceOsgExtractRoadFactory::create()
{
	return new COsgExtractRoad;
}