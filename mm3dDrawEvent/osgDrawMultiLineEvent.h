#pragma once

#include "InterfaceBaseDrawer.h"
#include "BaseUserEvent.h"
#include "operationState.h"


class COsgDrawMultiLineEvent : virtual public CBaseUserEvent
{
public:
	COsgDrawMultiLineEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editSceneGroup) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;

		InterfaceBaseDrawerFactory IBaseDrawerFactory;
		IBaseDrawer = IBaseDrawerFactory.create();
		mEditSceneGroup = editSceneGroup;

		isActivated = false;
	}

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	virtual void LeftClickDrawMultiLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix);

	virtual bool MouseMoveDrawMultiLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix);

	virtual void DoubleClickDrawMultiLineOperate(std::vector<osg::Vec3d> &vecWorldCoord);

	virtual void BackSpaceReverseDrawMultiLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::ref_ptr<osg::Group> drawGroup);

	virtual void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

protected:
	std::vector<osg::Vec3d> vecWorldCoord;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group* mEditSceneGroup;

	InterfaceBaseDrawer* IBaseDrawer;

	bool isActivated;
};