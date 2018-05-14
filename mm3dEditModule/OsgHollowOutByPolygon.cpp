#include "stdafx.h"
#include "OsgHollowOutByPolygon.h"
#include "InterfaceMFCExcute.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceOsgClip.h"
#include "qcomm.h"
#include "InterfaceUserEventCallBack.h"

void COsgHollowOutByPolygon::startClipModelByScope(std::vector<osg::MatrixTransform*> vecTrans, std::string headOutputFilePath,  std::string inFilePath, 
	std::vector<CScope> vecClipScope, CStProgressSetter stProgressSetter, bool isInside)
{
	for (int i = 0; i < vecTrans.size(); i++)
	{
		osg::ref_ptr<osg::MatrixTransform> mTrans = vecTrans[i];

		fileSelector.createClipOutputFolder(mTrans, headOutputFilePath);
		std::string outputFilePath = fileSelector.getClipOutputFilePath();

		stProgressSetter.percentStart += stProgressSetter.percentInterval * i;
		InterfaceOsgClipFactory iOsgClipFactory;
		InterfaceOsgClip* iOsgClip = iOsgClipFactory.create(BY_POLYGON);
		iOsgClip->clipSceneByScope(mTrans, inFilePath, outputFilePath, vecClipScope, stProgressSetter, isInside);
	}
}

void COsgHollowOutByPolygon::showHintClipping(InterfaceSigEventCallBack* iSigEventCallBack)
{
	_logop("开始进行挖空");
	iSigEventCallBack->EmitSigShowOpsHint("正在进行挖空...");
}

void COsgHollowOutByPolygon::showHintFinishClipping(InterfaceSigEventCallBack* iSigEventCallBack)
{
	iSigEventCallBack->EmitSigShowOpsHint("挖空完成");
}