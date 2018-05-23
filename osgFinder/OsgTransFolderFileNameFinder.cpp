#include "StdAfx.h"
#include "OsgTransFolderFileNameFinder.h"

void COsgTransFolderNameFinder::apply(osg::PagedLOD& pagedLod)
{
	std::string fileName = pagedLod.getFileName(1);
	int pos = fileName.find('\\') + 1;
	int rPos = fileName.rfind('\\');
	int length = rPos - pos;
	std::string childFileName = fileName.substr(pos, length);
	rPos = childFileName.rfind('\\');
	length = rPos - 0;
	FolderFileName = childFileName.substr(0, length);
	return;
}

void COsgTransFolderNameFinder::FindTransFolderName(osg::ref_ptr<osg::Node> node, std::string &folderFileName)
{
	COsgTransFolderNameFinder ive;
	node->accept(ive);
	folderFileName = ive.FolderFileName;
}