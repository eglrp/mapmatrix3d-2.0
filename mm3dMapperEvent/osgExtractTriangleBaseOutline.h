#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceOsgFinder.h"
#include "PGControl.h"
class cLine
{
public:
	osg::Vec3d ps;
	osg::Vec3d pe;
};
class COsgExtractTriangleBaseOutLineEvent : public CBaseUserEvent
{
public:
	COsgExtractTriangleBaseOutLineEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup, osg::ref_ptr<osg::Group> TempGroup, InterfaceSigEventCallBack* iSigEventCallBack) :  CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		this->iSigEventCallBack = iSigEventCallBack;
		mapperGroup = MapperGroup;
		tempGroup = TempGroup;
		conn = NULL;

		m_offs = 3.5;
	}

protected: 
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);
	bool LeftClickDrawTriangleBaseOutLine(osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> mapperGroup, osg::Matrix worldMatrix, osg::NodePath np);
	bool LeftClickSearchPostGis(float x, float y, osg::Vec3d worldPt, osg::NodePath np,osg::Vec3d nm);
	void getTrianglePointsInScope(osg::ref_ptr<osg::Node> sceneNode, std::vector<osg::Vec3d>& vecWorldCoord, std::vector<CLine>& vecPoints, double h);
	void getTrianglePointsInScope(osg::ref_ptr<osg::Node> sceneNode, std::vector<osg::Vec3d>& vecWorldCoord, std::string id, double height = 0);
	void linkLine(PGconn* conn, const osg::Vec3d& worldPt, std::vector<osg::Vec3d>& vecPt);

	void showExtractArea(const osg::Vec3d& vecCoord,const osg::Matrix& matrix, osg::ref_ptr<osg::Group> drawGroup);
	void showPlane(double A,double B,double C,double D,osg::Vec3d pt,double h,double w);

protected:
	void rectExtend(std::vector<osg::Vec3d>& vecRect, float dist);
	void replace_all(std::string& s, std::string const & t, std::string const & w);
	void outRect(const vector<osg::Vec3d>& vecOsgPts, vector<osg::Vec3d>& vec);
	void createCircle(osg::Vec3d center, double radiu, std::vector<osg::Vec3d>& _vertices);
	void create8Polygon(osg::Vec3d center, double radiu, std::vector<osg::Vec3d>& _vertices);
	double caculateError(const std::vector<osg::Vec3d>& vecOri, const std::vector<osg::Vec3d>& vecSimp);

	bool extractContour(double high);//提取等高线
protected:
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *mapperGroup;
	osg::Group *tempGroup;

	InterfaceSigEventCallBack* iSigEventCallBack;
	osg::Matrix worldMatrix;
	osg::Vec3d worldPt;
	bool isActivated;

	PGconn* conn;
	double m_offs;
	std::string mEXEPath;
};