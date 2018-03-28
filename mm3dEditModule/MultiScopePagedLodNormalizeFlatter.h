#pragma once
#include "HeadPagedLodTraveller.h"

class CMultiScopePagedLodNormalizeFlatter : public CHeadPagedLodTraveller
{
public:
	CMultiScopePagedLodNormalizeFlatter(std::vector<CScope> vecScope, std::string inFilePath, std::string outFilePath, osg::Matrix matrix, double flattenHeight, 
		double sceneMaxH, CStProgressSetter stProgress)
		: CHeadPagedLodTraveller(inFilePath, outFilePath, stProgress), stProgress(stProgress)
	{
		this->vecScope = vecScope;
		this->outFilePath = outFilePath;
		this->inFilePath = inFilePath;
		this->matrix = matrix;
		this->flattenHeight = flattenHeight;
		this->sceneMaxH = sceneMaxH;
	}

protected:
	virtual bool ApplyChildNode(osg::ref_ptr<osg::Node> node);

	virtual CHeadPagedLodTraveller* clone(std::string inFilePath, std::string outFilePath);

private:
	std::vector<CScope> vecScope;
	std::string outFilePath;
	std::string inFilePath;
	osg::Matrix matrix;
	double flattenHeight;
	double sceneMaxH;
	CStProgressSetter stProgress;
};

