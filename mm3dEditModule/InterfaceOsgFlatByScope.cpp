#include "stdafx.h"
#include "InterfaceOsgFlatByScope.h"
#include "OsgFlatByPolygon.h"
#include "OsgNormalizeFlatSceneByPolygon.h"

InterfaceOsgFlatByScope* InterfaceOsgFlatByScopeFactory::create(MODE mode)
{
	if (mode == FLAT_BY_NORMALIZE)
	{
		return new COsgNormalizeFlatSceneByPolygon();
	}
	else if (mode == FLAT_BY_SAME_HEIGHT)
	{
		return new COsgFlatByPolygon();
	}
}
