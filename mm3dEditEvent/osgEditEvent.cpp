// mm3dEditEvent.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "osgEditEvent.h"
#include "osgClipByHeightEvent.h"
#include "osgClipByPolygonEvent.h"
#include "osgHollowOutByPolygonEvent.h"
#include "osgHollowOutSuspendEvent.h"
#include "osgRepairHoleEvent.h"
#include "osgSelTileRemoveEvent.h"

COsgEditEvent::COsgEditEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editGroup, osg::Group* tempGroup, osg::Group* scopeGroup, osg::Group* dragGroup,
	InterfaceSigEventCallBack* iSigEventCallBack)
{
	CBaseUserEvent* clipByHeightEvent = new COsgClipByHeightEvent(viewer, root, iSigEventCallBack);
	CBaseUserEvent* clipByPolygonEvent = new COsgClipByPolygonEvent(viewer, root, iSigEventCallBack);
	CBaseUserEvent* hollowOutByPolygonEvent = new COsgHollowOutByPolygonEvent(viewer, root, iSigEventCallBack);
	CBaseUserEvent* hollowOutSuspendEvent = new COsgHollowOutSuspendEvent(viewer, root, dragGroup, tempGroup, scopeGroup, iSigEventCallBack);
	CBaseUserEvent* repairHoleEvent = new COsgRepairHoleEvent(viewer, root, iSigEventCallBack);
	CBaseUserEvent* selTileRemoveEvent = new COsgSelTileRemoveEvent(viewer, root, iSigEventCallBack);

	vecEditEvent.push_back(clipByHeightEvent);
	vecEditEvent.push_back(clipByPolygonEvent);
	vecEditEvent.push_back(hollowOutByPolygonEvent);
	vecEditEvent.push_back(hollowOutSuspendEvent);
	vecEditEvent.push_back(repairHoleEvent);
	vecEditEvent.push_back(selTileRemoveEvent);
}

void COsgEditEvent::RepairHole()
{
	for (int i = 0; i < vecEditEvent.size(); i++)
	{
		COsgRepairHoleEvent* repairHoleEvent = dynamic_cast<COsgRepairHoleEvent*> (vecEditEvent[i]);

		if (repairHoleEvent)
		{
			repairHoleEvent->StartRepairHole();
			break;
		}
	}
}

void COsgEditEvent::addEventToViewer(osg::ref_ptr<osgViewer::Viewer> viewer)
{
	for (int i = 0; i < vecEditEvent.size(); i++)
	{
		viewer->addEventHandler(vecEditEvent[i]);
	}
}

void COsgEditEvent::ClipHeight()
{
	for (int i = 0; i < vecEditEvent.size(); i++)
	{
		COsgClipByHeightEvent* clipHeightEvent = dynamic_cast<COsgClipByHeightEvent*> (vecEditEvent[i]);

		if (clipHeightEvent)
		{
			clipHeightEvent->ClipHeight();
			break;
		}
	}
}


void COsgEditEvent::InvertClipByHeight()
{
	for (int i = 0; i < vecEditEvent.size(); i++)
	{
		COsgClipByHeightEvent* clipHeightEvent = dynamic_cast<COsgClipByHeightEvent*> (vecEditEvent[i]);

		if (clipHeightEvent)
		{
			clipHeightEvent->InvertClipByHeight();
			break;
		}
	}
}

void COsgEditEvent::ClipByPolygon(std::string inFilePath)
{
	for (int i = 0; i < vecEditEvent.size(); i++)
	{
		COsgClipByPolygonEvent* clipByPolygonEvent = dynamic_cast<COsgClipByPolygonEvent*> (vecEditEvent[i]);

		if (clipByPolygonEvent)
		{
			clipByPolygonEvent->ClipByPolygon(inFilePath);
			break;
		}
	}
}

void COsgEditEvent::HollowOutByPolygon(std::string inFilePath)
{
	for (int i = 0; i < vecEditEvent.size(); i++)
	{
		COsgHollowOutByPolygonEvent* hollowOutByPolygonEvent = dynamic_cast<COsgHollowOutByPolygonEvent*>(vecEditEvent[i]);

		if (hollowOutByPolygonEvent)
		{
			hollowOutByPolygonEvent->HollowOutByPolygon(inFilePath);
			break;
		}
	}
}

void COsgEditEvent::HollowOutSuspend(std::string inFilePath)
{
	for (int i = 0; i < vecEditEvent.size(); i++)
	{
		COsgHollowOutSuspendEvent* hollowOutSuspendEvent = dynamic_cast<COsgHollowOutSuspendEvent*>(vecEditEvent[i]);

		if (hollowOutSuspendEvent)
		{
			hollowOutSuspendEvent->HollowOutSuspend(inFilePath);
			break;
		}
	}
}