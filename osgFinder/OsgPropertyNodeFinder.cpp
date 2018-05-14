#include "stdafx.h"
#include "OsgPropertyNodeFinder.h"


osg::ref_ptr<osg::Node> COsgPropertyNodeFinder::getObjectNode()
{
	return objectNode.get();
}