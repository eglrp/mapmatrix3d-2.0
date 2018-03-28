#pragma once
#include "OSG_Plus.h"
#include <vector>
#include "InterfaceOsgFinder.h"

class COsgPagedLodFinder : public osg::NodeVisitor
{
public:
	COsgPagedLodFinder(void);
	~COsgPagedLodFinder(void);

	//½Ó¿Ú
	void FindPagedLod(osg::ref_ptr<osg::Node> node, std::vector<osg::PagedLOD*> &vecPagedLod);			//²éÕÒpagedLod

public:
	std::vector<osg::PagedLOD*> vecPagedLod;

protected:
	virtual void apply(osg::PagedLOD& pagedLod);
};

