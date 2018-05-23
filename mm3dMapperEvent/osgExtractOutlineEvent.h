#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"

class COsgExtractOutLineEvent : public CBaseUserEvent
{
public:
	COsgExtractOutLineEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup, InterfaceSigEventCallBack* iSigEventCallBack) :  CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		this->iSigEventCallBack = iSigEventCallBack;
		mapperGroup = MapperGroup;
		vecVec.clear();
	}

	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);
	std::vector<std::vector<osg::Vec3d> >& getVec();
	/*BOOL outForShp();
	BOOL outForDxf();*/

	bool exportOutline();
	void pushBack(std::vector<std::vector<osg::Vec3d> >& vec);//添加轮廓线
	void clearVec();
protected: 
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);


protected:
	std::vector<osg::Vec3d> vecWorldCoord;
	std::vector<std::vector<osg::Vec3d> > vecVec;
	std::vector<std::vector<osg::Vec3d> > vecVec1;
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *mapperGroup;

	InterfaceSigEventCallBack* iSigEventCallBack;

	//绝对定向旋转矩阵
	osg::Matrix absMatrix;
	bool isActivated;
};