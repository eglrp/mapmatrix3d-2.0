#pragma once
#include "OSG_Plus.h"
#include <string>

class COsgPageLodSetter : public osg::NodeVisitor
{
public:
	COsgPageLodSetter(std::string FileName);
	~COsgPageLodSetter(void);

	//接口
	void SetLodFileName(osg::ref_ptr<osg::Node> node, std::string fileName);		//设置lod文件名

protected:
	virtual void apply(osg::PagedLOD& pagedLod);
private:
	std::string fileName;
};

