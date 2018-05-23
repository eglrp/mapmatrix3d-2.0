#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "imageTools.h"

class  COsgExtractFarmLandByDsmEvent : public CBaseUserEvent
{
public:
	COsgExtractFarmLandByDsmEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
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

	void getFarmEdge(osg::Vec3d& worldPt, osg::Matrix worldMatrix);

protected:

	osgViewer::Viewer *mViewer;

	osg::Group *mRoot;

	osg::Group *mapperGroup;//”√”⁄¡Ÿ ±ªÊÕº

	InterfaceSigEventCallBack* ISigEventCallBack;

	bool isActivated;

	double leftX;

	double leftY;

	double resX;

	double resY;

	IplImage* contourImage;

	CImageTool imageTool;

	char* coordFileName;

	char* dsmFileName;

	bool isFirstLine;
};