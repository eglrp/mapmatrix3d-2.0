#pragma once
#include "OSG_Plus.h"
#include "publicDefined.h"
#include "InterfaceUserEventCallBack.h"

#ifdef CLIPBYSCOPELIBDLL  
#define CLIPBYSCOPEAPI _declspec(dllexport)  
#else  
#define CLIPBYSCOPEAPI  _declspec(dllimport)  
#endif  


class CLIPBYSCOPEAPI InterfaceCOsgClipByScope
{
public:
	virtual bool hollowOutByScope(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePath, std::vector<CScope> vecClipScope, bool isInside, 
		InterfaceSigEventCallBack* iSigEventCallBack, bool filePathCheck = true) = 0;
};

class CLIPBYSCOPEAPI InterfaceCOsgextractByScope
{
public:
	virtual bool extractByScope(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePathName, std::string outputObjectFileName, 
		std::vector<CScope> vecClipScope, bool isInside, InterfaceSigEventCallBack* iSigEventCallBack) = 0;
};

enum MODE1 {BY_POLYGON_1, BY_SUSPEND_SCOPE_1, HOLLOW_OUT_BY_POLYGON_1, ABS_HOLLOW_OUT_BY_POLYGON_1, ACCURATE_CLIP_BY_POLYGON_1,
	ACCURATE_HOLLOW_OUT_BY_POLYGON_1, ACCURATE_HOLLOW_OUT_BY_SUSPEND_SCOPE_1};

class CLIPBYSCOPEAPI InterfaceCOsgClipByScopeFactory
{
public:
	InterfaceCOsgClipByScope* create(MODE1 mode);

};

class CLIPBYSCOPEAPI InterfaceCOsgextractByScopeFactory
{
public:
	InterfaceCOsgextractByScope* create();
};