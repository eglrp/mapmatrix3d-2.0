#pragma once
#include <osgManipulator/Translate2DDragger>
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"
#include <osg/ShapeDrawable>
#include <osgManipulator/Command>
#include <osgManipulator/CommandManager>  
#include <osgViewer/Viewer>
#include "CDraggerPoint.h"


using namespace osgManipulator;

class COsgHollowOutSuspendEvent : public CBaseUserEvent
{
public:
	COsgHollowOutSuspendEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editGroup,osg::Group* tempGroup,osg::Group* scopeGroup, 
		InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		mEditScopeGroup = editGroup;
		mTempScopeGroup =  tempGroup;
		ScopeGroup = scopeGroup;
		isActivated = false;
		this->iSigEventCallBack = iSigEventCallBack;
		_activeDragger = 0;
		atScope = false;
		scopeInfo.scope.maxH = 0;
		scopeInfo.scope.minH = 0;
	}

	void HollowOutSuspend(std::string inFilePath);																																//裁切悬空物

	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

protected: 
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	virtual bool CtrlLeftClickOperate(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	virtual bool CtrlReleaseOperate(osg::ref_ptr<osg::Group> root, osg::ref_ptr<osg::Group> group, std::vector<CScope> &VecScope);

	virtual bool CtrlDragOperate(osgViewer::Viewer* viewer, const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	virtual bool BackSpaceOperate(std::vector<CScope> &vecScope, osg::ref_ptr<osg::Group> group);

	virtual bool KeyZOperate(std::vector<CScope> &vecScope, osg::ref_ptr<osg::Group> group);

	bool StartHollowOutSuspendedByScope(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePath, bool isInside, InterfaceSigEventCallBack* iSigEventCallBack);

	virtual bool ObtainScope(CScope &scope, std::string &fileName, osg::Vec3d worldPt, osg::Vec3d localPt, osg::Matrix worldMatrix, osg::NodePath np);

	bool findMinMax(std::vector<osg::Vec3d> vecMinMax, std::vector<osg::Vec3d> &coord);

	bool inScope(osg::Vec3d& pt, CScope& scope, SCOPEINFO& scopeInfo);//判断点在包围圈内

	void pushEvent(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	CScope ReDrawScope(osg::Vec3d &pt,SCOPEINFO &scopeInfo,osg::Group* EditSceneGroup,osg::Matrix worldMatrix);

	inline double distance2(osg::Vec3d p1,osg::Vec3d p2)
	{
		return sqrt((p1.x()-p2.x())*(p1.x()-p2.x()) + (p1.y()-p2.y())*(p1.y()-p2.y()) + (p1.z()-p2.z())*(p1.z()-p2.z()));
	}
protected:
	std::vector<CScope> vecScope;
	
	osg::Matrix worldMatrix;
	osg::Vec3d worldPt;
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group* mEditScopeGroup;//场景scope
	osg::Group* ScopeGroup;
	osg::Group* mTempScopeGroup;//临时scope
	InterfaceSigEventCallBack* iSigEventCallBack;

	SCOPEINFO scopeInfo;
	bool isUp;//是否向上选择范围
	bool isActivated;
	bool atScope;
	osg::Vec3d endPoint;
	
	osgManipulator::PointerInfo _pointer;
	CDraggerPoint* _activeDragger;
};



