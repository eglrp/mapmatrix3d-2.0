#include "StdAfx.h"
#include "OsgTransNodeFinder.h"

void COsgTransNodeFinder::apply(osg::MatrixTransform& node)
{
	if (node.getName() != "repairMatrix")
	{
		osg::Matrix matrix = node.getMatrix();
		vecTrans.push_back(&node);
	}

	traverse(node);
}

void COsgTransNodeFinder::FindTransNode(osg::ref_ptr<osg::Node> node, std::vector<osg::MatrixTransform*> &vecTrans)
{
	COsgTransNodeFinder ive;
	node->accept(ive);
	vecTrans = ive.vecTrans;
}