#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceOsgFinder.h"
#include <libpq-fe.h>
#include "PGControl.h"

class COsgExtractLineBasePGEvent : public CBaseUserEvent
{
public:
	COsgExtractLineBasePGEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		bReplace = false;
		this->iSigEventCallBack = iSigEventCallBack;
		mapperGroup = MapperGroup;

		bEditObject = false;

		
	}

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);
	int LeftClickSearchPostGis(float x, float y, osg::Vec3d worldPt, osg::NodePath& np, osg::Vec3d nm, osg::Matrix worldMatrix);

	void RegisterJScripts();
	void UnRegisterJScripts();
	//全场景扩展
	void extentAll(PGconn* conn);
	
	//从数据库删除记录
	int deleteResFromPG(PGconn* conn, osg::Vec3d pt);

	//从数据库读取轮库线记录
	void readContourFromPG(PGconn* conn, std::string tb);
	
	//提取单体
	void DoExtractObject(PGconn* conn ,double x,double y,double dist = 1.0);

	//根据鼠标屏幕坐标 查找最底层osgb名称，转化为相应表名
	bool getLodBottomLevelFile(float x, float y, osg::NodePath& np, osgViewer::Viewer *mViewer, std::string& m_tableName);
	
	string getTableName(string fileName);

	void editObject(osg::Vec3d worldPt, osg::Vec3d worldNm);
	void deleteTriInAngle(std::vector<int>& vNei,double nx,double ny,double nz,std::string tilename);
	void deleteOutofPolygon(std::vector<osg::Vec3d>& vCoord);



	void replace_all(std::string& s, std::string const & t, std::string const & w);

protected:
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *mapperGroup;

	InterfaceSigEventCallBack* iSigEventCallBack;
	osg::Matrix worldMatrix;
	osg::Vec3d worldPt;
	osg::Vec3d worldNm;
	osg::Vec3d normal;
	osg::NodePath np;

	double xx;
	double yy;
	bool isActivated;
	bool bReplace;

	PGconn* conn;

	std::string mEXEPath;
	bool bEditObject;
	std::vector<osg::Vec3d> vecCoord;
	std::string mContour;
	double mZ;
	std::vector<int> mVec;

	
};