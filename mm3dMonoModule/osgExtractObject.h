#pragma once
#include "OSG_Plus.h"
#include "publicDefined.h"
#include "InterfaceCOsgExtractObject.h"
#include "EmptyRemover.h"
#include "Outputer.h"

class COsgExtractObject : public InterfaceCOsgExtractObject
{
public:
	//½Ó¿Ú
	virtual bool ExtractBottomObjectByScope(osg::ref_ptr<osg::Node> sceneNode, osg::ref_ptr<osg::Group> root, std::string inFilePathName, std::string outputObjectFilePathName, 
		std::vector<CScope> vecScope, osg::ref_ptr<osg::MatrixTransform> &mTrans, InterfaceSigEventCallBack* iSigEventCallBack);

	virtual bool AccurateClipBottomScene(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope, osg::ref_ptr<osg::MatrixTransform> &mTrans, bool bAccurate = true);

protected:
	virtual void StartExtractOriginalScene(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePathName, std::string outputObjectFileName, std::vector<CScope> vecScope, 
		InterfaceSigEventCallBack* iSigEventCallBack);

protected:
	COsgEmptyRemover emptyRemover;

	COsgOutputer osgOutputer;
};