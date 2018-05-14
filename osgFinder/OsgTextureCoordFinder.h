#pragma once
#include "OSG_Plus.h"

class COsgTextureCoordFinder : public osg::ArrayVisitor
{
public:
	COsgTextureCoordFinder() : osg::ArrayVisitor()
	{
		mTexCoorArray = new osg::Vec2Array;
	}

	//½Ó¿Ú 
	virtual osg::Vec2Array* GetTexCoorArray();

protected:
	virtual void apply(osg::Vec2Array &array);

protected:
	osg::Vec2Array* mTexCoorArray;
};

