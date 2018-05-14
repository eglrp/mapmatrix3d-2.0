#pragma once
#include "OSG_Plus.h"
#include "BaseUserEvent.h"
#include "InterfaceOsgMeasureEvent.h"

class COsgMeasureEvent : public InterfaceOsgMeasureEvent
{
public:
	COsgMeasureEvent(osgViewer::Viewer *viewer, osg::Group *root, InterfaceSigEventCallBack* iSigEventCallBack);

	virtual void addEventToViewer(osg::ref_ptr<osgViewer::Viewer> viewer);

private:
	std::vector<CBaseUserEvent*> vecMeasureEvent;
};

