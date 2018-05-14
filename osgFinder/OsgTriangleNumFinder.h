#pragma once
#include "OSG_Plus.h"

class COsgTriangleNumFinder : public osg::NodeVisitor
{
public:
	COsgTriangleNumFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
		numTri = 0;
	}

protected:
	virtual void apply(osg::Geode& geode);

public:
	int numTri; 

};