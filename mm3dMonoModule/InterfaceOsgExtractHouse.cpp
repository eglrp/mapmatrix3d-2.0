#include "stdafx.h"
#include "InterfaceOsgExtractHouse.h"
#include "osgExtractHouse.h"

InterfaceOsgExtractHouse* InterfaceOsgExtractHouseFactory::create()
{
	return new COsgExtractHouse();
}