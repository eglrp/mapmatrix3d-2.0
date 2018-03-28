#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "osgEditLine.h"

class  COsgBreakoutLineEvent : public CBaseUserEvent
{
public:
	COsgBreakoutLineEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup, osg::ref_ptr<osg::Group> TempGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
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

protected:

	osgViewer::Viewer *mViewer;

	osg::Group *mRoot;

	osg::Group *mapperGroup;

	osg::Group *tempGroup;

	osg::Vec3d ptStart, ptEnd;

	std::vector<osg::Vec3d> vecBackup;//存储打断端点

	std::vector<osg::Vec3d> vecPoints;

	std::vector<osg::Vec3d> vecAddPoints;

	InterfaceSigEventCallBack* ISigEventCallBack;

	osg::Matrix worldMatrix;

	size_t indexGroup;     //选中的特征线序号

	bool isActivated;

	COsgEditLine editLine;
};