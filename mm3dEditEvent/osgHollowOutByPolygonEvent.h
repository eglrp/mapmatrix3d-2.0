#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include "InterfaceUserEventCallBack.h"

                        
class COsgHollowOutByPolygonEvent : public CBaseUserEvent
{
public:
	COsgHollowOutByPolygonEvent(osgViewer::Viewer *viewer, osg::Group *root, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
	{
		mViewer = viewer;
		mRoot = root;
		isActivated = false;
		this->iSigEventCallBack = iSigEventCallBack;
	}

	void HollowOutByPolygon(std::string inFilePath)
	{
		osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);

		std::vector<CScope> vecClipScope;
		bool isSuccess = GetClipScope(vecClipScope, vecScope, vecWorldCoord);

		if (!isSuccess)
		{
			return;
		}

		StartHollowOutByPolygon(sceneNode, inFilePath, vecClipScope, false, iSigEventCallBack);
	}

	void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

protected: 
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

protected:
	bool StartHollowOutByPolygon(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePath, std::vector<CScope> vecClipScope, bool isInside, InterfaceSigEventCallBack* iSigEventCallBack);

	bool GetClipScope(std::vector<CScope> &vecClipScope, std::vector<CScope> vecScope, std::vector<osg::Vec3d> vecWorldCoord);

protected:
	std::vector<osg::Vec3d> vecWorldCoord;
	std::vector<CScope> vecScope;

	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	bool isActivated;

	InterfaceSigEventCallBack* iSigEventCallBack;

	
};
