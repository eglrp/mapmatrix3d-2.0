#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"

class COsgMeasurePtEvent : public CBaseUserEvent
{
public:
	COsgMeasurePtEvent(osgViewer::Viewer *viewer, osg::Group *root, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;

		this->iSigEventCallBack = iSigEventCallBack;
		isActivated = false;
	}

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

protected:
	std::vector<osg::Vec3d> vecWorldCoord;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group* mEditSceneGroup;

	InterfaceSigEventCallBack* iSigEventCallBack;

	bool isActivated;
};