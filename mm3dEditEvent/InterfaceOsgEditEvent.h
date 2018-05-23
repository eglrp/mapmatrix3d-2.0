#pragma once
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"


#ifdef EDITEVENTLIBDLL  
#define EDITEVENTAPI _declspec(dllexport)  
#else  
#define EDITEVENTAPI  _declspec(dllimport)  
#endif 

class EDITEVENTAPI InterfaceOsgEditEvent
{
public:
	virtual void RepairHole() = 0;

	virtual void addEventToViewer(osg::ref_ptr<osgViewer::Viewer> viewer) = 0;

	virtual void ClipHeight() = 0;

	virtual void InvertClipByHeight() = 0; 

	virtual void ClipByPolygon(std::string inFilePath) = 0;

	virtual	void HollowOutByPolygon(std::string inFilePath) = 0;

	virtual void HollowOutSuspend(std::string inFilePath) = 0;
};

class EDITEVENTAPI InterfaceOsgEditEventFactory
{
public:
	InterfaceOsgEditEvent* create(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editGroup, osg::Group* tempGroup, osg::Group* scopeGroup, osg::Group* dragGroup,
		InterfaceSigEventCallBack* iSigEventCallBack);
};

