#include "StdAfx.h"
#include "InterfaceOpencvDependency.h"
#include "OpencvDependency.h"

InterfaceOpencvDependeny* InterfaceOpencvDependenyFactory::create()
{
	return new OpencvDependency();
}