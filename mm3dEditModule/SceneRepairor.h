#pragma once
#include "OSG_Plus.h"
#include <vector>
#include "publicDefined.h"

class CSceneRepairor
{
public:
	CSceneRepairor(void);
	~CSceneRepairor(void);

	//接口
	osg::ref_ptr<osg::Node> createInterpolateRepairPatch(std::vector<osg::Vec3d> vecCoord, osg::Matrix worldMatrix, std::string imgFileName);	//创建插值补块

private:
	void InterpolatePt(osg::Geode& geode, osg::Matrix matrix, std::vector<osg::Vec3d> &vecPt, GridDem gDem, bool isRepair);						//插值点

	osg::ref_ptr<osg::Node> createRepairPatch(std::vector<osg::Vec3d> vecCoord, std::string imageFileName);										//创建修补节点

	bool ObtainGeoRect(osg::ref_ptr<osg::Node> node, osg::Matrix matrix, std::vector<osg::Vec3d> &minMaxGeode);									//获取最大范围
};





