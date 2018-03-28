#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceOsgFinder.h"
#include <vector>


class COsgCombineGroupEvent : public CBaseUserEvent
{
public:
	COsgCombineGroupEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> MapperGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		this->iSigEventCallBack = iSigEventCallBack;

		
		mapperGroup = MapperGroup;

		numChild = 0;
	}
protected:
	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
	
	bool isActivated;
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	
	osg::Group *mapperGroup;//可编辑的墙面绘制在这个节点上
	osg::Matrix worldMatrix;
	InterfaceSigEventCallBack* iSigEventCallBack;

	std::vector<LineForCombine> vecLFC;
	vector<osg::Vec3d> vecCoord;
	
	int numChild ;
};
