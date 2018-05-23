#pragma once
#include "osgDrawClosedLineDBclickEvent.h"
#include "osgDrawSinglePtHandler.h"

class COsgDrawClosedLineDBWithPointEvent : public COsgDrawClosedLineDBclickEvent, public COsgDrawSinglePtHandler
{
public:
	COsgDrawClosedLineDBWithPointEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editSceneGroup, osg::Group* tempGroup) : COsgDrawClosedLineDBclickEvent(viewer, root, editSceneGroup, tempGroup), COsgDrawSinglePtHandler(viewer, root, editSceneGroup), COsgDrawMultiClosedLineEvent(viewer, root, editSceneGroup, tempGroup), CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		mEditSceneGroup = editSceneGroup;
		mTempGroup = tempGroup;
	}

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	virtual void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	virtual void DoubleClickDrawClosedLineOperate(osg::Vec3d worldPt, std::vector<osg::Vec3d> VecWorldCoord, osg::ref_ptr<osg::Group> editSceneGroup, osg::Matrix worldMatrix);

	virtual bool DrawClosedLineEventOperate(const osgGA::GUIEventAdapter &ea);

	virtual bool DrawSinglePtEventOperate(const osgGA::GUIEventAdapter &ea);

protected:
	std::vector<osg::Vec3d> vecWorldCoord;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group* mEditSceneGroup;
	osg::Group* mTempGroup;

	bool isActivated1;
	bool isActivated2;

};

