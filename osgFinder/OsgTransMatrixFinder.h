#pragma once
#include "OSG_Plus.h"
#include <vector>

class COsgTransMatrixFinder : public osg::NodeVisitor
{
public:
	std::vector<osg::Matrix> vecMatrix;
	std::vector<osg::MatrixTransform*> vecTrans;
public:
	COsgTransMatrixFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
	{

	}

protected:
	virtual void apply(osg::MatrixTransform& node);

private:

};