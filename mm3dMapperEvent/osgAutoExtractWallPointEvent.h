#pragma once
#include "BaseUserEvent.h"
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceSqliteDatabase.h"
#include "PGControl.h"
#include "CDraggerPoint.h"
#include "Geometry.h"


class COsgAutoExtractWallPointEvent : public CBaseUserEvent
{
public:
	COsgAutoExtractWallPointEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> EditGroup, osg::ref_ptr<osg::Group> interfaceGroup, osg::ref_ptr<osg::Group> TempGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		this->iSigEventCallBack = iSigEventCallBack;
		mEditGroup = EditGroup;
		slotGroup = interfaceGroup;
		mTempGroup = TempGroup;
		conn = NULL;
	}

	bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

protected:
	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	void RegisterJScripts();
	
	void UnRegisterJScripts();

	void doBackSpaceEvent();
	void doSearchPointEvent(float x, float y, osg::NodePath np, osg::Vec3d worldPt, osg::Vec3d normal, std::vector<point3D>& vecLineRes);
	osgViewer::Viewer *mViewer;																									
	osg::Group *mRoot;
	osg::Group *mEditGroup;	 //用于临时绘图
	osg::Group *slotGroup;//用于将编辑节点加到根节点中
	osg::Group *mTempGroup;
	InterfaceSigEventCallBack* iSigEventCallBack;
	osg::Matrix worldMatrix;
	bool isActivated;																											//墙面事件激活

	std::vector<std::vector<osg::Vec3d> > vecTotalTriangle;																		//所有三角面

	InterfaceSqliteDatabase* iSqliteDatabase;																					//sqlite3数据库

	PGconn* conn;
	std::string mEXEPath;

	std::vector<PLane> m_vecPlane;
	std::vector<osg::Vec3d> m_vecPlanePoint;
	std::vector<osg::Vec3d> m_vecContourPoint;

	std::vector<osg::Vec3d> m_vecPointOfLastWall;           //记录拟合上一个面的点集
};

