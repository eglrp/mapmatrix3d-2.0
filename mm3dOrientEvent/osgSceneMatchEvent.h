#pragma once

#include "InterfaceBaseDrawer.h"
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"

class COsgSceneMatchEvent : public CBaseUserEvent
{
public:
	COsgSceneMatchEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editGroup, InterfaceSigEventCallBack* iSigEventCallBack);

	virtual void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	virtual void MatchSingleModel();																							//进行匹配

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

private:
	std::vector<osg::Vec3d> vecWorldCoord;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group* mEditGroup;

	InterfaceSigEventCallBack* iSigEventCallBack;

	//模型匹配
	std::vector<osg::Vec3d> vecRelaCoord;
	std::vector<osg::Vec3d> vecAbsCoord;

	//绝对定向旋转矩阵
	osg::Matrix absMatrix;

	bool isActivated;
};