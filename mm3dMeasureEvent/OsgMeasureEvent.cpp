#include "stdafx.h"
#include "OsgMeasureEvent.h"
#include "osgMeasurePtEvent.h"
#include "osgMeasureSingleLineEvent.h"
#include "osgMeasureMultiLineEvent.h"
#include "osgMeasrueAreaEvent.h"

COsgMeasureEvent::COsgMeasureEvent(osgViewer::Viewer *viewer, osg::Group *root, InterfaceSigEventCallBack* iSigEventCallBack)
{
	CBaseUserEvent* ptMeasureEvent = new COsgMeasurePtEvent(viewer, root, iSigEventCallBack);
	CBaseUserEvent* lineMeasureEvent = new COsgMeasureLineEvent(viewer, root, iSigEventCallBack);
	CBaseUserEvent* multiLineMeasureEvent = new COsgMeasureMultiLineEvent(viewer, root, iSigEventCallBack);
	CBaseUserEvent* areaMeasureEvent = new COsgMeasureAreaEvent(viewer, root, iSigEventCallBack);
	vecMeasureEvent.push_back(ptMeasureEvent);
	vecMeasureEvent.push_back(lineMeasureEvent);
	vecMeasureEvent.push_back(multiLineMeasureEvent);
	vecMeasureEvent.push_back(areaMeasureEvent);
}


void COsgMeasureEvent::addEventToViewer(osg::ref_ptr<osgViewer::Viewer> viewer)
{
	for (int i = 0; i < vecMeasureEvent.size(); i++)
	{
		viewer->addEventHandler(vecMeasureEvent[i]);
	}
}