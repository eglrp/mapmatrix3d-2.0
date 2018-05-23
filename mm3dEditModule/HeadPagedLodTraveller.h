#pragma once
#include <string>
#include "OSG_Plus.h"
#include <direct.h>
#include "qcomm.h"
#include "publicDefined.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceOsgClip.h"
#include "OsgPagedLodReader.h"
#include "OsgPagedLodOutputer.h"
#include "StProgressSetter.h"

class CHeadPagedLodTraveller : public osg::NodeVisitor
{
public:
	CHeadPagedLodTraveller(std::string inFilePath, std::string outFilePath, CStProgressSetter stProgressSetter)
		: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), osgPagedLodReader(inFilePath), osgPagedLodOutputer(outFilePath), 
		stProgressSetter(stProgressSetter)
	{
		this->outFilePath = outFilePath;
		this->inFilePath = inFilePath;	
	}

	CHeadPagedLodTraveller(std::string inFilePath, std::string outFilePath) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
		osgPagedLodReader(inFilePath), osgPagedLodOutputer(outFilePath)
	{
		this->outFilePath = outFilePath;
		this->inFilePath = inFilePath;
	}

	void setOutFilePath(std::string outFilePath)
	{
		this->outFilePath = outFilePath;
	}

	void setInFilePath(std::string inFilePath)
	{
		this->inFilePath = inFilePath;
	}

protected:
	virtual void apply(osg::PagedLOD& pagedLod);

	virtual bool ApplyChildNode(osg::ref_ptr<osg::Node> node);												//重写添加新操作

	virtual void travelChildNode(osg::PagedLOD& pagedLod, osg::ref_ptr<osg::Node> node);

	virtual CHeadPagedLodTraveller* clone(std::string inFilePath, std::string outFilePath);					//继承类必须进行重写

private:
	std::string outFilePath;
	std::string inFilePath;

protected:
	COsgPagedLodReader osgPagedLodReader;

	COsgPagedLodOutputer osgPagedLodOutputer;

	CStProgressSetter stProgressSetter;

};

