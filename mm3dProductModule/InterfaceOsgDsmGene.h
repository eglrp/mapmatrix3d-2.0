#pragma once
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"

#ifdef OSGDSMLIBDLL  
#define OSGDSMAPI _declspec(dllexport)  
#else  
#define OSGDSMAPI  _declspec(dllimport)  
#endif  

class OSGDSMAPI InterfaceOsgDsmGene
{
public:
	virtual void GenerateDSM(osg::ref_ptr<osg::Node> node, std::string strSceneFileName, double xRes, double yRes, InterfaceSigEventCallBack* ISigCallBack) = 0;	//²úÉúDSM
};

class OSGDSMAPI InterfaceOsgDsmGeneFactory
{
public:
	InterfaceOsgDsmGene* create();
};
