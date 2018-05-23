#pragma once
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"

#ifdef ORIEVENTLIBDLL  
#define ORIEVENTAPI _declspec(dllexport)  
#else  
#define ORIEVENTAPI  _declspec(dllimport)  
#endif 

class ORIEVENTAPI InterfaceOsgOrientEvent
{
public:
	virtual void addEventToViewer(osg::ref_ptr<osgViewer::Viewer> viewer) = 0;

	virtual bool absOrient(std::string inFilePath) = 0;

	virtual void loadControlPt() = 0;

	virtual void reClickCtrlPt(int ptNum) = 0;

	virtual void reSetCtrlPtCheckStatus(int ptNum, bool isCheckPt) = 0;

	virtual void matchSingleModel() = 0;
};



class ORIEVENTAPI InterfaceOsgOrientEventFactory
{
public:
	InterfaceOsgOrientEvent* create(osgViewer::Viewer *viewer, osg::Group *root, osg::Group *editGroup,
		InterfaceSigEventCallBack* iSigEventCallBack);
};
