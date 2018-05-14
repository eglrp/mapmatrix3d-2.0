#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "osgEditLine.h"

class  COsgCrossLineEvent : public CBaseUserEvent
{
public:
	COsgCrossLineEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup, osg::ref_ptr<osg::Group> TempGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		ISigEventCallBack = iSigEventCallBack;
		mapperGroup = MapperGroup;
		tempGroup = TempGroup;
	}

protected:

	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	void pickOperate(osg::Vec3d& pt);

protected:

	osgViewer::Viewer *mViewer;

	osg::Group *mRoot;

	osg::Group *mapperGroup;

	osg::Group *tempGroup;

	std::vector<osg::Vec3d> vecPoints1,vecPoints2;  //存储要连通的两条线

	std::vector<osg::Vec3d> vecPtSel;   //选择的点  限制为4个  一条线两个

	InterfaceSigEventCallBack* ISigEventCallBack;

	osg::Matrix worldMatrix;

	bool isActivated;

	size_t indexGroup1, indexGroup2;

	COsgEditLine editLine;
};