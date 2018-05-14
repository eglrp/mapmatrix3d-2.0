#pragma once
#include "OSG_Plus.h"

class COsgEmptyPagedLodRemover : public osg::NodeVisitor
{
public:
	COsgEmptyPagedLodRemover();
	~COsgEmptyPagedLodRemover();
	//接口
	void RemoveEmtyLod(osg::ref_ptr<osg::Node> node);				//移除空lod

	void StartRemovePagedLod();										//开始移除

protected: 
	virtual void apply(osg::PagedLOD& pagedLod);

	std::vector<osg::PagedLOD*> vecPagedLod;
};

