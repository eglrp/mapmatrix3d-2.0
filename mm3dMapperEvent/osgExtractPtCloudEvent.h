#pragma once
#include "BaseUserEvent.h"
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceSqliteDatabase.h"
#include <vector>

class COsgExtractPtCloudEvent : public CBaseUserEvent
{
public:
	COsgExtractPtCloudEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> TempGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		this->iSigEventCallBack = iSigEventCallBack;
		tempGroup = TempGroup;
		//创建database和相应的table
		InterfaceSqliteDatabaseFactory iSqliteDatabaseFactory;
		iSqliteDatabase = iSqliteDatabaseFactory.create();
		ANGLE = 10;
		similarity = 0.9;
	}

	bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	double ANGLE;
	double similarity;//颜色相似度
protected:
	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);
	
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *tempGroup;																										//用于临时绘图
	InterfaceSigEventCallBack* iSigEventCallBack;

	bool isActivated;																											//墙面事件激活

	std::vector<std::vector<osg::Vec3d> > vecTotalTriangle;																		//所有三角面

	InterfaceSqliteDatabase* iSqliteDatabase;

	std::vector<osg::Vec3d> vecNormal;    //保存给定特征法向量
	std::vector<osg::Vec4> vecCorlor;     //保存特征向量
};
