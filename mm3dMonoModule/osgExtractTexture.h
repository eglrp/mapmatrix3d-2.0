#pragma once
#include "InterfaceOsgExtractTexture.h"
#include "SceneExtractTexture.h"

class COsgExtractTexture : public InterfaceOsgExtractTexture
{
public:
	virtual osg::ref_ptr<osg::Node> ExtractTexture(std::vector<osg::Vec3d> vecWorldPt, osg::ref_ptr<osg::Node> sceneNode, double res);	//提取纹理

	virtual osg::ref_ptr<osg::Image> ExtractTextureImage(std::vector<osg::Vec3d> vecWorldPt, osg::ref_ptr<osg::Node> sceneNode, double res); //提取纹理影像
};