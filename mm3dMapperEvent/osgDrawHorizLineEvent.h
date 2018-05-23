#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceOsgFinder.h"
#include <vector>


class COsgDrawHorizLineEvent : public CBaseUserEvent
{
public:
	COsgDrawHorizLineEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup, osg::ref_ptr<osg::Group> TempGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
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
	osg::Matrix worldMatrix;
	InterfaceSigEventCallBack* iSigEventCallBack;

	vector<osg::Vec3d> vecCoord;
	std::vector<LineInfo> vecLine;//存储mapperGroup中绘制的直线
	std::vector<char*> vecName;//保存线段名字

	double horiz_high;
	bool islineStart;
};
