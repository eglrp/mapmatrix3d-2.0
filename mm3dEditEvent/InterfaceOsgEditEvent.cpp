
#include "stdafx.h"
#include "InterfaceOsgEditEvent.h"
#include "osgEditEvent.h"

InterfaceOsgEditEvent* InterfaceOsgEditEventFactory::create(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editGroup, 
	osg::Group* tempGroup, osg::Group* scopeGroup, osg::Group* dragGroup, InterfaceSigEventCallBack* iSigEventCallBack)
{
	return new COsgEditEvent(viewer, root, editGroup, tempGroup, scopeGroup, dragGroup, iSigEventCallBack);
}

