#include "stdafx.h"
#include "OsgTextureCoordFinder.h"


osg::Vec2Array* COsgTextureCoordFinder::GetTexCoorArray()
{
	return mTexCoorArray;
}

void COsgTextureCoordFinder::apply(osg::Vec2Array &array)
{
	mTexCoorArray = &array;
}