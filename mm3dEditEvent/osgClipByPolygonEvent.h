#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"


class COsgClipByPolygonEvent : public CBaseUserEvent
{
public:
	COsgClipByPolygonEvent(osgViewer::Viewer *viewer, osg::Group *root, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;

		this->iSigEventCallBack = iSigEventCallBack;
		isActivated = false;
	}

	void ClipByPolygon(std::string inFilePath);

	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

protected:
	bool StartClipSceneByPolygon(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePath, std::vector<CScope> vecClipScope, bool isInside, InterfaceSigEventCallBack* iSigEventCallBack);

	bool GetClipScope(std::vector<CScope> &vecClipScope, std::vector<CScope> vecScope, std::vector<osg::Vec3d> vecWorldCoord);

protected:
	std::vector<osg::Vec3d> vecWorldCoord;
	std::vector<CScope> vecScope;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;

	InterfaceSigEventCallBack* iSigEventCallBack;
	bool isActivated;
};