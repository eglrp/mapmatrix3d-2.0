#pragma once
#include "HeadPagedLodTraveller.h"
#include "InterfaceOsgClip.h"

class CMultiScopePagedLodClipper : public CHeadPagedLodTraveller
{
public:
	CMultiScopePagedLodClipper(std::vector<CScope> vecScope, std::string inFilePath, std::string outFilePath, osg::Matrix matrix, 
		CStProgressSetter stProgressSetter, bool isInside) : CHeadPagedLodTraveller(inFilePath, outFilePath, stProgressSetter), stProgressSetter(stProgressSetter)
	{
		this->vecScope = vecScope;
		this->outFilePath = outFilePath;
		this->inFilePath = inFilePath;
		this->matrix = matrix;
		this->isInside = isInside;
	}

protected:
	virtual bool ApplyChildNode(osg::ref_ptr<osg::Node> node);

	virtual CHeadPagedLodTraveller* clone(std::string inFilePath, std::string outFilePath);

private:
	std::vector<CScope> vecScope;
	std::string outFilePath;
	std::string inFilePath;
	osg::Matrix matrix;
	CStProgressSetter stProgressSetter;
	bool isInside;
};

