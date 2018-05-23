#pragma once
#include "OSG_Plus.h"

#ifdef REPAIRLIBDLL  
#define REPAIRAPI _declspec(dllexport)  
#else  
#define REPAIRAPI  _declspec(dllimport)  
#endif  


class REPAIRAPI InterfaceOsgRepair
{
public:
	virtual osg::ref_ptr<osg::Node> createRepairPatch(std::vector<osg::Vec3d> vecWorldCoord, osg::Matrix worldMatrix, std::string imgFileName) = 0;  //´´½¨¿Õ¶´²¹¶¡
};

class REPAIRAPI InterfaceOsgRepairFactory
{
public:
	InterfaceOsgRepair* create();
};

