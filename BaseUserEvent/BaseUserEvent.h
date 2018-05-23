#pragma once
#include "OSG_Plus.h"
#include "publicDefined.h"
#include "operationState.h"



#ifdef USEREVENTLIBDLL  
#define USEREVENTAPI _declspec(dllexport)  
#else  
#define USEREVENTAPI  _declspec(dllimport)  
#endif  

class USEREVENTAPI CBaseUserEvent : public osgGA::GUIEventHandler 
{
public:
	CBaseUserEvent(osgViewer::Viewer* viewer)
	{
		//禁用裁剪小细节
		osg::CullStack::CullingMode cullingMode = viewer->getCamera()->getCullingMode();  
		cullingMode &= ~(osg::CullStack::SMALL_FEATURE_CULLING);  
		viewer->getCamera()->setCullingMode(cullingMode);  
	}

	virtual bool IsKeyPress(const osgGA::GUIEventAdapter &ea);														//是否按键
																													
	virtual bool IsKeyUp(const osgGA::GUIEventAdapter &ea);															//是否松开按键
																													
	virtual bool IsLeftClick(const osgGA::GUIEventAdapter &ea);														//是否左击
																													
	virtual bool IsDoubleClick(const osgGA::GUIEventAdapter &ea);													//是否双击
																													
	virtual bool IsRightClick(const osgGA::GUIEventAdapter &ea);													//是否右击
																													
	virtual bool IsMouseMove(const osgGA::GUIEventAdapter &ea);														//是否鼠标移动

	virtual CPickResult PickResult(float x, float y, osgViewer::Viewer *mViewer);									//碰撞检测

	virtual void ClearGroupResult(osg::ref_ptr<osg::Group> group);													//清除组结果

	virtual double CalLength(std::vector<osg::Vec3d> coord);														//计算长度

	virtual double CalculateDist(osg::Vec3d pt1, osg::Vec3d pt2);													//计算距离

	virtual double CalculateArea(std::vector<osg::Vec3d> coord);													//计算面积

	virtual bool isActivate(std::vector<OperatingState> vecOpState, OperatingState ops);							//判断事件是否激活

	virtual void UpdateActivatedStatus(std::vector<OperatingState> vecOpState) = 0;									//更新事件激活状态

};