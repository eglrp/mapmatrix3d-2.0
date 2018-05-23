// osgExtractObject.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "osgExtractObject.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceOsgClip.h"
#include "InterfaceOsgEditor.h"
#include "InterfaceMFCExcute.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceCOsgClipByScope.h"
#include "AccurateClipper.h"


bool COsgExtractObject::ExtractBottomObjectByScope(osg::ref_ptr<osg::Node> sceneNode, osg::ref_ptr<osg::Group> root, std::string inFilePathName, std::string outputObjectFilePathName, std::vector<CScope> vecScope, osg::ref_ptr<osg::MatrixTransform> &mTrans, InterfaceSigEventCallBack* iSigEventCallBack)
{
	bool isSuccess = AccurateClipBottomScene(sceneNode, vecScope, mTrans);

	if (!isSuccess)
	{
		return false;
	}

	//清除没有三角网的geode
	emptyRemover.RemoveEmptyGeode(mTrans);

	osgOutputer.OutputOsgb(mTrans, outputObjectFilePathName);

	//裁切原来场景
	StartExtractOriginalScene(sceneNode, inFilePathName, outputObjectFilePathName, vecScope, iSigEventCallBack);

	return true;
}

bool COsgExtractObject::AccurateClipBottomScene(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope, osg::ref_ptr<osg::MatrixTransform> &mTrans, bool bAccurate)
{
	CAccurateClipper clipper(sceneNode, vecScope);
	clipper.AccurateClipBottomScene(mTrans,bAccurate);

	return true;
}

void COsgExtractObject::StartExtractOriginalScene(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePathName, std::string outputObjectFileName, std::vector<CScope> vecScope, InterfaceSigEventCallBack* iSigEventCallBack)
{
	InterfaceCOsgextractByScopeFactory iextractByScopeFactory;
	InterfaceCOsgextractByScope* IExtractor = iextractByScopeFactory.create();
	IExtractor->extractByScope(sceneNode, inFilePathName, outputObjectFileName, vecScope, false, iSigEventCallBack);
}

