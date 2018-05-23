#pragma once
#include "OSG_Plus.h"
#include "FileSelector.h"
#include "FindTrans.h"
#include "InterfaceOsgFlatByScope.h"

class COsgFlatByScope : public InterfaceOsgFlatByScope
{
public:
	virtual bool flatSceneByScope(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePath, std::vector<CScope> vecClipScope,
		InterfaceSigEventCallBack* ISigEventCallBack);
		
	virtual bool flatSingleObjectByScope(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecClipScope) = 0;

protected:
	virtual void startFlatModelByScope(std::vector<osg::MatrixTransform*> vecTrans, std::string headOutputFilePath) = 0;

	virtual void flatOsgbHeader(std::vector<CScope> vecClipScope, osg::ref_ptr<osg::Node> sceneNode, std::string headOutputFileName) = 0;

	virtual void showHintFlating(InterfaceSigEventCallBack* ISigEventCallBack) = 0;

	virtual void showHintFinishFlat(InterfaceSigEventCallBack* ISigEventCallBack) = 0;

protected:
	CFileSelector fileSelector;

	CFindTrans transFinder;
};

