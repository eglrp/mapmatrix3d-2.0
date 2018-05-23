#include "StdAfx.h"
#include "OsgClipNodeFinder.h"


void COsgClipNodeFinder::FindClipNode(osg::ref_ptr<osg::Node> node, std::vector<osg::ClipNode*> &vecTrans)
{
	COsgClipNodeFinder ive;
	node->accept(ive);
	vecTrans = ive.vecTrans;
}

void COsgClipNodeFinder::apply(osg::ClipNode& node)
{
	vecTrans.push_back(&node);	
}