#pragma once
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"
#include "publicDefined.h"


#ifdef FLATBYSCOPELIBDLL  
#define FLATBYSCOPEAPI _declspec(dllexport)  
#else  
#define FLATBYSCOPEAPI  _declspec(dllimport)  
#endif  


class FLATBYSCOPEAPI InterfaceOsgFlatByScope
{
public:
	virtual bool flatSceneByScope(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePath, std::vector<CScope> vecClipScope, 
		InterfaceSigEventCallBack* iSigEventCallBack) = 0;

	virtual bool flatSingleObjectByScope(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecClipScope) = 0;

	
};

enum MODE { FLAT_BY_NORMALIZE, FLAT_BY_SAME_HEIGHT };

class FLATBYSCOPEAPI InterfaceOsgFlatByScopeFactory
{
public:
	InterfaceOsgFlatByScope* create(MODE mode);
};
