#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceSqliteDatabase.h"
#include <vector>


class COsgDragPointEvent : public CBaseUserEvent
{
public:
	COsgDragPointEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MpointsGroup, osg::ref_ptr<osg::Group> TempGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		this->iSigEventCallBack = iSigEventCallBack;
		dragflag = false;
		pointsGroup = MpointsGroup;
		tempGroup = TempGroup;
	
		//创建database
		InterfaceSqliteDatabaseFactory iSqliteDatabaseFactory;
		iSqliteDatabase = iSqliteDatabaseFactory.create();
	}
protected:
	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	bool findPointNearby(osg::Vec3d& pt, osg::Vec3d& ptFind, osg::Group *pointsGroup, double dis);
	void doubleClickDeletePoint(osg::Vec3d&worldPt, osg::Group *pointsGroup, double dis);
	void loadPointsData(std::vector<osg::Vec3d>& vecWorldCoord, InterfaceSqliteDatabase* iSqliteDatabase,char* name);
	void deleteItemInVector(osg::Vec3d& pt, std::vector<osg::Vec3d>& vecWorldCoord);
	
protected:
	bool isActivated;
	bool dragflag;//标记是否在拖动点
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	std::vector<osg::Vec3d> vecBackup;
	osg::Group *pointsGroup;
	osg::Group *tempGroup;
	osg::Matrix worldMatrix;

	std::vector<osg::Vec3d> vecWorldCoord;
	InterfaceSigEventCallBack* iSigEventCallBack;

	InterfaceSqliteDatabase* iSqliteDatabase;

};