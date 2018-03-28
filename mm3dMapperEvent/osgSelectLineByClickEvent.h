#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceOsgFinder.h"
#include "Geometry.h"

class COsgSelectLineByClickEvent :  public CBaseUserEvent
{
public:
	COsgSelectLineByClickEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup, InterfaceSigEventCallBack* iSigEventCallBack) :  CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		this->iSigEventCallBack = iSigEventCallBack;
		mapperGroup = MapperGroup;
	}

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	

	void COsgDeleteSelectLine(osg::ref_ptr<osg::Group> MapperGroup);																											//删除选择线	

	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	void HighLightLine(osg::ref_ptr<osg::Group> MapperGroup, osg::Vec3d pt);																			//高亮在矩形内的线

	void CancelHighLightLineForAll(osg::ref_ptr<osg::Group> MapperGroup);																										//取消全部高亮

	void CancelHighLightLine(osg::ref_ptr<osg::Group> MapperGroup);																		//取消范围内的高亮		

	//double distanceOfPointAndLine(osg::Vec3d pt,osg::Vec3d line1,osg::Vec3d line2);

	double distancep2p(osg::Vec3d pt1,osg::Vec3d pt2)
	{
		return sqrt((pt1.x()-pt2.x())*(pt1.x()-pt2.x()) + (pt1.y()-pt2.y())*(pt1.y()-pt2.y()) + (pt1.z()-pt2.z())*(pt1.z()-pt2.z()));
	}
protected:
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *mapperGroup;

	std::vector<CL::Vec2d> currentPol;
	std::vector<std::vector<osg::MatrixTransform*> > vecHighLightTrans;
	InterfaceSigEventCallBack* iSigEventCallBack;
	std::vector<osg::Vec3d> vecWorldCoord;
	osg::Matrix worldMatrix;
	osg::Vec3d worldPt;
	bool isActivated;
	bool isFirstPoint;
};