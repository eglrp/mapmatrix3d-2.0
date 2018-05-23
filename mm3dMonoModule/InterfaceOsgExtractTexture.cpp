#include "StdAfx.h"
#include "InterfaceOsgExtractTexture.h"
#include "osgExtractTexture.h"


InterfaceOsgExtractTexture* InterfaceOsgExtractTextureFactory::create()
{
	return new COsgExtractTexture();
}