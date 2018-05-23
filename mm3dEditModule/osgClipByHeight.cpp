// osgClipByHeight.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "osgClipByHeight.h"

void COsgClipByHeight::StartClipSceneByHeight(double clipHeight, double reFactor, osg::ref_ptr<osg::Group> sceneGroup)
{
	//进行裁剪
	double cM = -1 / clipHeight;
	osg::ref_ptr<osg::ClipPlane> clipPlane = new osg::ClipPlane(0, osg::Vec4d(0, 0, cM, 1));

	//反选因子
	osg::Vec4d planePara(0, 0, cM, 1);
	CalClipPlanePara(planePara, clipHeight, reFactor);

	clipPlane->setClipPlane(planePara);
	osg::ref_ptr<osg::Node> oriNode = sceneGroup->getChild(0);
	osg::ref_ptr<osg::ClipNode> clipNode = dynamic_cast<osg::ClipNode*> (sceneGroup->getChild(0));
	int num = 0;

	if (!clipNode)
	{
		clipNode = new osg::ClipNode();
		clipNode->addClipPlane(clipPlane);
		clipNode->addChild(oriNode);
		sceneGroup->replaceChild(oriNode, clipNode);
	}
	else																				//增加裁剪面
	{
		int clipNum = clipNode->getNumClipPlanes();
		clipPlane->setClipPlaneNum(clipNum);
		clipNode->addClipPlane(clipPlane);
	}

	//再设置一次,实时显示
	SetClipNodeClipPlane(clipNode, planePara);
}

void COsgClipByHeight::ResetClipHeightAndRefactor(double clipHeight, double reFactor, osg::ref_ptr<osg::ClipNode> clipNode)
{
	if (clipNode)
	{
		osg::Vec4d planePara(0, 0, 0, 0);
		CalClipPlanePara(planePara, clipHeight, reFactor);
		SetClipNodeClipPlane(clipNode, planePara);
	}
}

void COsgClipByHeight::CalClipPlanePara(osg::Vec4d &planePara, double clipHeight, double reFactor)
{
	//反选因子
	double D;
	double cM = -1 / clipHeight;

	if (clipHeight > 0)
	{
		cM = cM * -1 * reFactor;
		D = -1 * reFactor;
	}
	else
	{
		cM = cM * reFactor;
		D = 1 * reFactor;
	}

	planePara.x() = 0;
	planePara.y() = 0;
	planePara.z() = cM;
	planePara.w() = D;
}

void COsgClipByHeight::SetClipNodeClipPlane(osg::ref_ptr<osg::ClipNode> clipNode, osg::Vec4d planePara)
{
	int numClipPlane = clipNode->getNumClipPlanes();
	osg::ref_ptr<osg::ClipPlane> plane = clipNode->getClipPlane(numClipPlane - 1);
	plane->setClipPlane(planePara);
}