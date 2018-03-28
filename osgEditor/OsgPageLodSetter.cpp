#include "StdAfx.h"
#include <osg/LOD>
#include <osg/PagedLOD>
#include "OsgPageLodSetter.h"
#include "qcomm.h"


COsgPageLodSetter::COsgPageLodSetter(std::string FileName) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
	fileName = FileName;
}


COsgPageLodSetter::~COsgPageLodSetter(void)
{
}

void COsgPageLodSetter::SetLodFileName(osg::ref_ptr<osg::Node> node, std::string fileName)
{
	COsgPageLodSetter ive(fileName);
	node->accept(ive);
}

void COsgPageLodSetter::apply(osg::PagedLOD& pagedLod)
{
	pagedLod.setDatabasePath("");
	traverse(pagedLod);
}