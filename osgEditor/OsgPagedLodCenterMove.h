#pragma once
#include "OSG_Plus.h"

class COsgPagedLodCenterMove : public osg::NodeVisitor
{
public:
	COsgPagedLodCenterMove(double XMove, double YMove, double ZMove);
	~COsgPagedLodCenterMove(void);
	//½Ó¿Ú
	void MoveLodCenter(osg::ref_ptr<osg::Node> node, double XMove, double YMove, double ZMove);

private:
	virtual void COsgPagedLodCenterMove::apply(osg::PagedLOD& pagedLod);

private:
	double xMove;
	double yMove;
	double zMove;
};

