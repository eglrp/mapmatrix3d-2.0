#pragma once
#include "OSG_Plus.h"
#include "publicDefined.h"
#include "CDraggerPoint.h"

#ifdef DRAWERLIBDLL  
#define DRAWERAPI _declspec(dllexport)  
#else  
#define DRAWERAPI  _declspec(dllimport)  
#endif  

class DRAWERAPI InterfaceBaseDrawer
{
public:
	virtual void BaseDrawPtOperate(osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix) = 0;											//画点

	virtual void BaseDrawLineOperate(osg::Vec3d worldPt1, osg::Vec3d worldPt2, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix) = 0;					//画线

	virtual void BaseDrawSolidOperate(std::vector<osg::Vec3d> worldCoord, double height, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix) = 0;		//画体

	virtual void BaseDrawFlatPlaneOperate(osg::Vec3d newCenter, double radius, osg::Vec4d color, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix) = 0;								//画平面

	virtual void BaseDrawLineLoopOperate(std::vector<osg::Vec3d> vecWorldCoord, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix) = 0;				//画线圈

	virtual void BaseDrawCurLineOperate(std::vector<osg::Vec3d> vecWorldCoord, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix) = 0;					//画曲线

	virtual void BaseDrawCircleLoopOperate(osg::Vec3d newCenter, double radius, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix) = 0;				//画圆圈

	virtual void DeleteVecChild(int childIndex, std::vector<osg::Vec3d> &vecGroup) = 0;																											//删除vector子节点

	virtual void BaseDrawPolygon(std::vector<osg::Vec3d> vecWorldCoord, osg::Vec4d color, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix) = 0;

	virtual void ClearGroupChild(osg::ref_ptr<osg::Group> group) = 0;	                                                                         //清除组

	virtual void AddDraggerToScene(osg::ref_ptr<osg::Group> mRoot,osg::ref_ptr<osg::Group> tempGroup,osg::ref_ptr<osg::Group> scopeGroup) = 0;
};

class DRAWERAPI InterfaceBaseDrawerFactory
{
public:
	InterfaceBaseDrawer* create();
};