#pragma once
#include "operationState.h"
#include <vector>
#include "InterfaceUserEventCallBack.h"
#include "osgHighLightObjectByClickEvent.h"
#include "HighLighter.h"
#include "Selector.h"

class COsgLinkPropertyWithObjectEvent : public COsgHighLightObjectByClickEvent
{
public:
	COsgLinkPropertyWithObjectEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> EditGroup, InterfaceSigEventCallBack* iSigEventCallBack) : COsgHighLightObjectByClickEvent(viewer, root, EditGroup, iSigEventCallBack)
	{
		mViewer = viewer;
		mRoot = root;
		editGroup = EditGroup;
		this->iSigEventCallBack = iSigEventCallBack;
		propertyNode = NULL;
	}

	virtual void SetObjectProperty(std::string strObjectProperty);

	virtual void LookUpOjbectProperty(std::string objectPropertyName, std::string objectPropertyValue);

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	virtual void UpdateActivatedStatus(std::vector<OperatingState> vecOpState);

	virtual void FlyToPropertyNode(osg::ref_ptr<osg::Node> grandPropertyNode);

protected:
	osgViewer::Viewer *mViewer;
	osg::Group *mRoot;
	osg::Group *editGroup;
	bool isActivated;
	InterfaceSigEventCallBack* iSigEventCallBack;
	osg::ref_ptr<osg::Node> propertyNode;

protected:
	CHighLighter highLighter;

	CSelector selector;
};