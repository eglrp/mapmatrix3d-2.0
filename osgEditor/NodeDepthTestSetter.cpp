#include "stdafx.h"
#include "NodeDepthTestSetter.h"


void CNodeDepthTestSetter::apply(osg::Geometry& geometry)
{
	geometry.getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
}