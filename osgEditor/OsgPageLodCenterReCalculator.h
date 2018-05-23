#pragma once
#include <osg/NodeVisitor>
#include <osg/PagedLOD>

class CPageLodCenterReCalculator : public osg::NodeVisitor
{
public:
	CPageLodCenterReCalculator() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{

	}

protected:
	virtual void apply(osg::PagedLOD& pagedLod);

};

