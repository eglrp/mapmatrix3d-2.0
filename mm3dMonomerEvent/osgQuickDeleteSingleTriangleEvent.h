#pragma once


#include "BaseUserEvent.h"
#include "operationState.h"
#include <vector>
#include "InterfaceUserEventCallBack.h"


class COsgQuickDeleteSingleTriangleEvent : public CBaseUserEvent
{
public:
	COsgQuickDeleteSingleTriangleEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> EditGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
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

	void StartQuickDelete(osg::ref_ptr<osg::Node> node, osg::Matrix worldMatrix, osg::Vec3d worldPt);

	bool PtInTriangle(osg::Vec3d pt, osg::Vec3d& t1, osg::Vec3d& t2, osg::Vec3d& t3);

	double getAngleForVector(osg::Vec3d& v1, osg::Vec3d& v2);

protected:
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *editGroup;
	bool isActivated;

	InterfaceSigEventCallBack* iSigEventCallBack;
};