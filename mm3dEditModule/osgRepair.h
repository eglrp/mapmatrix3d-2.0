#pragma once
#include "SceneRepairor.h"
#include "InterfaceOsgRepair.h"

class osgRepair : public InterfaceOsgRepair
{
public:
	osg::ref_ptr<osg::Node> createRepairPatch(std::vector<osg::Vec3d> vecWorldCoord, osg::Matrix worldMatrix, std::string imgFileName);  //´´½¨¿Õ¶´²¹¶¡
};

