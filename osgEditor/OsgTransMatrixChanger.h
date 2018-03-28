#pragma once
#include "OSG_Plus.h"

class COsgTransMatrixChanger : public osg::NodeVisitor
{
public:
	double xMove;
	double yMove;
	double zMove;

public:
	COsgTransMatrixChanger(double XMove, double YMove, double ZMove) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
		xMove = XMove;
		yMove = YMove;
		zMove = ZMove;
	}

	//接口
	void ChangeTransMatrix(osg::ref_ptr<osg::Node> node, double XMove, double YMove, double ZMove);		//改变节点四元素矩阵

protected:
	virtual void apply(osg::MatrixTransform& node);
};

