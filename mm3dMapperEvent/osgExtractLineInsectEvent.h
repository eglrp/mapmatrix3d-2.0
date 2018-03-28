#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceOsgFinder.h"
#include <vector>


class COsgExtractLineInsectEvent : public CBaseUserEvent
{
public:
	COsgExtractLineInsectEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup, osg::ref_ptr<osg::Group> TempGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		this->iSigEventCallBack = iSigEventCallBack;

		tempGroup = TempGroup;
		mapperGroup = MapperGroup;

		
	}
protected:
	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	bool isActivated;
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *tempGroup;//用于临时绘图
	osg::Group *mapperGroup;//可编辑的墙面绘制在这个节点上
	InterfaceSigEventCallBack* iSigEventCallBack;

	vector < osg::Vec3d > vecCoord;
	char* transName1, *transName2;//求交线段名称
	std::vector<LineInfo> vecLineforInsect;//线段求交专用
	osg::ref_ptr<osg::MatrixTransform> selectTrans1, selectTrans2;//存储求交的两条线段
	osg::Vec3d PointInLine1, PointInLine2;//存储求交的两个端点

	osg::Matrix worldMatrix;
};