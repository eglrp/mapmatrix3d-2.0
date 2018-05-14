#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"


#ifdef SHOWCOORDEVENTLIBDLL  
#define SHOWCOORDEVENTAPI _declspec(dllexport)  
#else  
#define SHOWCOORDEVENTAPI  _declspec(dllimport)  
#endif  

class SHOWCOORDEVENTAPI COsgShowCoordEvent : public CBaseUserEvent
{
public:

	COsgShowCoordEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* textgroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		textGroup = textgroup;
		iSigEventCallBack = iSigEventCallBack;
		isActivated = false;
	}

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

protected:
	std::vector<osg::Vec3d> vecWorldCoord;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group* textGroup;

	InterfaceSigEventCallBack* iSigEventCallBack;

	bool isActivated;
};