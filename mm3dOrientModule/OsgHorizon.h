#pragma once
#include "OSG_Plus.h"
#include "InterfaceOsgHorizon.h"


class COsgHorizon : public InterfaceOsgHorizon
{
public:
	void ModelHorizontalization(osg::ref_ptr<osg::Group> &sceneGroup);				//ÖÃÆ½³¡¾°

	void ModelMirror(osg::ref_ptr<osg::Group> &sceneGroup);							//³¡¾°¾µÏñ

	void ModelUpsideDown(osg::ref_ptr<osg::Group> &sceneGroup);						//³¡¾°µßµ¹
};

