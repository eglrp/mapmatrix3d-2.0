#pragma once
#include "OSG_Plus.h"
#include "InterfaceOsgFinder.h"

class COsgTransFolderNameFinder : public osg::NodeVisitor
{
public:
	COsgTransFolderNameFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{

	};

	// 接口
	void FindTransFolderName(osg::ref_ptr<osg::Node> node, std::string &folderFileName);	//查找Trans目录名

protected:
	virtual void apply(osg::PagedLOD& pagedLod);

public:
	std::string FolderFileName;
};