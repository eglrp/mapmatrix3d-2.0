#pragma once
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"
#include "publicDefined.h"

#ifdef RECOVERFLATBYSCOPELIBDLL  
#define RECOVERFLATBYSCOPEAPI _declspec(dllexport)  
#else  
#define RECOVERFLATBYSCOPEAPI  _declspec(dllimport)  
#endif  

class RECOVERFLATBYSCOPEAPI InterfaceOsgRecoverFlatByScope
{
public:
	virtual bool recoverFlatSceneByScope(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePath, std::vector<CScope> vecClipScope, std::vector<CScope> vecPreClipScope,
		 InterfaceSigEventCallBack* iSigEventCallBack) = 0;

	virtual bool recoverFlatSingleObjectByScope(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecClipScope, std::vector<CScope> preVecClipScope, double preSceneMaxHeight,
		double preFlattenHeight) = 0;
};

class RECOVERFLATBYSCOPEAPI InterfaceOsgRecoverFlatByScopeFactory
{
public:
	InterfaceOsgRecoverFlatByScope* create();
};
