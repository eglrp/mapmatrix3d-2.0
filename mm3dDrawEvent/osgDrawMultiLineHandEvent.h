#pragma once
#include "InterfaceBaseDrawer.h"
#include "osgDrawClosedLineHandEvent.h"
#include "operationState.h"

class COsgDrawMultiLineHandEvent : public COsgDrawClosedLineHandEvent
{
public:
	COsgDrawMultiLineHandEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editSceneGroup, osg::Group* tempGroup) : COsgDrawClosedLineHandEvent(viewer, root,  editSceneGroup,  tempGroup), CBaseUserEvent(viewer)
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

	virtual void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	virtual void DoubleClickDrawClosedLineOperate(std::vector<osg::Vec3d> &vecWorldCoord,  osg::Vec4d color,double size,  osg::ref_ptr<osg::Group> mEditSceneGroup,osg::Matrix worldMatrix);	

protected:
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group* mEditSceneGroup;
	osg::Group* mTempGroup;
	InterfaceBaseDrawer* IBaseDrawer;
	osg::Matrix worldMatrix;
	bool isActivated;
};


//继承   用户多线测量的绘图
class COsgDrawMultiLineForMultiLineEvent : public COsgDrawMultiLineHandEvent
{
public:
	COsgDrawMultiLineForMultiLineEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editSceneGroup, osg::Group* tempGroup) : 
	COsgDrawMultiLineHandEvent(viewer, root,  editSceneGroup,  tempGroup), CBaseUserEvent(viewer)
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

	virtual void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	virtual void DoubleClickDrawClosedLineOperate(std::vector<osg::Vec3d> &vecWorldCoord,  osg::Vec4d color,double size,  osg::ref_ptr<osg::Group> mEditSceneGroup,osg::Matrix worldMatrix);	
protected:
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group* mEditSceneGroup;
	osg::Group* mTempGroup;
	InterfaceBaseDrawer* IBaseDrawer;
	osg::Matrix worldMatrix;
	bool isActivated;
};