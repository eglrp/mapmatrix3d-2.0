#pragma once
#include "OSG_Plus.h"
#include "publicDefined.h"

#ifdef EXTRACTAREALIBDLL  
#define EXTRACTAREAAPI _declspec(dllexport)  
#else  
#define EXTRACTAREAAPI  _declspec(dllimport)  
#endif  

class EXTRACTAREAAPI InterfaceCOsgExtractArea
{
public:
	virtual void extractArea(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePathName, std::vector<CScope> vecScope) = 0;

	virtual osg::ref_ptr<osg::Node> extractObjectByArea(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope) = 0;
};

class EXTRACTAREAAPI InterfaceCOsgExtractAreaFactory
{
public:
	InterfaceCOsgExtractArea* create();
};