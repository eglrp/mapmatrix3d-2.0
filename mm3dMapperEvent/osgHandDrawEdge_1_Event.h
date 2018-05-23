#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "osgDrawMultiLineEvent.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceSqliteDatabase.h"
#include "osgExtractHandPaintLineEvent.h"

#define DLLAPI _declspec(dllexport)  

class DLLAPI COsgHandDrawEdge_1_Event : public COsgExtractHandPaintLineEvent
{
public:
	COsgHandDrawEdge_1_Event(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup, osg::ref_ptr<osg::Group> TempGroup, osg::ref_ptr<osg::Group> DataGroup, InterfaceSigEventCallBack* iSigEventCallBack) :
		COsgExtractHandPaintLineEvent(viewer, root, MapperGroup, TempGroup, iSigEventCallBack)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		ISigEventCallBack = iSigEventCallBack;
		mapperGroup = MapperGroup;
		tempGroup = TempGroup;
		dataGroup = DataGroup;

		InterfaceSqliteDatabaseFactory iSqliteDatabaseFactory;
		iSqliteDatabase = iSqliteDatabaseFactory.create();
		InterfaceBaseDrawerFactory IBaseDrawerFactory;
		IBaseDrawer = IBaseDrawerFactory.create();
	}

	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);
	bool CatchPointOperate(osg::Vec3d &worldPt, std::vector<osg::Vec3d>& vecPoints);
	void clearGroupSafety(osg::Group* father, osg::Group* children);
	void mouseMoveEvent(osg::Vec3d& worldPt, osg::NodePath np);
	bool getPointsInPolygon(string& childFileName, osg::Vec3d& worldPt, double r);
protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);


protected:
	std::vector<osg::Vec3d> vecWorldCoord;

	std::vector<osg::Vec3d> vecPoints;
	osg::Vec3d normal;
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *mapperGroup;
	osg::Group* tempGroup;
	osg::Group* dataGroup;
	osg::Matrix worldMatrix;

	InterfaceBaseDrawer* IBaseDrawer;
	InterfaceSigEventCallBack* ISigEventCallBack;
	bool isActivated;
	bool isCatch;

	InterfaceSqliteDatabase* iSqliteDatabase;																					//sqlite3Êý¾Ý¿â
};