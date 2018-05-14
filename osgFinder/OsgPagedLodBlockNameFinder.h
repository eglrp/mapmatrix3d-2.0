#pragma once
#include "OSG_Plus.h"
#include "InterfaceOsgFinder.h"

class COsgPagedLodBlockNameFinder : public osg::NodeVisitor
{
public:
	COsgPagedLodBlockNameFinder();
	~COsgPagedLodBlockNameFinder();

	//½Ó¿Ú
	void FindLodBlockName(osg::ref_ptr<osg::Node> node, std::string& blockName);

public:
	std::string blockName;

protected:
	virtual void apply(osg::PagedLOD& pagedLod);

	std::string ObtainBlockName(std::string fileName);
};

