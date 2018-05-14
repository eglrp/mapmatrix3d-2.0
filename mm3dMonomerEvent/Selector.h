#pragma once
#include "OSG_Plus.h"

class CSelector
{
public:
	virtual osg::ref_ptr<osg::Node> findSelPagedLod(osg::ref_ptr<osg::Node> node, osg::Vec3d worldPt, osg::Matrix worldMatrix);

	virtual bool isPtInScope(osg::Vec3d worldPt, osg::ref_ptr<osg::Node> node);
};

