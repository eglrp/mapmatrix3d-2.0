#include "StdAfx.h"
#include "OsgTransMatrixChanger.h"

void COsgTransMatrixChanger::apply(osg::MatrixTransform& node)
{
	if (node.getName() != "repairMatrix")
	{
		osg::Matrix matrix = node.getMatrix();
		double* db = matrix.ptr();
		db[12] += xMove;
		db[13] += yMove;
		db[14] += zMove;
		node.setMatrix(matrix);
	}

	traverse(node);
}

void COsgTransMatrixChanger::ChangeTransMatrix(osg::ref_ptr<osg::Node> node, double XMove, double YMove, double ZMove)
{
	COsgTransMatrixChanger ive(XMove, YMove, ZMove);
	node->accept(ive);
}