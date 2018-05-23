#include "stdafx.h"
#include "InterfaceOsgRecoverFlatByScope.h"
#include "OsgRecoverFlatByScope.h"

InterfaceOsgRecoverFlatByScope* InterfaceOsgRecoverFlatByScopeFactory::create()
{
	return new COsgRecoverFlatByScope();
}

