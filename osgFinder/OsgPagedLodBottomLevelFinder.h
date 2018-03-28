#pragma once
#include "OSG_Plus.h"
#include "InterfaceOsgFinder.h"

class COsgPagedLodBottomLevelFinder : public osg::NodeVisitor
{
public:
	COsgPagedLodBottomLevelFinder(std::vector<std::string> *VecBottomLevelPagedLodFileName);
	~COsgPagedLodBottomLevelFinder(void);

protected:
	virtual void apply(osg::PagedLOD& pagedLod);

public:
	std::vector<std::string>* vecBottomLevelPagedLodFileName;
};

	