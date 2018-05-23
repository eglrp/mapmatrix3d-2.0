#pragma once
#include "osgSelectLineByRectangleEvent.h"
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"

class COsgSelectLineByAreaEvent : public COsgSelectLineByRectangleEvent
{
public:
	COsgSelectLineByAreaEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup, InterfaceSigEventCallBack* iSigEventCallBack) :  COsgSelectLineByRectangleEvent(viewer, root, MapperGroup, iSigEventCallBack)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		this->iSigEventCallBack = iSigEventCallBack;
		mapperGroup = MapperGroup;
	}

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	bool CtrlDoubleClickSelectLineByArea(osg::ref_ptr<osg::Group> MapperGroup, std::vector<osg::Vec3d> &VecWorldCoord, std::vector<std::vector<osg::MatrixTransform*>> &VecHighLightTrans);

protected:
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *mapperGroup;

	std::vector<std::vector<osg::MatrixTransform*>> vecHighLightTrans;

	InterfaceSigEventCallBack* iSigEventCallBack;
	std::vector<osg::Vec3d> vecWorldCoord;
	osg::Matrix worldMatrix;
	bool isActivated;

};