#include "StdAfx.h"
#include "OsgPagedLodCombiner.h"

void COsgPagedLodCombiner::CombineScene(osg::ref_ptr<osg::Node> node, std::string groupFileFolder)
{
	COsgPagedLodCombiner ive(groupFileFolder);
	node->accept(ive);
}