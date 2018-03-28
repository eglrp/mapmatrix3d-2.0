#include "stdafx.h"
#include "OsgClipperByPolygon.h"
#include "MultiScopePagedLodClipper.h"

void COsgClipperByPolygon::clipHalfInPolygonTile(osg::ref_ptr<osg::Node> node, std::vector<CScope> VecScope, std::string InFilePath, std::string OutFilePath, 
	osg::Matrix Matrix, CStProgressSetter StProgressSetter, bool IsInside)
{
	//对于不完全在范围内进行裁切
	CMultiScopePagedLodClipper iveClipper(VecScope, InFilePath, OutFilePath, Matrix, StProgressSetter, IsInside);
	node->accept(iveClipper);

}

