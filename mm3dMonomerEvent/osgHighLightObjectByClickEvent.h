#pragma once
#include "BaseUserEvent.h"
#include "operationState.h"
#include <vector>
#include "InterfaceUserEventCallBack.h"
#include "HighLighter.h"
#include "Selector.h"
#include "HighLightOutputer.h"

class COsgHighLightObjectByClickEvent : public CBaseUserEvent
{
public:
	COsgHighLightObjectByClickEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> EditGroup, InterfaceSigEventCallBack* iSigEventCallBack) 
		: CBaseUserEvent(viewer), highLightOutputer(viewer, iSigEventCallBack)
	{
		mViewer = viewer;
		mRoot = root;
		editGroup = EditGroup;
		this->iSigEventCallBack = iSigEventCallBack;
		selPagedLod = NULL;
	}

	virtual void outputModelWithoutTexture(std::string outModelWithoutFileName);

	virtual void outputTextureModel(std::string outModelFileName);

	virtual	void removeSelPagedLod();

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	virtual void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

protected:
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *editGroup;
	bool isActivated;
	InterfaceSigEventCallBack* iSigEventCallBack;

	osg::ref_ptr<osg::Node> selPagedLod;

protected:
	CHighLighter highLighter;

	CSelector selector;

	CHighLightOutputer highLightOutputer;
};