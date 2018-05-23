#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceSqliteDatabase.h"
#include "alphaShapes.h"
#include <vector>

class COsgExtractPtCloudEdgeEvent : public CBaseUserEvent
{
public:
	COsgExtractPtCloudEdgeEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup, osg::ref_ptr<osg::Group> TempGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		this->iSigEventCallBack = iSigEventCallBack;
		mapperGroup = MapperGroup;
		tempGroup = TempGroup;
		alpha = 0.5;
		//创建database和相应的table
		InterfaceSqliteDatabaseFactory iSqliteDatabaseFactory;
		iSqliteDatabase = iSqliteDatabaseFactory.create();
	}
	double alpha;
protected:
	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	
protected:
	vector<osg::Vec3d>  vecWorldCoord;
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *mapperGroup;
	osg::Group *tempGroup;
	osg::Matrix worldMatrix;
	bool isActivated;
	
	
	InterfaceSigEventCallBack* iSigEventCallBack;

	InterfaceSqliteDatabase* iSqliteDatabase;
};