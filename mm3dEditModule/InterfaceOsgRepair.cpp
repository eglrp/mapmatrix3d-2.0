#include "StdAfx.h"
#include "InterfaceOsgRepair.h"
#include "osgRepair.h"

InterfaceOsgRepair* InterfaceOsgRepairFactory::create()
{
	return new osgRepair();
}