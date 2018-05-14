#include "StdAfx.h"
#include "InterfaceOsgMultiFileLinkCreator.h"
#include "osgMultiFileLinkCreator.h"

InterfaceOsgMultiFileLinkCreator* InterfaceOsgMultiFileLinkCreatorFactory::create()
{
	return new COsgMultiFileLinkCreator();
}

