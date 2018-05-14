#pragma once
#include "OSG_Plus.h"
#include "BaseUserEvent.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceOsgDrawEvent.h"

class COsgDrawEvent : public InterfaceOsgDrawEvent
{
public:
	COsgDrawEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editSceneGroup, osg::Group* tempGroup, osg::Group* mapGroup);

	virtual void addEventToViewer(osg::ref_ptr<osgViewer::Viewer> viewer);

private:
	std::vector<CBaseUserEvent*> vecDrawEvent;
};