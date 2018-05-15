#pragma once
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceCOsgClipByScope.h"
#include "StProgressSetter.h"
#include "FileSelector.h"
#include "FindTrans.h"

class COsgClipByScope : public InterfaceCOsgClipByScope
{
public:
	virtual bool hollowOutByScope(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePath, std::vector<CScope> vecClipScope, bool isInside, 
		InterfaceSigEventCallBack* iSigEventCallBack, bool filePathCheck = true);

protected:
	virtual void showHintClipping(InterfaceSigEventCallBack* iSigEventCallBack) = 0;
	
	virtual void showHintFinishClipping(InterfaceSigEventCallBack* iSigEventCallBack) = 0;

	virtual void startClipModelByScope(std::vector<osg::MatrixTransform*> vecTrans, std::string headOutputFilePath, std::string inFilePath, 
		std::vector<CScope> vecClipScope, CStProgressSetter stProgressSetter, bool isInside) = 0;																		//ÐèÒªÖØÐ´¸²¸Ç 

	virtual void HollowOutOsgbHeader(std::vector<CScope> vecClipScope, osg::ref_ptr<osg::Node> sceneNode, std::string headOutputFileName, bool isInside);

protected:
	CFileSelector fileSelector;

	CFindTrans transFinder;
};

