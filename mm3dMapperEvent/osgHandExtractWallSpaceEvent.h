#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceSqliteDatabase.h"
#include <vector>

class COsgHandExtractWallSpaceEvent : public CBaseUserEvent
{
public:
	COsgHandExtractWallSpaceEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> TempGroup, osg::ref_ptr<osg::Group> EditGroup, osg::ref_ptr<osg::Group> interfaceGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		this->iSigEventCallBack = iSigEventCallBack;
		tempGroup = TempGroup;
		mEditGroup = EditGroup;
		mSlotGroup = interfaceGroup;
		//创建database和相应的table
		InterfaceSqliteDatabaseFactory iSqliteDatabaseFactory;
		iSqliteDatabase = iSqliteDatabaseFactory.create();
	}
	
protected:
	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
	

protected:
	vector<vector<osg::Vec3d> > vecTotalTriangle;
	std::vector<osg::Vec3d> vecCoord;
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *tempGroup;
	osg::Group *mEditGroup;
	osg::Group *mSlotGroup;
	bool isActivated;
	osg::Vec3d normal;
	osg::Matrix worldMatrix;
	InterfaceSigEventCallBack* iSigEventCallBack;

	InterfaceSqliteDatabase* iSqliteDatabase;																					//sqlite3数据库
};