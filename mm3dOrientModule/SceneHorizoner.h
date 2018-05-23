#pragma once
#include "OSG_Plus.h"

class CSceneHorizoner
{
public:
	CSceneHorizoner(void);
	~CSceneHorizoner(void);
	//接口
	void StartModelHorizontalization(osg::ref_ptr<osg::Group>& sceneGroup);				//开始进行场景置平

	//接口	
	void ModelMirror(osg::ref_ptr<osg::Group> &ceneGroup);								//场景镜像

	//接口
	void ModelUpsideDown(osg::ref_ptr<osg::Group> &sceneGroup);							//场景颠倒

private:
	void CalculateSceneRotMat(osg::ref_ptr<osg::Node> sceneNode, osg::Matrix &matrix);	//计算场景的旋转矩阵
};

