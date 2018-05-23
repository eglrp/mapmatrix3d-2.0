// mm3dDrawEvent.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "osgDrawEvent.h"
#include "osgDrawSinglePtHandler.h"
#include "osgDrawSingleLineEvent.h"
#include "osgDrawRectangleEvent.h"
#include "osgDrawMultiSingleLineEvent.h"
#include "osgDrawMultiPtEvent.h"
#include "osgDrawMultiLineEvent.h"
#include "osgDrawMultiLineHandEvent.h"
#include "osgDrawMultiClosedLineEvent.h"
#include "osgDrawFlatPlaneEvent.h"
#include "osgDrawClosedLineHandEvent.h"
#include "OsgDrawClosedLineForMeasureAreaEvent.h"
#include "osgDrawClosedLineEvent.h"
#include "OsgDrawClosedLineDBWithPointEvent.h"
#include "osgDrawClosedLineDBclickEvent.h"
#include "osgDrawRoadPathEvent.h"

COsgDrawEvent::COsgDrawEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editSceneGroup, osg::Group* tempGroup, osg::Group* mapGroup)
{
	CBaseUserEvent* drawSinglePtEvent = new COsgDrawSinglePtHandler(viewer, root, editSceneGroup);
	CBaseUserEvent* drawSingleLineEvent = new COsgDrawSingleLineEvent(viewer, root, editSceneGroup);
	CBaseUserEvent* drawRectangleEvent = new COsgDrawRectangleEvent(viewer, root, editSceneGroup);
	CBaseUserEvent* drawMultiSingleLineEvent = new COsgDrawMultiSingleLineEvent(viewer, root, editSceneGroup);
	CBaseUserEvent* drawMultiPtEvent = new COsgDrawMultiPtEvent(viewer, root, editSceneGroup);
	CBaseUserEvent* drawMultiLineEvent = new COsgDrawMultiLineEvent(viewer, root, editSceneGroup);
	CBaseUserEvent* drawMultiLineHandEvent = new COsgDrawMultiLineHandEvent(viewer, root, editSceneGroup, tempGroup);
	CBaseUserEvent* drawMultiClosedLineEvent = new COsgDrawMultiClosedLineEvent(viewer, root, editSceneGroup, tempGroup);
	CBaseUserEvent* drawMultiLineForMultiLineEvent = new COsgDrawMultiLineForMultiLineEvent(viewer, root, editSceneGroup, tempGroup);
	
	CBaseUserEvent* drawFlatPlaneEvnet = new COsgDrawFlatPlaneEvent(viewer, root, editSceneGroup);
	CBaseUserEvent* drawClosedLineHandEvent = new COsgDrawClosedLineHandEvent(viewer, root, editSceneGroup, tempGroup);
	CBaseUserEvent* drawClosedLineForMeasureAreaEvent = new COsgDrawClosedLineForMeasureAreaEvent(viewer, root, editSceneGroup, tempGroup);
	CBaseUserEvent* drawClosedLineEvent = new COsgDrawClosedLineEvent(viewer, root, editSceneGroup, tempGroup);
	CBaseUserEvent* drawClosedLineDbWithPointEvent = new COsgDrawClosedLineDBWithPointEvent(viewer, root, editSceneGroup, tempGroup);
	CBaseUserEvent* drawClosedLineDbClickEvent = new COsgDrawClosedLineDBclickEvent(viewer, root, editSceneGroup, tempGroup);
	CBaseUserEvent* drawRoadPathEvent = new COsgDrawRoadPathEvent(viewer, root, editSceneGroup);

	vecDrawEvent.push_back(drawSinglePtEvent);
	vecDrawEvent.push_back(drawSingleLineEvent);
	vecDrawEvent.push_back(drawRectangleEvent);
	vecDrawEvent.push_back(drawMultiSingleLineEvent);
	vecDrawEvent.push_back(drawMultiPtEvent);
	vecDrawEvent.push_back(drawMultiLineEvent);
	vecDrawEvent.push_back(drawMultiLineHandEvent);
	vecDrawEvent.push_back(drawMultiClosedLineEvent);
	vecDrawEvent.push_back(drawMultiLineForMultiLineEvent);
	vecDrawEvent.push_back(drawFlatPlaneEvnet);
	vecDrawEvent.push_back(drawClosedLineHandEvent);
	vecDrawEvent.push_back(drawClosedLineForMeasureAreaEvent);
	vecDrawEvent.push_back(drawClosedLineEvent);
	vecDrawEvent.push_back(drawClosedLineDbWithPointEvent);
	vecDrawEvent.push_back(drawClosedLineDbClickEvent);
	vecDrawEvent.push_back(drawRoadPathEvent);
}

void COsgDrawEvent::addEventToViewer(osg::ref_ptr<osgViewer::Viewer> viewer)
{
	for (int i = 0; i < vecDrawEvent.size(); i++)
	{
		viewer->addEventHandler(vecDrawEvent[i]);
	}
}
