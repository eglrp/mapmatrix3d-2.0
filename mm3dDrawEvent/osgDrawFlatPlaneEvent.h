#pragma once
#include "InterfaceBaseDrawer.h"	
#include "BaseUserEvent.h"
#include "operationState.h"


class COsgDrawFlatPlaneEvent : public CBaseUserEvent
{
public:
	COsgDrawFlatPlaneEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editSceneGroup) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		mEditSceneGroup = editSceneGroup;
		InterfaceBaseDrawerFactory IBaseDrawerFactory;
		IBaseDrawer = IBaseDrawerFactory.create();

		isActivated = false;
	}

	void DrawFlatPlaneByHeight();																											//¸ß¶È²ÃÇÐ

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	void KeyPressAdjustDrawHeight(double zDecimal, osg::ref_ptr<osg::Node> node);

protected:
	std::vector<osg::Vec3d> vecWorldCoord;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group* mEditSceneGroup;
	InterfaceBaseDrawer* IBaseDrawer;
	bool isActivated;
};