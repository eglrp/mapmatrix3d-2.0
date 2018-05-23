#pragma once
#include "OSG_Plus.h"

class COsgTransMatrixMultiplier : public osg::NodeVisitor
{
public:
	COsgTransMatrixMultiplier(osg::Matrix PosMulMatrix) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
	{
		posMulMatrix = PosMulMatrix;
	}

public:
	void MultiplyTransMatrix(osg::ref_ptr<osg::Node> node,osg::Matrix PosMulMatrix);

protected:
	virtual void apply(osg::MatrixTransform& node);

private:
	osg::Matrix posMulMatrix;
};