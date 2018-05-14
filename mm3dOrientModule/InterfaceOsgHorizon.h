#pragma once
#include "OSG_Plus.h"

#ifdef HORIZONLIBDLL  
#define HORIZONAPI _declspec(dllexport)  
#else  
#define HORIZONAPI  _declspec(dllimport)  
#endif  

class HORIZONAPI InterfaceOsgHorizon
{
public:
	virtual void ModelHorizontalization(osg::ref_ptr<osg::Group> &sceneGroup) = 0;				//ÖÃÆ½³¡¾°

	virtual void ModelMirror(osg::ref_ptr<osg::Group> &sceneGroup) = 0;							//³¡¾°¾µÏñ

	virtual void ModelUpsideDown(osg::ref_ptr<osg::Group> &sceneGroup) = 0;						//³¡¾°µßµ¹
};


class HORIZONAPI InterfaceOsgHorizonFactory
{
public:
	InterfaceOsgHorizon* create();
};

