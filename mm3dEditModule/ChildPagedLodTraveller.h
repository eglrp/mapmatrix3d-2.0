#pragma once
#include <string>
#include "OSG_Plus.h"
#include <direct.h>
#include "qcomm.h"
#include "publicDefined.h"
#include "InterfaceOsgClip.h"
#include "OsgPagedLodReader.h"
#include "OsgPagedLodOutputer.h"

class CChildPagedLodTraveller : public osg::NodeVisitor
{
public:
	CChildPagedLodTraveller(std::string InFilePath, std::string OutFilePath) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), 
		osgPagedLodReader(InFilePath), osgPagedLodOutputer(OutFilePath)
	{
		outFilePath = OutFilePath;
		inFilePath = InFilePath;
	}

protected:
	virtual void apply(osg::PagedLOD& pagedLod);

	virtual bool ApplyChildNode(osg::ref_ptr<osg::Node> node);
	
	virtual void travelChildNode(osg::PagedLOD& pagedLod, osg::ref_ptr<osg::Node> node);

private:
	std::string outFilePath;
	std::string inFilePath;

protected:
	COsgPagedLodReader osgPagedLodReader;

	COsgPagedLodOutputer osgPagedLodOutputer;

};

