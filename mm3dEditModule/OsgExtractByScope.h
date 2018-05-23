#pragma once
#include "osgClipByScope.h"

class COsgextractByScope : public InterfaceCOsgextractByScope
{
public:
	virtual bool extractByScope(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePathName, std::string outputObjectFileName, std::vector<CScope> vecClipScope,
		bool isInside, InterfaceSigEventCallBack* iSigEventCallBack);

//protected:
//	virtual void showHintClipping(InterfaceSigEventCallBack* iSigEventCallBack);
//
//	virtual void showHintFinishClipping(InterfaceSigEventCallBack* iSigEventCallBack);
//
//	virtual void startClipModelByScope(std::vector<osg::MatrixTransform*> vecTrans, std::string headOutputFilePath, std::string inFilePath,
//		std::vector<CScope> vecClipScope, CStProgressSetter stProgressSetter, bool isInside);
//
//
//	virtual bool ClipByScope(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string inFilePath, std::string outputFilePath, std::vector<CScope> vecScope,
//		CStProgressSetter stProgressSetter, bool isInside);
//
//	virtual void HollowOutOsgbHeader(std::vector<CScope> vecClipScope, osg::ref_ptr<osg::Node> sceneNode, std::string headOutputFileName, 
//		std::string outputObjectFileName, bool isInside);
//
//	virtual bool AddObjectInCopyNode(std::string outputObjectFileName, osg::ref_ptr<osg::Node> copyNode);
//
//protected:
//	CFileSelector fileSelector;
//
//	CFindTrans transFinder;
};

