#include "StdAfx.h"
#include "InterfaceBaseDrawer.h"
#include "BaseDrawer.h"

InterfaceBaseDrawer* InterfaceBaseDrawerFactory::create()
{
	return new CSceneDrawer();
}