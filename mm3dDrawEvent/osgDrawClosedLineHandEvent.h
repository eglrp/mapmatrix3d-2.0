// osgDrawClosedLinehandEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#pragma once
#include "InterfaceBaseDrawer.h"
#include "osgDrawClosedLineEvent.h"
#include "operationState.h"


class COsgDrawClosedLineHandEvent : public COsgDrawClosedLineEvent
{
public:
	COsgDrawClosedLineHandEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editSceneGroup, osg::Group* tempGroup) : COsgDrawClosedLineEvent(viewer, root,  editSceneGroup,  tempGroup), CBaseUserEvent(viewer)
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

	void LeftClickDrawClosedLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix);

	bool MouseMoveDrawClosedLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix);

	void BackSpaceReverseDrawClosedLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::ref_ptr<osg::Group> drawGroup);

	void DoubleClickDrawClosedLineOperate(std::vector<osg::Vec3d> &vecWorldCoord,  osg::Vec4d color,double size,  osg::ref_ptr<osg::Group> mEditSceneGroup,osg::Matrix worldMatrix);	

	virtual void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

protected:
	std::vector<osg::Vec3d> vecWorldCoord;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group* mEditSceneGroup;
	osg::Group* mTempGroup;
	InterfaceBaseDrawer* IBaseDrawer;
	osg::Matrix worldMatrix;
	bool isActivated;
};





