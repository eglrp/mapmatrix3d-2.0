#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include <vector>
#include "InterfaceUserEventCallBack.h"


class COsgFlatSceneByPolygonEvent : public CBaseUserEvent
{
public:
	COsgFlatSceneByPolygonEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> EditGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		editGroup = EditGroup;
		this->iSigEventCallBack = iSigEventCallBack;
	}

	virtual void FlatByPolygon(std::string inFilePath);

protected:
	virtual bool StartFlatSceneByPolygon(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePath, std::vector<CScope> vecClipScope, bool isInside, InterfaceSigEventCallBack* iSigEventCallBack);

	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	virtual void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	virtual bool GetClipScope(std::vector<CScope> &vecClipScope, std::vector<CScope> vecScope, std::vector<osg::Vec3d> vecWorldCoord);

private:
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *editGroup;
	bool isActivated;
	std::vector<osg::Vec3d> vecWorldCoord;
	std::vector<CScope> vecScope;
	InterfaceSigEventCallBack* iSigEventCallBack;

	std::string inFilePath;
	std::vector<CScope> vecPreScope;

};