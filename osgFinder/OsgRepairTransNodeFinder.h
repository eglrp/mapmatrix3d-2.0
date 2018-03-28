#pragma once
#include "OSG_Plus.h"
#include "InterfaceOsgFinder.h"


class COsgRepairTransNodeFinder : public osg::NodeVisitor
{
public:
	std::vector<osg::MatrixTransform*> vecMatTrans;

public:
	COsgRepairTransNodeFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
	{

	}

	//接口
	void FindRepairTransNode(osg::ref_ptr<osg::Node> node, std::vector<osg::MatrixTransform*> &vecMatTrans);	//查找修复节点的trans节点

protected:
	virtual void apply(osg::MatrixTransform& node);

private:

};