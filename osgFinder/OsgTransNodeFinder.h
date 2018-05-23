#pragma once
#include "OSG_Plus.h"
#include <vector>

class COsgTransNodeFinder : public osg::NodeVisitor
{
public:
	std::vector<osg::MatrixTransform*> vecTrans;
public:
	COsgTransNodeFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
	{

	}

	//接口
	void FindTransNode(osg::ref_ptr<osg::Node> node, std::vector<osg::MatrixTransform*> &vecTrans);		//查找trans节点

protected:
	virtual void apply(osg::MatrixTransform& node);

private:

};