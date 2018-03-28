#pragma once
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"

#ifdef MEASUREEVENTLIBDLL  
#define MEASUREEVENTAPI _declspec(dllexport)  
#else  
#define MEASUREEVENTAPI  _declspec(dllimport)  
#endif 


class MEASUREEVENTAPI InterfaceOsgMeasureEvent
{
public:
	virtual void addEventToViewer(osg::ref_ptr<osgViewer::Viewer> viewer) = 0;
};


class MEASUREEVENTAPI InterfaceOsgMeasureEventFactory
{
public:
	InterfaceOsgMeasureEvent* create(osgViewer::Viewer *viewer, osg::Group *root, InterfaceSigEventCallBack* iSigEventCallBack);
};
