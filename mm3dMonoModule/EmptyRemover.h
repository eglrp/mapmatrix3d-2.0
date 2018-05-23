#pragma once
#include "OSG_Plus.h"

class COsgEmptyRemover
{
public:
	virtual void RemoveEmptyGeode(osg::ref_ptr<osg::MatrixTransform> mTrans);

};

