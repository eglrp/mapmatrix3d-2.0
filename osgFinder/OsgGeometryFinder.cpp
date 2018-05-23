#include "StdAfx.h"
#include "OsgGeometryFinder.h"
#include "InterfaceMFCExcute.h"

void COsgGeodeFinder::apply(osg::Geode& geode)
{
	isFindGeometry = true;                                            /////修改成保存
	traverse(geode);

	InterfaceMFCExcuteFactory IMfcExcuteFactory;
	InterfaceMFCExcute* IMFCExecute = IMfcExcuteFactory.create();
	IMFCExecute->MFCPeekMessage();			//刷新进度条
}