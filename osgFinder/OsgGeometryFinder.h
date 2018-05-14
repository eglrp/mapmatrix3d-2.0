#pragma once
#include "OSG_Plus.h"


class COsgGeodeFinder : public osg::NodeVisitor
{

public:
	COsgGeodeFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
	{
		isFindGeometry = false;
	}

	virtual void apply(osg::Geode& geode);

public:
	bool isFindGeometry;
};

