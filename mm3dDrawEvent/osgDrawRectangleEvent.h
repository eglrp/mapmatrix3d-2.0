#pragma once
#include "osgDrawSinglePtHandler.h"
#include "InterfaceBaseDrawer.h"
#include "operationState.h"

class COsgDrawRectangleEvent : public COsgDrawSinglePtHandler
{
public:
	COsgDrawRectangleEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> editSceneGroup) : COsgDrawSinglePtHandler(viewer, root, editSceneGroup), CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		mEditSceneGroup = editSceneGroup;
		InterfaceBaseDrawerFactory IBaseDrawerFactory;
		IBaseDrawer = IBaseDrawerFactory.create();

		isActivated = false;
		isFirstPoint = true;
	}

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	virtual void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	virtual void DragDrawRectangleOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix, osg::Matrix viewMatrix);

	virtual void BackSpaceReverseDrawRectangleOperate(osg::ref_ptr<osg::Group> drawGroup);

protected:
	std::vector<osg::Vec3d> vecWorldCoord;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::ref_ptr<osg::Group> mEditSceneGroup;

	InterfaceBaseDrawer* IBaseDrawer;

	bool isActivated;
	bool isFirstPoint;
};



