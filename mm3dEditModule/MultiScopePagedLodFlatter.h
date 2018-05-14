#pragma once
#include "MultiScopePagedLodClipper.h"

class CMultiScopePagedLodFlatter : public CHeadPagedLodTraveller
{
public:
	CMultiScopePagedLodFlatter(std::vector<CScope> vecScope, std::string inFilePath, std::string outFilePath, osg::Matrix matrix, double flattenHeight,
		CStProgressSetter stProgress) : CHeadPagedLodTraveller(inFilePath, outFilePath, stProgress), stProgress(stProgress)
	{
		this->vecScope = vecScope;
		this->outFilePath = outFilePath;
		this->inFilePath = inFilePath;
		this->matrix = matrix;
		this->flattenHeight = flattenHeight;
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
	CStProgressSetter stProgress;
};

