#include "StdAfx.h"
#include "OsgPagedLodFinder.h"


COsgPagedLodFinder::COsgPagedLodFinder(void)  : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
}


COsgPagedLodFinder::~COsgPagedLodFinder(void)
{
}

void COsgPagedLodFinder::FindPagedLod(osg::ref_ptr<osg::Node> node, std::vector<osg::PagedLOD*> &vecPagedLod)
{
	COsgPagedLodFinder ive;
	node->accept(ive);
	vecPagedLod = ive.vecPagedLod;
}

void COsgPagedLodFinder::apply(osg::PagedLOD& pagedLod)
{
	vecPagedLod.push_back(&pagedLod);
}
