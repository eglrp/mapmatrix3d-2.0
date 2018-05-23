#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"



class COsgMeasureAreaEvent : public CBaseUserEvent
{
public:
	COsgMeasureAreaEvent(osgViewer::Viewer *viewer, osg::Group *root, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		
		this->iSigEventCallBack = iSigEventCallBack;
		isActivated = false;
	}

	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

protected:
	std::vector<osg::Vec3d> vecWorldCoord;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group* mEditSceneGroup;

	InterfaceSigEventCallBack* iSigEventCallBack;

	bool isActivated;
};