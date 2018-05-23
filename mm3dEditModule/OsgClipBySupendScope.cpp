#include "stdafx.h"
#include "OsgClipBySupendScope.h"
#include "InterfaceOsgClip.h"
#include "qcomm.h"


void COsgClipBySupendScope::startClipModelByScope(std::vector<osg::MatrixTransform*> vecTrans, std::string headOutputFilePath, std::string inFilePath,
	std::vector<CScope> vecClipScope, CStProgressSetter stProgressSetter, bool isInside)
{
	InterfaceOsgClipFactory iOsgClipFactory;
	InterfaceOsgClip* IClip = iOsgClipFactory.create(BY_SUSPEND_SCOPE);
	//裁切
	for (int i = 0; i < vecTrans.size(); i++)
	{
		osg::ref_ptr<osg::MatrixTransform> mTrans = vecTrans[i];

		std::vector<std::string> vecHalfInPolygonFileName, vecTotalInPolygonFileName;
		IClip->clipSceneByScope(mTrans, inFilePath, headOutputFilePath, vecClipScope, stProgressSetter, isInside);
	}
}


void COsgClipBySupendScope::showHintClipping(InterfaceSigEventCallBack* iSigEventCallBack)
{
	iSigEventCallBack->EmitSigShowOpsHint("正在进行悬空物裁切");
}

void COsgClipBySupendScope::showHintFinishClipping(InterfaceSigEventCallBack* iSigEventCallBack)
{
	iSigEventCallBack->EmitSigShowOpsHint("悬空物裁切完成");
}