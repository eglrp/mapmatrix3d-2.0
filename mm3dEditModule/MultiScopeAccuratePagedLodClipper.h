#pragma once
#include "HeadPagedLodTraveller.h"
#include "StProgressSetter.h"

class CMultiScopeAccuratePagedLodClipper : public CHeadPagedLodTraveller
{
public:
	CMultiScopeAccuratePagedLodClipper(std::vector<CScope> VecScope, std::string InFilePath, std::string OutFilePath, osg::Matrix Matrix, 
		CStProgressSetter StProgressSetter, bool IsInside) : CHeadPagedLodTraveller(InFilePath, OutFilePath, StProgressSetter)
	{
		vecScope = VecScope;
		outFilePath = OutFilePath;
		inFilePath = InFilePath;
		matrix = Matrix;
		isInside = IsInside;
	}

protected:
	virtual bool ApplyChildNode(osg::ref_ptr<osg::Node> node);

	virtual CHeadPagedLodTraveller* clone(std::string inFilePath, std::string outFilePath);

private:
	std::vector<CScope> vecScope;
	std::string outFilePath;
	std::string inFilePath;
	osg::Matrix matrix;
	bool isInside;
};

