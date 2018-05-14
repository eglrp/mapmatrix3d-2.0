#include "stdafx.h"
#include "InterfaceOsgFlat.h"
#include "osgFlat.h"

InterfaceOsgFlat* InterfaceOsgFlatFactory::create()
{
	return new COsgFlatter();
}