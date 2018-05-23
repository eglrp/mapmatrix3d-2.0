#include "stdafx.h"
#include "OsgAccurateClipByPolygon.h"
#include "InterfaceMFCExcute.h"
#include "InterfaceOsgClip.h"
#include "qcomm.h"

void COsgAccurateClipByPolygon::startClipModelByScope(std::vector<osg::MatrixTransform*> vecTrans, std::string headOutputFilePath, std::string inFilePath, 
	std::vector<CScope> vecClipScope, CStProgressSetter stProgressSetter, bool isInside)
{
	InterfaceOsgClipFactory iOsgClipFactory;
	InterfaceOsgClip* IClip = iOsgClipFactory.create(BY_ACCURATE_POLYGON);
	//裁切
	for (int i = 0; i < vecTrans.size(); i++)
	{
		osg::ref_ptr<osg::MatrixTransform> mTrans = vecTrans[i];
		stProgressSetter.percentStart += stProgressSetter.percentInterval * i;

		std::vector<std::string> vecHalfInPolygonFileName, vecTotalInPolygonFileName;
		IClip->clipSceneByScope(mTrans, inFilePath, headOutputFilePath, vecClipScope, stProgressSetter, isInside);
	}
}

void COsgAccurateClipByPolygon::showHintClipping(InterfaceSigEventCallBack* iSigEventCallBack)
{
	_logop("开始进行精确范围裁切");
	iSigEventCallBack->EmitSigShowOpsHint("正在进行精确范围裁切...");
}

void COsgAccurateClipByPolygon::showHintFinishClipping(InterfaceSigEventCallBack* iSigEventCallBack)
{
	iSigEventCallBack->EmitSigShowOpsHint("精确范围裁切完成");
}