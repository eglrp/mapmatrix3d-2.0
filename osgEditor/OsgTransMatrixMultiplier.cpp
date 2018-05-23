#include "StdAfx.h"
#include "OsgTransMatrixMultiplier.h"

void COsgTransMatrixMultiplier::apply(osg::MatrixTransform& node)
{
	if (node.getName() != "repairMatrix")
	{
		osg::Matrix matrix = node.getMatrix();
		matrix.postMult(posMulMatrix);
		node.setMatrix(matrix);
	}

	traverse(node);
}

void COsgTransMatrixMultiplier::MultiplyTransMatrix(osg::ref_ptr<osg::Node> node,osg::Matrix PosMulMatrix)
{
	COsgTransMatrixMultiplier ive(PosMulMatrix);
	node->accept(ive);
}