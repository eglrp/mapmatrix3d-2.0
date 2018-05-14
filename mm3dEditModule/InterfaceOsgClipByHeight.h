#pragma once
#include "OSG_Plus.h"

#ifdef OSGCLIPBYHEIGHTLIBDLL  
#define OSGCLIPBYHEIGHTAPI _declspec(dllexport)  
#else  
#define OSGCLIPBYHEIGHTAPI  _declspec(dllimport)  
#endif  

class OSGCLIPBYHEIGHTAPI InterfaceOsgClipByHeight
{
public:
	virtual void StartClipSceneByHeight(double clipHeight, double reFactor, osg::ref_ptr<osg::Group> sceneGroup) = 0;

	virtual void ResetClipHeightAndRefactor(double clipHeight, double reFactor, osg::ref_ptr<osg::ClipNode> clipNode) = 0;
};

class OSGCLIPBYHEIGHTAPI InterfaceOsgClipByHeightFactory
{
public:
	InterfaceOsgClipByHeight* create();
};