#pragma once
#include "HeadPagedLodTraveller.h"
#include "MultiScopeAbsoluteHeadGeodeClipper.h"
#include "InterfaceOsgFinder.h"

class CMultiScopeAbsolutePagedLodClipper : public CHeadPagedLodTraveller
{
public:
	CMultiScopeAbsolutePagedLodClipper(std::vector<CScope> VecScope, std::string InFilePath, std::string OutFilePath, osg::Matrix Matrix, 
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

	virtual	CHeadPagedLodTraveller* clone(std::string inFilePath, std::string outFilePath);

private:
	std::vector<CScope> vecScope;
	std::string outFilePath;
	std::string inFilePath;
	osg::Matrix matrix;
	bool isInside;
};

