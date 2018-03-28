#include "StdAfx.h"
#include <osg/LOD>
#include <osg/PagedLOD>
#include "OsgPagedLodCenterMove.h"


COsgPagedLodCenterMove::COsgPagedLodCenterMove(double XMove, double YMove, double ZMove) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
	xMove = XMove; 
	yMove = YMove;
	zMove = ZMove;
}


COsgPagedLodCenterMove::~COsgPagedLodCenterMove(void)
{
}

void COsgPagedLodCenterMove::MoveLodCenter(osg::ref_ptr<osg::Node> node, double XMove, double YMove, double ZMove)
{
	COsgPagedLodCenterMove ive(XMove, YMove, ZMove);
	node->accept(ive);
}

void COsgPagedLodCenterMove::apply(osg::PagedLOD& pagedLod)
{
	osg::Vec3 center = pagedLod.getCenter();
	center.x() += xMove;
	center.y() += yMove;
	center.z() += zMove;
	pagedLod.setCenter(center);
	traverse(pagedLod);
}
