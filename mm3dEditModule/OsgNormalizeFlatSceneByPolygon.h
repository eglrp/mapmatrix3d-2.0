#pragma once
#include "InterfaceOsgFlatByScope.h"
#include "FileSelector.h"
#include "FindTrans.h"

class COsgNormalizeFlatSceneByPolygon : public InterfaceOsgFlatByScope
{
public:
	virtual bool flatSceneByScope(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePath, std::vector<CScope> vecClipScope,
		InterfaceSigEventCallBack* iSigEventCallBack);

	virtual bool flatSingleObjectByScope(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecClipScope);

protected:
	virtual void startFlatModelByScope(std::vector<osg::MatrixTransform*> vecTrans, std::string headOutputFilePath);

	virtual void flatOsgbHeader(std::vector<CScope> vecClipScope, osg::ref_ptr<osg::Node> sceneNode, std::string headOutputFileName);

	virtual void showHintFlating(InterfaceSigEventCallBack* iSigEventCallBack);

	virtual void showHintFinishFlat(InterfaceSigEventCallBack* iSigEventCallBack);

protected:
	virtual void InitFlatPara(std::string InFilePath, std::vector<CScope> VecClipScope, InterfaceSigEventCallBack* iSigEventCallBack);

private:
	std::string inFilePath;
	std::vector<CScope> vecClipScope;
	InterfaceSigEventCallBack* iSigEventCallBack;

protected:
	CFileSelector fileSelector;

	CFindTrans transFinder;

};

