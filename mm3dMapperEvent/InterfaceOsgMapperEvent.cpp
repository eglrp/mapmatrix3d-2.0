#include "stdafx.h"
#include "InterfaceOsgMapperEvent.h"
#include "osgMapperEvent.h"


InterfaceOsgMapperEvent* InterfaceOsgMapperEventFactory::create(osgViewer::Viewer *viewer, osg::Group *root, osg::ref_ptr<osg::Group> editGroup, osg::ref_ptr<osg::Group> interfaceGroup, 
	osg::ref_ptr<osg::Group> TempGroup, osg::ref_ptr<osg::Group> linkGroup, osg::ref_ptr<osg::Group> DataGroup, InterfaceSigEventCallBack* iSigEventCallBack)
{
	return new COsgMapperEvent(viewer, root, editGroup, interfaceGroup, TempGroup, linkGroup, DataGroup, iSigEventCallBack);
}