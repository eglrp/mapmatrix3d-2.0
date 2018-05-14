#pragma once
#include "osgViewer/Viewer"

class osgOffScreenViewer : public osgViewer::Viewer
{
public:
	//不需要显示出来的viewer
	void realize();

};