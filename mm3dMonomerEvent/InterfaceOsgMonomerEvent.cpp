#include "stdafx.h"
#include "InterfaceOsgMonomerEvent.h"
#include "osgMonomerEvent.h"


InterfaceOsgMonomerEvent* InterfaceOsgMonomerEventFactory::create(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> EditGroup, osg::ref_ptr<osg::Group> ObjectGroup,
	InterfaceSigEventCallBack* iSigEventCallBack)
{
	return new COsgMonomerEvent(viewer, root, EditGroup, ObjectGroup, iSigEventCallBack);
}
