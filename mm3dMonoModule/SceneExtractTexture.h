#pragma once
#include "OSG_Plus.h"
#include <vector>
#include "cvLine.h"
#include "publicDefined.h"

class CSceneExtractTexture
{
public:
	osg::ref_ptr<osg::Node> extractTexture(std::vector<osg::Vec3d> VecWorldPt, osg::ref_ptr<osg::Node> sceneNode, double Res);

	osg::ref_ptr<osg::Image> ExtractTextureImage(std::vector<osg::Vec3d> VecWorldPt, osg::ref_ptr<osg::Node> sceneNode, double Res);

protected:
	osg::ref_ptr<osg::Node> ProjectTextureInRect(osg::ref_ptr<osg::Node> sceneNode);

	osg::ref_ptr<osg::Node> createOsgbTextureModel(std::string coordTxtFileName, std::string imageFileName);

	bool FindBottomLevelOsgbModelInRect(std::vector<CScope> vecScope, osg::ref_ptr<osg::Group> mGroup, osg::Matrix worldMatrix, osg::ref_ptr<osg::Group> group);

	osg::Matrix StartCaculateRotationMatrix(std::vector<osg::Vec3d> vecWorldPt);

	osg::ref_ptr<osg::Image> ExtractTextureInRect(osg::ref_ptr<osg::Node> sceneNode, std::string outImgFileName);

protected:
	double res;

	std::vector<osg::Vec3d> vecWorldPt;
};

	