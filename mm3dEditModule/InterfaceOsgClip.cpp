#include "StdAfx.h"
#include "InterfaceOsgClip.h"
#include "OsgClipperByPolygon.h"
#include "OsgClipperBySuspendScope.h"
#include "OsgClipperAbsoluteHollowOutByPolygon.h"
#include "OsgAccurateClipperByPolygon.h"
#include "OsgAccurateClipperBySuspendScope.h"

InterfaceOsgClip* InterfaceOsgClipFactory::create(MODE mode)
{
	if (mode == BY_POLYGON)
	{
		return new COsgClipperByPolygon();
	}
	else if(mode == BY_SUSPEND_SCOPE)
	{
		return new COsgClipperBySuspendScope();
	}
	else if(mode == BY_ABSOLUTE_POLYGON)
	{
		return new COsgClipperAbsoluteHollowOutByPolygon();
	}
	else if(mode == BY_ACCURATE_POLYGON)
	{
		return new COsgAccurateClipperByPolygon();
	}
	else
	{
		return new COsgAccurateClipperBySuspendScope();
	}
}