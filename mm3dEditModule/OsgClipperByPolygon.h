#pragma once
#include "osgClip.h"
#include "qcomm.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceMFCExcute.h"
#include "Geometry.h"
#include "StProgressSetter.h"

class COsgClipperByPolygon : public COsgClipper
{
public:
	//½Ó¿Ú

protected:
	virtual void clipHalfInPolygonTile(osg::ref_ptr<osg::Node> node, std::vector<CScope> VecScope, std::string InFilePath, std::string OutFilePath,
		osg::Matrix Matrix, CStProgressSetter StProgressSetter, bool IsInside);


};

