#include "StdAfx.h"
#include "OsgEmptyPagedLodRemover.h"
#include "OsgTriangleNumFinder.h"
#include "InterfaceOsgFinder.h"
#include "qcomm.h"

COsgEmptyPagedLodRemover::COsgEmptyPagedLodRemover() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
{

}

COsgEmptyPagedLodRemover::~COsgEmptyPagedLodRemover()
{
	
}

void COsgEmptyPagedLodRemover::apply(osg::PagedLOD& pagedLod)
{
	int numTri = 0;
	//查找三角形个数
	InterfaceOsgFinderFactory IOsgFinderFactory;
	InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();
	IOsgFinder->FindTriangleNum(&pagedLod, numTri);

	std::string fileName = pagedLod.getFileName(1);

	if (numTri == 0)
	{
		vecPagedLod.push_back(&pagedLod);
	}
}

void COsgEmptyPagedLodRemover::StartRemovePagedLod()
{
	for(int i = 0; i < vecPagedLod.size(); i++)
	{
		osg::ref_ptr<osg::Group> group = vecPagedLod[i]->getParent(0);

		if (!group)
		{
			return;
		}

		std::string fileName = vecPagedLod[i]->getFileName(1);

		group->removeChild(vecPagedLod[i]);		
	}
};

void COsgEmptyPagedLodRemover::RemoveEmtyLod(osg::ref_ptr<osg::Node> node)
{
	COsgEmptyPagedLodRemover ive;
	node->accept(ive);
	ive.StartRemovePagedLod();
}

