#pragma once
#include "OSG_Plus.h"

class COsgPagedLodSplitter : public osg::NodeVisitor
{
public:
	COsgPagedLodSplitter(std::string InFileName);
	~COsgPagedLodSplitter(void);

	//≥°æ∞∑÷¿Î
	void SplitSceneLod(osg::ref_ptr<osg::Node> node, std::string inFileName);

protected:
	virtual void apply(osg::MatrixTransform& node);

	std::string ObtainBlockName(std::string fileName);
	std::string ObtainTileFileName(std::string fileName);

protected:
	std::string inFileName;

};

