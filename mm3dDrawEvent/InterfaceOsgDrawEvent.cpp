#include "stdafx.h"
#include "InterfaceOsgDrawEvent.h"
#include "osgDrawEvent.h"

InterfaceOsgDrawEvent* InterfaceOsgDrawEventFactory::create(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editSceneGroup,
	osg::Group* tempGroup, osg::Group* mapGroup)
{
	return new COsgDrawEvent(viewer, root, editSceneGroup, tempGroup, mapGroup);
}