#pragma once
#include "osgClipByScope.h"
#include "StProgressSetter.h"

class COsgClipByPolygon : public COsgClipByScope
{
protected:
	virtual void startClipModelByScope(std::vector<osg::MatrixTransform*> vecTrans, std::string headOutputFilePath, std::string inFilePath,
		std::vector<CScope> vecClipScope, CStProgressSetter stProgressSetter, bool isInside);

	virtual void showHintClipping(InterfaceSigEventCallBack* iSigEventCallBack);

	virtual void showHintFinishClipping(InterfaceSigEventCallBack* iSigEventCallBack);
};

