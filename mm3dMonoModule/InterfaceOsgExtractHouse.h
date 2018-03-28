#pragma once
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"

#ifdef EXTRACTHOUSELIBDLL  
#define EXTRACTHOUSEAPI _declspec(dllexport)  
#else  
#define EXTRACTHOUSEAPI  _declspec(dllimport)  
#endif  

class EXTRACTHOUSEAPI InterfaceOsgExtractHouse
{
public:
	virtual void StartExtractHouse(osg::ref_ptr<osg::Node> node, osg::Matrix worldMatrix, double userDefineGroundHeight) = 0;

	virtual void extractHouse(osg::ref_ptr<osg::Node> sceneNode, std::vector<osg::Vec3d> vecWorldPt) = 0;
};


class EXTRACTHOUSEAPI InterfaceOsgExtractHouseFactory
{
public:
	InterfaceOsgExtractHouse* create();
};

