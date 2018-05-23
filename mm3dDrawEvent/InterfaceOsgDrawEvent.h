#pragma once
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"

#ifdef DRAWEVENTLIBDLL  
#define DRAWEVENTAPI _declspec(dllexport)  
#else  
#define DRAWEVENTAPI  _declspec(dllimport)  
#endif  


class DRAWEVENTAPI InterfaceOsgDrawEvent
{
public:
	virtual void addEventToViewer(osg::ref_ptr<osgViewer::Viewer> viewer) = 0;
};


class DRAWEVENTAPI InterfaceOsgDrawEventFactory
{
public:
	virtual InterfaceOsgDrawEvent* create(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editSceneGroup, osg::Group* tempGroup,
		osg::Group* mapGroup);
};
