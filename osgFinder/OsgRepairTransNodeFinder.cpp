#include "StdAfx.h"
#include "OsgRepairTransNodeFinder.h"

void COsgRepairTransNodeFinder::FindRepairTransNode(osg::ref_ptr<osg::Node> node, std::vector<osg::MatrixTransform*> &vecMatTrans)
{
	COsgRepairTransNodeFinder ive;
	node->accept(ive);
	vecMatTrans = ive.vecMatTrans;
}

void COsgRepairTransNodeFinder::apply(osg::MatrixTransform& node)
{
	if (node.getName() == "repairMatrix")
	{
		osg::Matrix matrix = node.getMatrix();
		vecMatTrans.push_back(&node);
	}

	traverse(node);
}