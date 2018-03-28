//手动绘制闭合线（轮廓线）

#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"

class COsgExtractHandPaintOutLineEvent : public CBaseUserEvent
{
public:
	COsgExtractHandPaintOutLineEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup,InterfaceSigEventCallBack* iSigEventCallBack) :  CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		this->iSigEventCallBack = iSigEventCallBack;
		mapperGroup = MapperGroup;
		vecVec.clear();
		vecWorldCoord.clear();
	}

	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);
	std::vector<std::vector<osg::Vec3d> >& getVec();
	void clearVec();
	
protected: 
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);


protected:
	std::vector<osg::Vec3d> vecWorldCoord;
	std::vector<std::vector<osg::Vec3d> > vecVec;
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *mapperGroup;
	

	
	InterfaceSigEventCallBack* iSigEventCallBack;

	//绝对定向旋转矩阵
	osg::Matrix absMatrix;
	bool isActivated;

	osg::Matrix worldMatrix;
	osg::Vec3d worldPt;
};