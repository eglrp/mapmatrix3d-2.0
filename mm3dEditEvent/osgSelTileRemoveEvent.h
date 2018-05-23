#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "qcomm.h"


class COsgSelTileRemoveEvent : public CBaseUserEvent
{
public:
	COsgSelTileRemoveEvent(osgViewer::Viewer *viewer, osg::Group *root, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;

		this->iSigEventCallBack = iSigEventCallBack;
		isActivated = false;
	}

	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	bool StartSelTileRemove(osg::ref_ptr<osg::Node> sceneNode, CPickResult pResult);

	bool RemoveSelTile(osg::NodePath np, osg::ref_ptr<osg::Node> sceneNode);

	void ObtainSelRolCowFromNodePath(osg::NodePath np, int &row, int &col);

	void ObtainRowAndColFromFileName(std::string fileName, int &row, int &col);

protected:
	//É¾³ýµÄtileµÄPagedLodºÍparent
	osg::ref_ptr<osg::Node> childNode;
	osg::ref_ptr<osg::Group> parentNode;

	vector<osg::ref_ptr<osg::Node> > vecchildNode;
	vector<osg::ref_ptr<osg::Group> > vecparentNode;

	std::vector<osg::Vec3d> vecWorldCoord;
	bool isActivated;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;

	InterfaceSigEventCallBack* iSigEventCallBack;
};