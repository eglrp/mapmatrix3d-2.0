#pragma once
#include "InterfaceBaseDrawer.h"
#include "osgDrawMultiLineEvent.h"
#include "operationState.h"

class COsgDrawClosedLineEvent : public COsgDrawMultiLineEvent
{
public:
	COsgDrawClosedLineEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editSceneGroup, osg::Group* tempGroup) : COsgDrawMultiLineEvent(viewer, root, editSceneGroup), CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		mEditSceneGroup = editSceneGroup;
		mTempGroup = tempGroup;
		InterfaceBaseDrawerFactory IBaseDrawerFactory;
		IBaseDrawer = IBaseDrawerFactory.create();

		isActivated = false;
	}

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
	 
	virtual void LeftClickDrawClosedLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::ref_ptr<osg::Group> tempGroup, osg::Matrix worldMatrix);

	virtual bool MouseMoveDrawClosedLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::ref_ptr<osg::Group> tempGroup, osg::Matrix worldMatrix);

	virtual void BackSpaceReverseDrawClosedLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::ref_ptr<osg::Group> drawGroup, osg::ref_ptr<osg::Group> tempGroup);

	virtual void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

protected:
	std::vector<osg::Vec3d> vecWorldCoord;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group* mEditSceneGroup;
	osg::Group* mTempGroup;
	InterfaceBaseDrawer* IBaseDrawer;

	bool isActivated;
};