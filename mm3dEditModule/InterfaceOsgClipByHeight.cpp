#include "stdafx.h"
#include "InterfaceOsgClipByHeight.h"
#include "osgClipByHeight.h"

InterfaceOsgClipByHeight* InterfaceOsgClipByHeightFactory::create()
{
	return new COsgClipByHeight();
}

