#include "StdAfx.h"
#include "OsgPagedLodBlockNameFinder.h"


COsgPagedLodBlockNameFinder::COsgPagedLodBlockNameFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
{

}


COsgPagedLodBlockNameFinder::~COsgPagedLodBlockNameFinder()
{
}

void COsgPagedLodBlockNameFinder::FindLodBlockName(osg::ref_ptr<osg::Node> node, std::string& blockName)
{
	COsgPagedLodBlockNameFinder ive;
	node->accept(ive);
	blockName = ive.blockName;
}

void COsgPagedLodBlockNameFinder::apply(osg::PagedLOD& pagedLod)
{
	if (blockName.size() > 0)
	{
		return;
	}

	for (int i = 0; i < pagedLod.getNumFileNames(); i++)
	{
		std::string fileName = pagedLod.getFileName(i);

		if (fileName.size() > 0)
		{
			blockName = ObtainBlockName(fileName);

			return;
		}
	}
}


std::string COsgPagedLodBlockNameFinder::ObtainBlockName(std::string fileName)
{
	int pos;

	if (fileName.find("Clip") != -1)
	{
		pos = fileName.find("Clip");
	}
	else
	{
		pos = fileName.find("Tile");
	}

	std::string blockName = fileName.substr(0, pos);

	return blockName;
}
