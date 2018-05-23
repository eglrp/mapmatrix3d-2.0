#include "stdafx.h"
#include "OsgHorizon.h"
#include "SceneHorizoner.h"

void COsgHorizon::ModelHorizontalization(osg::ref_ptr<osg::Group>& sceneGroup)
{
	CSceneHorizoner sceneHorizon;
	sceneHorizon.StartModelHorizontalization(sceneGroup);
}

void COsgHorizon::ModelMirror(osg::ref_ptr<osg::Group> &sceneGroup)
{
	CSceneHorizoner sceneHorizon;
	sceneHorizon.ModelMirror(sceneGroup);
}

void COsgHorizon::ModelUpsideDown(osg::ref_ptr<osg::Group> &sceneGroup)
{
	CSceneHorizoner sceneHorizon;
	sceneHorizon.ModelUpsideDown(sceneGroup);
}