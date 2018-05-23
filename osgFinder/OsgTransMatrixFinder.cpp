#include "StdAfx.h"
#include "OsgTransMatrixFinder.h"


void COsgTransMatrixFinder::apply(osg::MatrixTransform& node)
{
	if (node.getName() != "repairMatrix")
	{
		osg::Matrix matrix = node.getMatrix();
		vecMatrix.push_back(matrix);
		vecTrans.push_back(&node);
	}

	traverse(node);
}

