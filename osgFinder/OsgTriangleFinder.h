#pragma once
#include "OSG_Plus.h"

class COsgGeodeFinder_ : public osg::NodeVisitor
{

public:
	COsgGeodeFinder_() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
	{
		isFindGeometry = false;
		 vec_ptr.clear();
	}

	virtual void apply(osg::Geode& geode)
	{
		isFindGeometry = true;                                            /////ÐÞ¸Ä³É±£´æ
		traverse(geode);
		vec_ptr.push_back(&geode);
	}

public:
	bool isFindGeometry;

	std::vector<osg::Geode*> vec_ptr;
};

