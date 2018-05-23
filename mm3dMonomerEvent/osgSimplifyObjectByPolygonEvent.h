#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include <vector>
#include "InterfaceUserEventCallBack.h"
#include "publicDefined.h"

class COsgSimplifyObjectByPolygonEvent : public CBaseUserEvent
{
public:
	COsgSimplifyObjectByPolygonEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> EditGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		editGroup = EditGroup;
		iSigEventCallBack = iSigEventCallBack;
	}

	void StartSimplifyObjectByPolygon();

protected:
	osgViewer::Viewer *mViewer;

	osg::Group *mRoot;
	
	osg::Group *editGroup;
	
	bool isActivated;

	std::vector<osg::Vec3d> vecWorldPt;

	std::vector<CScope> vecScope;
	
	InterfaceSigEventCallBack* iSigEventCallBack;

protected:
	virtual void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	virtual void AccurateClipTriangleByPolygon(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope, osg::Matrix worldMatrix);

	virtual bool GetClipScope(std::vector<CScope> &vecClipScope, std::vector<CScope> vecScope, std::vector<osg::Vec3d> vecWorldCoord);
};


