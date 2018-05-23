#pragma once
#include "OSG_Plus.h"

class CObjectAdder
{
public:
	virtual void addObjectToScene(osg::ref_ptr<osg::Node> sceneNode, osg::ref_ptr<osg::MatrixTransform> objectNode);
};

