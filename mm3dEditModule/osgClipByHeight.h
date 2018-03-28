#pragma once
#include "InterfaceOsgClipByHeight.h"

class COsgClipByHeight : public InterfaceOsgClipByHeight
{
public:
	virtual void StartClipSceneByHeight(double clipHeight, double reFactor, osg::ref_ptr<osg::Group> sceneGroup);

	virtual void ResetClipHeightAndRefactor(double clipHeight, double reFactor, osg::ref_ptr<osg::ClipNode> clipNode);

protected:
	virtual void CalClipPlanePara(osg::Vec4d &planePara, double clipHeight, double reFactor);

	virtual void SetClipNodeClipPlane(osg::ref_ptr<osg::ClipNode> clipNode, osg::Vec4d planePara);

};