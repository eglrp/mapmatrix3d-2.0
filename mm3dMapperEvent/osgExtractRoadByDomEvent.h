#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "osgExtractRoad.h"
#include <vector>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "imageTools.h"

class  COsgExtractRoadByDomEvent : public CBaseUserEvent
{
public:
	COsgExtractRoadByDomEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		ISigEventCallBack = iSigEventCallBack;
		mapperGroup = MapperGroup;

		isFirstLine = true;
	}

protected:
	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	void getRoadEdge(IplImage* image, osg::Vec3d& worldPt, osg::Matrix worldMatrix);

protected:

	osgViewer::Viewer *mViewer;

	osg::Group *mRoot;

	osg::Group *mapperGroup;//用于临时绘图

	InterfaceSigEventCallBack* ISigEventCallBack;

	bool isActivated;

	double leftX;

	double leftY;

	double resX;

	double resY;

	IplImage* image;

	IplImage* contourImage;

	//CImageTool imageTool;

	char* coordFileName;
	
	char* imageFileName;

	char* dsmFileName;

	bool isFirstLine;  //是否是第一条线
};