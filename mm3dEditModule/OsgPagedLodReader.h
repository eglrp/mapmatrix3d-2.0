#pragma once
#include "OSG_Plus.h"

class COsgPagedLodReader
{
public:
	COsgPagedLodReader(std::string InFilePath)
	{
		inFilePath = InFilePath;
	}

	virtual osg::ref_ptr<osg::Node> readPagedLodNode(osg::PagedLOD& pagedLod);

	virtual osg::ref_ptr<osg::Group> combinePagedLod(std::vector<std::string> vecPagedLodFileName);

private:
	std::string inFilePath;
};

