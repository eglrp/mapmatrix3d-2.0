#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceOsgFinder.h"



class  COsgExtractTriangleBaseOutLineInAreaEvent : public CBaseUserEvent
{
public:
	COsgExtractTriangleBaseOutLineInAreaEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup, osg::ref_ptr<osg::Group> TempGroup, InterfaceSigEventCallBack* iSigEventCallBack) :
		CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		ISigEventCallBack = iSigEventCallBack;
		mapperGroup = MapperGroup;
		tempGroup = TempGroup;
		extractHeight = 0;
	}



protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	void handleDrawArea(const osgGA::GUIEventAdapter &ea);
	void handleExtractOutline(const osgGA::GUIEventAdapter &ea);

protected:
	void getTrianglePointsInScope(osg::ref_ptr<osg::Node> sceneNode, std::vector<osg::Vec3d>& vecWorldCoord, std::vector<osg::Vec3d>& vecPoints);
	void getOutlineByHeight(std::vector<osg::Vec3d>&vecPoints, double h, std::vector<osg::Vec3d>&vecLine);

	inline osg::Vec3d getInsection(osg::Vec3d& p1, osg::Vec3d& p2, double H)
	{
		return osg::Vec3d((H - p1.z())*(p2.x() - p1.x()) / (p2.z() - p1.z()) + p1.x(), (H - p1.z())*(p2.y() - p1.y()) / (p2.z() - p1.z()) + p1.y(), H);
	}
protected:
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *mapperGroup;
	osg::Group *tempGroup;

	std::vector<osg::Vec3d> vecWorldCoord;
	InterfaceSigEventCallBack* ISigEventCallBack;

	std::vector<osg::Vec3d> vecPoints;
	osg::Matrix worldMatrix;
	osg::Vec3d worldPt;
	bool isActivated;
	bool isActivated1;
	bool isActivated2;

	bool isDrawArea;

	bool isOutlineExist;//ÅÐ¶ÏÂÖÀªÏßÊÇ·ñ´æÔÚ
	double extractHeight;
};