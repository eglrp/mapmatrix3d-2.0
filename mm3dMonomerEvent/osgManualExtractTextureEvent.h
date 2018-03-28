
#pragma  once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceOsgFinder.h"
#include <vector>

class COsgManualExtractTextureEvent : public CBaseUserEvent
{
public:
	COsgManualExtractTextureEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> EditGroup, InterfaceSigEventCallBack* iSigEventCallBack) :  CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		editGroup = EditGroup;
	}

	virtual void StartExtractTexture(double res, double deltaZ);																					//开始抽取纹理

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	virtual void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *editGroup;
	std::vector<osg::Vec3d> vecWorldPt;
	osg::Vec3d pickNormal; 

	InterfaceSigEventCallBack* iSigEventCallBack;
	bool isActivated;
};