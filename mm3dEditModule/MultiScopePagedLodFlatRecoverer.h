#pragma once
#include "MultiScopePagedLodFlatter.h"
#include "HeadPagedLodTraveller.h"

class CMultiScopePagedLodFlatRecoverer : public CHeadPagedLodTraveller
{
public:
	CMultiScopePagedLodFlatRecoverer(std::vector<CScope> VecScope, std::vector<CScope> VecPreVecScope, std::string InFilePath, std::string OutFilePath, osg::Matrix Matrix, 
		double FlattenHeight, double SceneMaxH, CStProgressSetter stProgress) : CHeadPagedLodTraveller(InFilePath, OutFilePath, stProgressSetter)
	{
		vecScope = VecScope;
		vecPreVecScope = vecPreVecScope;
		outFilePath = OutFilePath;
		inFilePath = InFilePath;
		matrix = Matrix;
		flattenHeight = FlattenHeight;
		sceneMaxH = SceneMaxH;
	}

protected:
	virtual bool ApplyChildNode(osg::ref_ptr<osg::Node> node);

	virtual CHeadPagedLodTraveller* clone(std::string inFilePath, std::string outFilePath);

private:
	std::vector<CScope> vecScope;
	std::vector<CScope> vecPreVecScope;
	std::string outFilePath;
	std::string inFilePath;
	osg::Matrix matrix;
	double flattenHeight;
	double sceneMaxH;

};

