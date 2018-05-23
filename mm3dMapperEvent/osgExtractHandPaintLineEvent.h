//手动绘制非闭合线（剖面线）

#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceOsgFinder.h"


class COsgExtractHandPaintLineEvent : public CBaseUserEvent
{
public:
	COsgExtractHandPaintLineEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup, osg::ref_ptr<osg::Group> TempGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		this->iSigEventCallBack = iSigEventCallBack;
		mapperGroup = MapperGroup;
		tempGroup = TempGroup;

		InterfaceBaseDrawerFactory IBaseDrawerFactory;
		IBaseDrawer = IBaseDrawerFactory.create();
	}
protected:

	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);
	bool CatchPointOperate(osg::Vec3d &worldPt,osg::Group* mapperGroup);
	void InsertPoints(osg::Vec3d p_start,osg::Vec3d p_end,std::vector<osg::Vec3d> &vecPoints);

	double distancep2p(osg::Vec3d pt1,osg::Vec3d pt2)
	{
		return sqrt((pt1.x()-pt2.x())*(pt1.x()-pt2.x()) + (pt1.y()-pt2.y())*(pt1.y()-pt2.y()) + (pt1.z()-pt2.z())*(pt1.z()-pt2.z()));
	}
protected: 
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	virtual void LeftClickExtractHandPaintLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix);

	virtual bool MouseMoveExtractHandPaintLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix);

	virtual void DoubleClickDrawExtractHandPaintOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Group* TempGroup);

protected:
	std::vector<osg::Vec3d> vecWorldCoord;
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *mapperGroup;
	osg::Group* tempGroup;

	InterfaceBaseDrawer* IBaseDrawer;
	InterfaceSigEventCallBack* iSigEventCallBack;
	bool isActivated;
	bool isCatch;
};