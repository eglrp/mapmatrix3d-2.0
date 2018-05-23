#pragma once
#include "InterfaceBaseDrawer.h"	
#include "osgDrawSinglePtHandler.h"
#include "operationState.h"

class COsgDrawMultiPtEvent : public COsgDrawSinglePtHandler
{
public:
	COsgDrawMultiPtEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editSceneGroup) : COsgDrawSinglePtHandler(viewer, root, editSceneGroup), CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		mEditSceneGroup = editSceneGroup;
		InterfaceBaseDrawerFactory IBaseDrawerFactory;
		IBaseDrawer = IBaseDrawerFactory.create();

		isActivated = false;
	}
		
protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	virtual void CtrlLeftClickDrawMultiPtOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix);

	virtual void BackSpaceReverseMultiPtOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::ref_ptr<osg::Group> drawGroup);

	virtual void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

protected:
	std::vector<osg::Vec3d> vecWorldCoord;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group* mEditSceneGroup;
	InterfaceBaseDrawer* IBaseDrawer;
	bool isActivated;
};