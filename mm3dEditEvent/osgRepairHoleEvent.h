#pragma once

#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceOsgFinder.h"



class COsgRepairHoleEvent : public CBaseUserEvent
{
public:
	COsgRepairHoleEvent(osgViewer::Viewer *viewer, osg::Group *root, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;

		this->iSigEventCallBack = iSigEventCallBack;
		isActivated= false;
	}

	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	bool StartRepairHole();

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	bool ObtainTransNodeFromNodePath(osg::NodePath nodePath,  osg::ref_ptr<osg::MatrixTransform> &mTrans);

	void AddViewerOps(std::vector<OperatingState> vecOps);

protected:
	std::vector<osg::Vec3d> vecWorldCoord;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	bool isActivated;

	InterfaceSigEventCallBack* iSigEventCallBack;

	osg::ref_ptr<osg::MatrixTransform> mTrans;

	std::vector<osg::ref_ptr<osg::Node>> vecChildNode;
	std::vector<osg::ref_ptr<osg::Group>> vecParentNode;
	//osg::ref_ptr<osg::Node> saveRepairNode;
};
