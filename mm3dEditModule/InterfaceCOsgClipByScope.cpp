#include "stdafx.h"
#include "InterfaceCOsgClipByScope.h"
#include "OsgClipBySupendScope.h"
#include "OsgClipByPolygon.h"
#include "OsgHollowOutByPolygon.h"
#include "OsgAbsoluteHollowOutByPolygon.h"
#include "OsgAccurateClipByPolygon.h"
#include "OsgAccurateHollowOutByPolygon.h"
#include "OsgAccurateHollowOutBySuspendScope.h"
#include "OsgextractByScope.h"

InterfaceCOsgClipByScope* InterfaceCOsgClipByScopeFactory::create(MODE1 mode)
{
	if (mode == BY_POLYGON_1)
	{
		return new COsgClipByPolygon();
	}
	else if (mode == HOLLOW_OUT_BY_POLYGON_1)
	{
		return new COsgHollowOutByPolygon();
	}
	else if(mode == BY_SUSPEND_SCOPE_1)
	{
		return new COsgClipBySupendScope();
	}
	else if(mode == ABS_HOLLOW_OUT_BY_POLYGON_1)
	{
		return new COsgAbsoluteHollowOutByPolygon();
	}
	else if (mode == ACCURATE_CLIP_BY_POLYGON_1)
	{
		return new COsgAccurateClipByPolygon();
	}
	else if(mode == ACCURATE_HOLLOW_OUT_BY_POLYGON_1)
	{
		return new COsgAccurateHollowOutByPolygon();
	}
	else
	{
		return new COsgAccurateHollowOutBySuspendScope();
	}
}

InterfaceCOsgextractByScope* InterfaceCOsgextractByScopeFactory::create()
{
	return new COsgextractByScope();
}