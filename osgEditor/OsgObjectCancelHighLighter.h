#pragma once
#include<osg/Geode>
#include<osg/NodeVisitor>
#include<osg/Geometry>

class COsgObjectCancelHighLighter : public osg::NodeVisitor
{
public:
	COsgObjectCancelHighLighter() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{

	}
	
protected:
	virtual void apply(osg::Geode& geode);

	virtual void apply(osg::Geometry* geom);
};

