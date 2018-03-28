#pragma once
#include "BaseUserEvent.h"
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceOsgEditEvent.h"

class COsgEditEvent : public InterfaceOsgEditEvent
{
public:
	COsgEditEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editGroup, osg::Group* tempGroup, osg::Group* scopeGroup, osg::Group* dragGroup,
		InterfaceSigEventCallBack* iSigEventCallBack);

	virtual void addEventToViewer(osg::ref_ptr<osgViewer::Viewer> viewer);

	virtual void ClipHeight();

	virtual void InvertClipByHeight();

	virtual void ClipByPolygon(std::string inFilePath);

	virtual	void HollowOutByPolygon(std::string inFilePath);

	virtual void HollowOutSuspend(std::string inFilePath);

private:
	std::vector<CBaseUserEvent*> vecEditEvent;
};