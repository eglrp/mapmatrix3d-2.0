#pragma once
#include "OSG_Plus.h"
#include "InterfaceOsgFinder.h"
#include <vector>

class COsgClipNodeFinder : public osg::NodeVisitor
{
public:
	std::vector<osg::ClipNode*> vecTrans;

	//½Ó¿Ú
	void FindClipNode(osg::ref_ptr<osg::Node> node, std::vector<osg::ClipNode*> &vecTrans);

protected:
	virtual void apply(osg::ClipNode& node);
};

