#include "stdafx.h"
#include "OsgAbsoluteHollowOutByPolygon.h"
#include "InterfaceOsgClip.h"

void COsgAbsoluteHollowOutByPolygon::startClipModelByScope(std::vector<osg::MatrixTransform*> vecTrans, std::string headOutputFilePath, std::string inFilePath,
	std::vector<CScope> vecClipScope, CStProgressSetter stProgressSetter, bool isInside)
{
	for (int i = 0; i < vecTrans.size(); i++)
	{
		osg::ref_ptr<osg::MatrixTransform> mTrans = vecTrans[i];

		fileSelector.createClipOutputFolder(mTrans, headOutputFilePath);
		std::string outputFilePath = fileSelector.getClipOutputFilePath();

		InterfaceOsgClipFactory iOsgClipFactory;
		InterfaceOsgClip* iOsgClip = iOsgClipFactory.create(BY_ABSOLUTE_POLYGON);
		bool isSuccess = iOsgClip->clipSceneByScope(mTrans, inFilePath, outputFilePath, vecClipScope, stProgressSetter, isInside);
	}
}

void COsgAbsoluteHollowOutByPolygon::showHintClipping(InterfaceSigEventCallBack* iSigEventCallBack)
{
	iSigEventCallBack->EmitSigShowOpsHint("正在进行切割");
}

void COsgAbsoluteHollowOutByPolygon::showHintFinishClipping(InterfaceSigEventCallBack* iSigEventCallBack)
{
	iSigEventCallBack->EmitSigShowOpsHint("完成切割");
}