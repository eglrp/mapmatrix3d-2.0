#pragma once
#include "osgSelectLineByAreaEvent.h"
#include "osgSelectLineByRectangleEvent.h"
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceSqliteDatabase.h"

class COsgSelectPtCloudByAreaEvent : public COsgSelectLineByAreaEvent
{
public:
	COsgSelectPtCloudByAreaEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> PointsGroup, InterfaceSigEventCallBack* iSigEventCallBack) : COsgSelectLineByAreaEvent(viewer, root, PointsGroup, iSigEventCallBack)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		this->iSigEventCallBack = iSigEventCallBack;
		pointsGroup = PointsGroup;

		//创建database和相应的table
		InterfaceSqliteDatabaseFactory iSqliteDatabaseFactory;
		iSqliteDatabase = iSqliteDatabaseFactory.create();
	}
protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);
	
	void COsgDeleteSelectPoints(osg::ref_ptr<osg::Group> MapperGroup, std::vector<std::vector<osg::MatrixTransform*>> &VecHighLightTrans, std::vector<osg::Vec3d>& vecDelete);
	void deleteItemInVector(osg::Vec3d& pt, std::vector<osg::Vec3d>& vecWorldCoord);
protected:
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *pointsGroup;
	osg::Matrix worldMatrix;
	std::vector<std::vector<osg::MatrixTransform*>> vecHighLightTrans;
	InterfaceSigEventCallBack* iSigEventCallBack;
	std::vector<osg::Vec3d> vecWorldCoord;
	std::vector<osg::Vec3d> vecDelete;
	std::vector<osg::Vec3d> vecTotalPoints;
	bool isActivated;

	InterfaceSqliteDatabase* iSqliteDatabase;
};