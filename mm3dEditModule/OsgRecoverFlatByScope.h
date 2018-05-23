#pragma once
#include "InterfaceOsgRecoverFlatByScope.h"
#include <vector>
#include "publicDefined.h"
#include "FileSelector.h"
#include "FindTrans.h"

class COsgRecoverFlatByScope : public InterfaceOsgRecoverFlatByScope
{
public:
	virtual bool recoverFlatSceneByScope(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePath, std::vector<CScope> vecClipScope, std::vector<CScope> vecPreClipScope,
		InterfaceSigEventCallBack* iSigEventCallBack);

	virtual bool recoverFlatSingleObjectByScope(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecClipScope, std::vector<CScope> preVecClipScope, double preSceneMaxHeight,
		double preFlattenHeight);

protected:
	virtual bool startRecoverFlatModelByScope(std::vector<osg::MatrixTransform*> vecTrans, std::string headOutputFilePath);

	virtual void initRecoverFlatPara(std::string inFilePath, std::vector<CScope> vecClipScope, std::vector<CScope> vecPreClipScope, InterfaceSigEventCallBack* iSigEventCallBack,
		double sceneMaxHeight, double flattenHeight);

	virtual void recoverFlatOsgbHeader(std::vector<CScope> vecClipScope, osg::ref_ptr<osg::Node> sceneNode, std::string headOutputFileName);

	virtual void showHintRecovering(InterfaceSigEventCallBack* iSigEventCallBack);

	virtual void showHintFinishRecover(InterfaceSigEventCallBack* iSigEventCallBack);

protected:
	CFileSelector fileSelector;
	CFindTrans transFinder;

private:
	std::string inFilePath;
	std::vector<CScope> vecClipScope;
	std::vector<CScope> vecPreClipScope;
	double preSceneMaxHeight;
	double preFlattenHeight;
	InterfaceSigEventCallBack* iSigEventCallBack;
};

