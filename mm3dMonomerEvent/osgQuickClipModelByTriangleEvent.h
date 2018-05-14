#pragma once

#include "BaseUserEvent.h"
#include "operationState.h"
#include <vector>
#include "InterfaceUserEventCallBack.h"

class COsgQuickClipModelByTriangleEvent : public CBaseUserEvent
{
public:
	COsgQuickClipModelByTriangleEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> EditGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		editGroup = EditGroup;
		this->iSigEventCallBack = iSigEventCallBack;
	}

	void StartQuickClipSceneByTriangle();

protected:
	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	void selectSceneByTriangle(std::vector<osg::Vec3d> vecWorldPt);

protected:
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::ref_ptr<osg::Group> editGroup;
	bool isActivated;

	std::vector<osg::Vec3d> vecWorldPt;
	InterfaceSigEventCallBack* iSigEventCallBack;
};