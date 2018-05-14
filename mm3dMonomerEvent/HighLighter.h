#pragma once
#include "OSG_Plus.h"

class CHighLighter
{
public:
	virtual bool StartHighLight(osg::ref_ptr<osg::Node> node);

	virtual bool highLightByColor(osg::ref_ptr<osg::Node> node);

	virtual bool highLightPagedLod(osg::ref_ptr<osg::Node> node);

	virtual bool highLightCartoon(osg::ref_ptr<osg::Node> node);

	virtual void CancelHighLight(osg::ref_ptr<osg::Node> node);

	virtual bool cancelHighLightByColor(osg::ref_ptr<osg::Node> node);

	virtual void CancelAllHighLight(osg::ref_ptr<osg::Node> node);

	virtual void CancelHighLightPagedLod(osg::ref_ptr<osg::Node> node);

	virtual void CancelAllHighLightPagedLod(osg::ref_ptr<osg::Node> node);

};

