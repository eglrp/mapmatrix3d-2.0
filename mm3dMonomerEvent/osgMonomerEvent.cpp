// mm3dMonomerEvent.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "osgMonomerEvent.h"
#include "osgExtractObjectByPolygonEvent.h"
#include "osgFlatSceneByPolygonEvent.h"
#include "osgHighLightObjectByClickEvent.h"
#include "osgLinkPropertyWithObjectEvent.h"
#include "osgManualExtractTextureEvent.h"
#include "osgQuickClipModelByTriangleEvent.h"
#include "osgQuickDeleteSingleTriangleByLineEvent.h"
#include "osgQuickDeleteSingleTriangleEvent.h"
#include "osgQuickDeleteTriangleBySolidEvent.h"
#include "osgSimplifyObjectByPolygonEvent.h"
#include "osgFlatSceneByPolygonEvent.h"

COsgMonomerEvent::COsgMonomerEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> EditGroup, osg::ref_ptr<osg::Group> ObjectGroup,
	InterfaceSigEventCallBack* iSigEventCallBack)
{
	CBaseUserEvent* extractTextureEvent = new COsgManualExtractTextureEvent(viewer, root, EditGroup, iSigEventCallBack);
	CBaseUserEvent* extractObjectEvent = new COsgExtractObjectByPolygonEvent(viewer, root, EditGroup, ObjectGroup, iSigEventCallBack);
	CBaseUserEvent* quickClipModelByTriangleEvent = new COsgQuickClipModelByTriangleEvent(viewer, root, EditGroup, iSigEventCallBack);
	CBaseUserEvent* quickDeleteSingleTriangleEvent = new COsgQuickDeleteSingleTriangleEvent(viewer, root, EditGroup, iSigEventCallBack);
	CBaseUserEvent* quickDeleteSingleTriangleByLineEvent = new COsgQuickDeleteSingleTriangleByLineEvent(viewer, root, EditGroup, iSigEventCallBack);
	CBaseUserEvent* highLightObjectByClickEvent = new COsgHighLightObjectByClickEvent(viewer, root, EditGroup, iSigEventCallBack);
	CBaseUserEvent* linkPropertyWithObjectEvent = new COsgLinkPropertyWithObjectEvent(viewer, root, EditGroup, iSigEventCallBack);
	CBaseUserEvent* osgSimplifyObjectByPolygonEvent = new COsgSimplifyObjectByPolygonEvent(viewer, root, ObjectGroup, iSigEventCallBack);
	CBaseUserEvent* osgFlatSceneByPolygonEvent = new COsgFlatSceneByPolygonEvent(viewer, root, EditGroup, iSigEventCallBack);

	vecMonomerEvent.push_back(extractTextureEvent);
	vecMonomerEvent.push_back(extractObjectEvent);
	vecMonomerEvent.push_back(quickClipModelByTriangleEvent);
	vecMonomerEvent.push_back(quickDeleteSingleTriangleEvent);
	vecMonomerEvent.push_back(quickDeleteSingleTriangleByLineEvent);
	vecMonomerEvent.push_back(highLightObjectByClickEvent);
	vecMonomerEvent.push_back(linkPropertyWithObjectEvent);
	vecMonomerEvent.push_back(osgSimplifyObjectByPolygonEvent);
	vecMonomerEvent.push_back(osgFlatSceneByPolygonEvent);
}


void COsgMonomerEvent::addEventToViewer(osg::ref_ptr<osgViewer::Viewer> viewer)
{
	for (int i = 0; i < vecMonomerEvent.size(); i++)
	{
		viewer->addEventHandler(vecMonomerEvent[i]);
	}
}

void COsgMonomerEvent::extractObject(std::string inFilePathName)																					//接口， 单体化物体
{
	for (int i = 0; i < vecMonomerEvent.size(); i++)
	{
		COsgExtractObjectByPolygonEvent* extractObjectEvent = dynamic_cast<COsgExtractObjectByPolygonEvent*> (vecMonomerEvent[i]);

		if (extractObjectEvent)
		{
			extractObjectEvent->ExtractObject(inFilePathName);
			break;
		}
	}
}

void COsgMonomerEvent::extractArea(std::string inFilePathName)
{
	for (int i = 0; i < vecMonomerEvent.size(); i++)
	{
		COsgExtractObjectByPolygonEvent* extractObjectEvent = dynamic_cast<COsgExtractObjectByPolygonEvent*> (vecMonomerEvent[i]);

		if (extractObjectEvent)
		{
			extractObjectEvent->ExtractArea(inFilePathName);
			break;
		}
	}
}

void COsgMonomerEvent::flatByPolygon(std::string inFilePath)
{
	for (int i = 0; i < vecMonomerEvent.size(); i++)
	{
		COsgFlatSceneByPolygonEvent* flatObjectEvent = dynamic_cast<COsgFlatSceneByPolygonEvent*> (vecMonomerEvent[i]);

		if (flatObjectEvent)
		{
			flatObjectEvent->FlatByPolygon(inFilePath);
			break;
		}
	}
}

void COsgMonomerEvent::setObjectProperty(std::string strObjectProperty)
{
	for (int i = 0; i < vecMonomerEvent.size(); i++)
	{
		COsgLinkPropertyWithObjectEvent* linkPropertyEvent = dynamic_cast<COsgLinkPropertyWithObjectEvent*> (vecMonomerEvent[i]);

		if (linkPropertyEvent)
		{
			linkPropertyEvent->SetObjectProperty(strObjectProperty);
			break;
		}
	}
}

void COsgMonomerEvent::lookUpOjbectProperty(std::string objectPropertyName, std::string objectPropertyValue)
{
	for (int i = 0; i < vecMonomerEvent.size(); i++)
	{
		COsgLinkPropertyWithObjectEvent* linkPropertyEvent = dynamic_cast<COsgLinkPropertyWithObjectEvent*> (vecMonomerEvent[i]);

		if (linkPropertyEvent)
		{
			linkPropertyEvent->LookUpOjbectProperty(objectPropertyName, objectPropertyValue);
			break;
		}
	}
}

void COsgMonomerEvent::outputModelWithoutTexture(std::string outModelWithoutFileName)
{
	for (int i = 0; i < vecMonomerEvent.size(); i++)
	{
		COsgHighLightObjectByClickEvent* osgHighLightEvent = dynamic_cast<COsgHighLightObjectByClickEvent*> (vecMonomerEvent[i]);

		if (osgHighLightEvent)
		{
			osgHighLightEvent->outputModelWithoutTexture(outModelWithoutFileName);
			break;
		}
	}
}

void COsgMonomerEvent::outputTextureModel(std::string outModelWithoutFileName)
{
	for (int i = 0; i < vecMonomerEvent.size(); i++)
	{
		COsgHighLightObjectByClickEvent* osgHighLightEvent = dynamic_cast<COsgHighLightObjectByClickEvent*> (vecMonomerEvent[i]);

		if (osgHighLightEvent)
		{
			osgHighLightEvent->outputTextureModel(outModelWithoutFileName);
			break;
		}
	}
}

void COsgMonomerEvent::removeSelPagedLod()
{
	for (int i = 0; i < vecMonomerEvent.size(); i++)
	{
		COsgHighLightObjectByClickEvent* osgHighLightEvent = dynamic_cast<COsgHighLightObjectByClickEvent*> (vecMonomerEvent[i]);

		if (osgHighLightEvent)
		{
			osgHighLightEvent->removeSelPagedLod();
			break;
		}
	}
}