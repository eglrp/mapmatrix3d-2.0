#include "stdafx.h"
#include "InterfaceOsgMeasureEvent.h"
#include "OsgMeasureEvent.h"

InterfaceOsgMeasureEvent* InterfaceOsgMeasureEventFactory::create(osgViewer::Viewer *viewer, osg::Group *root, 
	InterfaceSigEventCallBack* iSigEventCallBack)
{
	return new COsgMeasureEvent(viewer, root, iSigEventCallBack);
}