#pragma once
#include <vector>
#include "publicDefined.h"
#include "IsInAreaJudger.h"

class CObtainSelTileFileName
{
public:
	CObtainSelTileFileName(std::vector<CScope> VecScope) : isInAreaJudger(VecScope)
	{
		vecScope = VecScope;
	}

	virtual void obtainSelTileName(osg::ref_ptr<osg::Group> mGroup, osg::Matrix matrix);

	virtual void obtainSelTileNamebyScope(osg::ref_ptr<osg::Group> mGroup, osg::Matrix matrix);

	virtual bool obtainFlatFileName(osg::ref_ptr<osg::MatrixTransform> mTrans);

	std::vector<std::string> getHalfInFileName() const { return vecHalfInFileName; }

	std::vector<std::string> getTotalInFileName() const { return vecTotalInFileName; }
 
	std::vector<std::string> getFlatPolygonFileName() const { return vecFlatPolygonFileName; } 

private:
	std::vector<CScope> vecScope;
	std::vector<std::string> vecHalfInFileName;
	std::vector<std::string> vecTotalInFileName;
	std::vector<std::string> vecFlatPolygonFileName;

protected:
	CIsInAreaJudger isInAreaJudger;
};

