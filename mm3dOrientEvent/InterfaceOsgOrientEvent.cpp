#include "stdafx.h"
#include "InterfaceOsgOrientEvent.h"
#include "osgOrientEvent.h"

InterfaceOsgOrientEvent* InterfaceOsgOrientEventFactory::create(osgViewer::Viewer *viewer, osg::Group *root, osg::Group *editGroup, 
	InterfaceSigEventCallBack* iSigEventCallBack)
{
	return new COsgOrientEvent(viewer, root, editGroup, iSigEventCallBack);
}


