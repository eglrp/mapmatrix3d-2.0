#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include <vector>
#include "InterfaceUserEventCallBack.h"


class COsgExtractObjectByPolygonEvent : public CBaseUserEvent
{
public:
	COsgExtractObjectByPolygonEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> EditGroup, osg::ref_ptr<osg::Group> ObjectGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated1 = false;
		isActivated2 = false;
		editGroup = EditGroup;
		objectGroup = ObjectGroup;
		iSigEventCallBack = iSigEventCallBack;
	}

	void ExtractObject(std::string inFilePathName);																					//接口， 单体化物体

	void ExtractArea(std::string inFilePathName);																					//接口, 区域单体化

	bool SelectObjectPt();																											//接口，选择裁切点
		
protected:
	osgViewer::Viewer *mViewer;

	osg::Group *mRoot;

	osg::Group *editGroup;

	osg::ref_ptr<osg::Group> objectGroup;
	
	bool isActivated1;
	
	bool isActivated2;

	std::vector<osg::Vec3d> vecWorldPt;
	
	osg::Vec3d saveClickPt;

	std::vector<CScope> vecScope;

	InterfaceSigEventCallBack* iSigEventCallBack;

protected:
	virtual void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	virtual bool GetClipScope(std::vector<CScope> &vecClipScope, std::vector<CScope> vecScope, std::vector<osg::Vec3d> vecWorldCoord);

	virtual bool DrawExtractObjectAreaOperate(const osgGA::GUIEventAdapter &ea);

	virtual bool DrawExtractHouseOperate(const osgGA::GUIEventAdapter &ea);

	virtual bool FindMinHeight(std::vector<osg::Vec3d> vecWorldPt, double &minHeight);
};