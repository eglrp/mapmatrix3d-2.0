#pragma once
#include "qcomm.h"
#include "OSG_Plus.h"
#include <string>

class COsgPagedLodCombiner : public osg::NodeVisitor
{
public:
	COsgPagedLodCombiner(std::string GroupFileFolder) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
	{
		groupFileFolder = GroupFileFolder;
	}

	//接口
	void CombineScene(osg::ref_ptr<osg::Node> node, std::string groupFileFolder);					//合并场景

protected:
	virtual void apply(osg::PagedLOD& pagedLod)
	{
		std::string fileName = pagedLod.getFileName(1);
		std::string combineChildFileName = '.' + groupFileFolder + fileName;

		pagedLod.setFileName(1, combineChildFileName);
		traverse(pagedLod);
	}

private:
	std::string groupFileFolder;
};

