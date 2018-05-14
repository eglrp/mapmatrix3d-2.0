#include "StdAfx.h"
#include "osgRepair.h"
#include "SceneRepairor.h"


osg::ref_ptr<osg::Node> osgRepair::createRepairPatch(std::vector<osg::Vec3d> vecWorldCoord, osg::Matrix worldMatrix, std::string imgFileName)
{
	CSceneRepairor repairor;
	osg::ref_ptr<osg::Node> node = repairor.createInterpolateRepairPatch(vecWorldCoord, worldMatrix, imgFileName);

	return node;
}
