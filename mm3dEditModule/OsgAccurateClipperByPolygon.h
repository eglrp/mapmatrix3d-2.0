#pragma once
#include "osgClip.h"

class COsgAccurateClipperByPolygon : public COsgClipper
{
public:

protected:
	virtual void clipHalfInPolygonTile(osg::ref_ptr<osg::Node> node, std::vector<CScope> VecScope, std::string InFilePath, std::string OutFilePath, 
		osg::Matrix Matrix, CStProgressSetter StProgressSetter, bool IsInside);
};

