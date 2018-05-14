#pragma once
#include "InterfaceBaseDrawer.h"
#include "BaseUserEvent.h"
#include "operationState.h"


#define SINGLELINEEVENTAPI _declspec(dllexport)  
 


class SINGLELINEEVENTAPI COsgDrawRoadPathEvent : public CBaseUserEvent
{
public:
	COsgDrawRoadPathEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editSceneGroup) : CBaseUserEvent(viewer)
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

	virtual void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	virtual void LeftClickDrawRoadPathOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix, bool isOnlyOneLine);

	virtual void MouseMoveDrawRoadPathOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix);

	void insertNodePath( osg::Vec3d& p1, osg::Vec3d& p2, std::vector<osg::Vec3d>& vecWorldCoord);
protected:
	std::vector<osg::Vec3d> vecWorldCoord;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group* mEditSceneGroup;

	InterfaceBaseDrawer* IBaseDrawer;
	bool isActivated;
};