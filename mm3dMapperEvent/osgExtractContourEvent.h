#pragma once
#include "BaseUserEvent.h"
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"
#include "osgExtractContourEvent.h"

class COsgExtractContourEvent : public CBaseUserEvent
{
public:
	COsgExtractContourEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		this->iSigEventCallBack = iSigEventCallBack;
		mMapperGroup = MapperGroup;
	}

	bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	void extractContour(double zs,double ze,double zspan);
protected:
	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);


	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *mMapperGroup;	 
	
	InterfaceSigEventCallBack* iSigEventCallBack;
	
	bool isActivated;
};