#include "stdafx.h"
#include "InterfaceCOsgExtractArea.h"
#include "OsgExtractArea.h"

InterfaceCOsgExtractArea* InterfaceCOsgExtractAreaFactory::create()
{
	return new COsgExtractArea();
}