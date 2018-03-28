#include "stdafx.h"
#include "InterfaceOsgExtractWall.h"
#include "osgExtractWall.h"

InterfaceOsgExtractWall* InterfaceOsgExtractWallFactory::create()
{
	return new COsgExtractWall();
}	