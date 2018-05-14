// mm3dOrientEvent.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "osgOrientEvent.h"
#include "osgAbsOrientEvent.h"
#include "osgMoveWholeSceneXY.h"
#include "osgMoveWholeSceneZ.h"
#include "osgSceneMatchEvent.h"
#include "osgHorizontalizationAccurateEvent.h"

COsgOrientEvent::COsgOrientEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group *editGroup, 
	InterfaceSigEventCallBack* iSigEventCallBack)
{
	CBaseUserEvent* osgAbsOriEvent = new COsgAbsOrientEvent(viewer, root, editGroup, iSigEventCallBack);
	CBaseUserEvent* osgMoveWholeSceneXYEvent = new COsgMoveWholeSceneXYEvent(viewer, root, iSigEventCallBack);
	CBaseUserEvent* osgMoveWholeSceneZEvent = new COsgMoveWholeSceneZEvent(viewer, root, iSigEventCallBack);
	CBaseUserEvent* osgSceneMatchEvent = new COsgSceneMatchEvent(viewer, root, editGroup, iSigEventCallBack);
	CBaseUserEvent* osgHorizontalization = new COsgHorizontalizationAccurateEvent(viewer,root,editGroup,iSigEventCallBack);

	vecOriEvent.push_back(osgAbsOriEvent);
	vecOriEvent.push_back(osgMoveWholeSceneXYEvent);
	vecOriEvent.push_back(osgMoveWholeSceneZEvent);
	vecOriEvent.push_back(osgSceneMatchEvent);
	vecOriEvent.push_back(osgHorizontalization);
}

bool COsgOrientEvent::absOrient(std::string inFilePath)
{
	for (int i = 0; i < vecOriEvent.size(); i++)
	{
		COsgAbsOrientEvent* absOriEvent = dynamic_cast<COsgAbsOrientEvent*> (vecOriEvent[i]);

		if (absOriEvent)
		{
			absOriEvent->AbsOrient(inFilePath);
			break;
		}
	}
}

void COsgOrientEvent::loadControlPt()
{
	for (int i = 0; i < vecOriEvent.size(); i++)
	{
		COsgAbsOrientEvent* absOriEvent = dynamic_cast<COsgAbsOrientEvent*> (vecOriEvent[i]);

		if (absOriEvent)
		{
			absOriEvent->LoadControlPt();
			break;
		}
	}
}

void COsgOrientEvent::reClickCtrlPt(int ptNum)
{
	for (int i = 0; i < vecOriEvent.size(); i++)
	{
		COsgAbsOrientEvent* absOriEvent = dynamic_cast<COsgAbsOrientEvent*> (vecOriEvent[i]);

		if (absOriEvent)
		{
			absOriEvent->ReclickCtrlPt(ptNum);
			break;
		}
	}
}

void COsgOrientEvent::reSetCtrlPtCheckStatus(int ptNum, bool isCheckPt)
{
	for (int i = 0; i < vecOriEvent.size(); i++)
	{
		COsgAbsOrientEvent* absOriEvent = dynamic_cast<COsgAbsOrientEvent*> (vecOriEvent[i]);

		if (absOriEvent)
		{
			absOriEvent->ReSetCtrlPtCheckStatus(ptNum, isCheckPt);
			break;
		}
	}
}

void COsgOrientEvent::matchSingleModel()
{
	for (int i = 0; i < vecOriEvent.size(); i++)
	{
		COsgSceneMatchEvent* matchEvent = dynamic_cast<COsgSceneMatchEvent*> (vecOriEvent[i]);

		if (matchEvent)
		{
			matchEvent->MatchSingleModel();
			break;
		}
	}
}


void COsgOrientEvent::addEventToViewer(osg::ref_ptr<osgViewer::Viewer> viewer)
{
	for (int i = 0; i < vecOriEvent.size(); i++)
	{
		viewer->addEventHandler(vecOriEvent[i]);
	}
}
