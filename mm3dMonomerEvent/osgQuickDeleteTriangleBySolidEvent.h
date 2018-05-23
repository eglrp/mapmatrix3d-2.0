#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include <vector>
#include "InterfaceUserEventCallBack.h"/*
#include "osgHollowOutSuspendEvent.h"


class COsgQuickDeleteSingleTriangleBySolidEvent : public COsgHollowOutSuspendEvent
{
public:
	COsgQuickDeleteSingleTriangleBySolidEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> editGroup, osg::Group* tempGroup, osg::Group* scopeGroup, InterfaceSigEventCallBack* iSigEventCallBack) : COsgHollowOutSuspendEvent(viewer, root, editGroup, tempGroup, scopeGroup, iSigEventCallBack)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		EditGroup = editGroup;
		iSigEventCallBack = iSigEventCallBack;
	}

	void StartQuickClipSceneBySolid();

protected:
	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	virtual bool ObtainScope(CScope &scope, std::string &fileName, osg::Vec3d worldPt, osg::Vec3d localPt, osg::Matrix worldMatrix, osg::NodePath np);

protected:
	std::vector<CScope> vecScope;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *EditGroup;
	bool isActivated;

	InterfaceSigEventCallBack* iSigEventCallBack;
};*/