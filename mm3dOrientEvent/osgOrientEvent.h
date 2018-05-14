#pragma once
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"
#include "BaseUserEvent.h"
#include "InterfaceOsgOrientEvent.h"


class COsgOrientEvent : public InterfaceOsgOrientEvent
{
public:
	COsgOrientEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group *editGroup, InterfaceSigEventCallBack* iSigEventCallBack);

	virtual void addEventToViewer(osg::ref_ptr<osgViewer::Viewer> viewer);

	virtual bool absOrient(std::string inFilePath);

	virtual void loadControlPt();

	virtual void reClickCtrlPt(int ptNum);

	virtual void reSetCtrlPtCheckStatus(int ptNum, bool isCheckPt);

	virtual void matchSingleModel();

private:
	std::vector<CBaseUserEvent*> vecOriEvent;

};