#pragma once
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"

class COsgOutputer
{
public:
	virtual void OutputOsgb(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string outputFileName);

	virtual void ReloadScene(InterfaceSigEventCallBack* iSigEventCallBack, std::string headOutputFileName);
};

