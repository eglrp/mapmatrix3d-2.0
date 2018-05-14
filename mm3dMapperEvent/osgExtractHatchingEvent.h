#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "PGControl.h"

class COsgExtractHatchingEvent : public CBaseUserEvent
{
public:
	COsgExtractHatchingEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup, osg::ref_ptr<osg::Group> TempGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		isBatch = false;
		this->iSigEventCallBack = iSigEventCallBack;
		mapperGroup = MapperGroup;
		tempGroup = TempGroup;
		mStep = 10;
		bLocal = false;
		bConnect = false;
	}

protected:

	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	//开始提取剖面线
	bool startComputeHatching(float x, float y, osg::Vec3d p1, osg::Vec3d p2,  osg::NodePath& np1, bool repeat = false);

	//开始批量提取剖面线
	bool startComputeBatchHatching(std::vector<osg::Vec3d>& vecIn);

	//获得鼠标选中的底层文件
	bool getLodBottomLevelFile(float x, float y, osg::NodePath& np, osgViewer::Viewer *mViewer, std::string& m_tableName);

	//文件名转化为postgis表名
	std::string getTableName(string fileName);

	//替换字符串中指定子串
	void replace_all(std::string& s, std::string const & t, std::string const & w);

	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	//通过给定路径计算剖面线两个端点集
	void getHatchPathMultiline(std::vector<osg::Vec3d>& vecPtIn,double HatchLenth,double step,std::vector<osg::Vec3d>& vecPtOut);

	//通过给定点坐标找所在底层文件名
	bool findStartFile(osg::Vec3d& ps,osg::Vec3d& pe,std::string & tableName);

	//将ps相对pe移动len的距离
	void stretchPointToPoint(osg::Vec3d& ps,osg::Vec3d& pe,double len);

	void RegisterJScripts();

	void UnRegisterJScripts();
protected:
	std::vector<osg::Vec3d> vecWorldCoord;
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *mapperGroup;
	osg::Group *tempGroup;
	InterfaceSigEventCallBack* iSigEventCallBack;

	//绝对定向旋转矩阵
	osg::Matrix absMatrix;
	bool isActivated;
	bool isBatch;
	
	osg::NodePath np;
	osg::Matrix worldMatrix;
	
	PGconn* conn;

	std::string mEXEPath;
	osg::Vec3d PS, PE;//记录上一条剖面线的两个端点

public:
	double mStep;//批量提取步长

	//用于两端搜索
	float FX1, FY1;
	osg::NodePath NP1;
	bool bLocal;
	bool bConnect;
};