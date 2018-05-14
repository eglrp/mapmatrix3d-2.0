#pragma once
#include "OSG_Plus.h"
#include "publicDefined.h"

class CAccurateClipper
{
public:
	CAccurateClipper(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope)
	{
		this->sceneNode = sceneNode;
		this->vecScope = vecScope;
	}

	virtual bool AccurateClipBottomScene(osg::ref_ptr<osg::MatrixTransform> &mTrans, bool bAccurate = true);

	virtual bool clipBottomScene(osg::ref_ptr<osg::MatrixTransform> &mTrans);

protected:
	virtual bool FindBottomLevelOsgbModelInRect(osg::ref_ptr<osg::MatrixTransform> mTrans, osg::ref_ptr<osg::Group> group);

	virtual void StartAccurateClipBottomScene(osg::ref_ptr<osg::MatrixTransform> mTrans, bool bAccurate = true);

	virtual void startClipBottomScene(osg::ref_ptr<osg::MatrixTransform> mTrans);

	virtual bool FindTransNode(std::vector<osg::MatrixTransform*> &vecTrans);

private:
	osg::ref_ptr<osg::Node> sceneNode;
	std::vector<CScope> vecScope;
};

