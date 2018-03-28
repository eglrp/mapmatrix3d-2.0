#pragma once
#include <osg/PagedLOD>
#include <osg/Geometry>
#include <osg/NodeVisitor>

class CNodeDepthTestSetter : public osg::NodeVisitor
{
public:
	CNodeDepthTestSetter() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{

	}

	virtual void apply(osg::Geometry& geometry);

};

