#pragma once

#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceOsgOrient.h"


class COsgAbsOrientEvent : public CBaseUserEvent
{
public:
	COsgAbsOrientEvent(osgViewer::Viewer *viewer, osg::Group *root,osg::Group *editGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		mEditGroup = editGroup;

		this->iSigEventCallBack = iSigEventCallBack;
		isActivated = false;

		InterfaceOsgOrientFactory IAbsOrientFactory;
		IAbsOrient = IAbsOrientFactory.create();
	}

	virtual BOOL AbsOrient(std::string inFilePath);																//绝对定向
	
	virtual void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);
	 
	virtual void LoadControlPt();																				//加载控制点
	 
	virtual void ReclickCtrlPt(int ptNum);
	
	virtual void ReSetCtrlPtCheckStatus(int ptNum, bool isCheckPt);												//重新设置检查点

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

protected:
	std::vector<osg::Vec3d> vecWorldCoord;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;

	osg::Group *mEditGroup;
	InterfaceSigEventCallBack* iSigEventCallBack;
	InterfaceOsgOrient* IAbsOrient;

	//绝对定向旋转矩阵
	osg::Matrix absMatrix;

	bool isActivated;
};