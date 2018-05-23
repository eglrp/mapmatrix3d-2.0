#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include <vector>
#include "InterfaceUserEventCallBack.h"


class COsgQuickDeleteSingleTriangleByLineEvent : public CBaseUserEvent
{
public:
	COsgQuickDeleteSingleTriangleByLineEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> EditGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		editGroup = EditGroup;
		iSigEventCallBack = iSigEventCallBack;
	}

protected:
	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	void StartQuickDeleteByLine(osg::ref_ptr<osg::Node> node, osg::Matrix worldMatrix, osg::Vec3d lineSegPt1, osg::Vec3d lineSegPt2);

	bool LineInTriangle(osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec3d& t1, osg::Vec3d& t2, osg::Vec3d& t3);

protected:
	std::vector<osg::Vec3d> vecWorldCoord;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *editGroup;
	bool isActivated;

	InterfaceSigEventCallBack* iSigEventCallBack;
};