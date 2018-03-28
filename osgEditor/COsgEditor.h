#pragma once
#include "OSG_Plus.h"
#include "InterfaceOsgEditor.h"

class OsgEditor : public InterfaceOsgEditor
{
public:
	virtual void RemoveEmtyLod(osg::ref_ptr<osg::Node> node);	//移除空lod

	virtual void MoveLodCenter(osg::ref_ptr<osg::Node> node, double XMove, double YMove, double ZMove);		//移动lod中心

	virtual void CombineScene(osg::ref_ptr<osg::Node> node, std::string groupFileFolder);					//合并场景

	virtual void SplitSceneLod(osg::ref_ptr<osg::Node> node, std::string inFileName);						//场景分离

	virtual void SetLodFileName(osg::ref_ptr<osg::Node> node, std::string fileName);						//设置lod文件名

	virtual void ChangeTransMatrix(osg::ref_ptr<osg::Node> node, double XMove, double YMove, double ZMove);	//改变节点四元素矩阵

	virtual void MultiplyTransMatrix(osg::ref_ptr<osg::Node> node,osg::Matrix PosMulMatrix);				//节点乘以四元素矩阵

	virtual void ComputeVertex(osg::ref_ptr<osg::Node> node, double *Rr, double *FWK, double s);			//顶点乘以七参数

	virtual void SetImageDataVariance(osg::ref_ptr<osg::Node> node);										//设置影像variance

	virtual std::vector<osg::ref_ptr<osg::Geometry>> SetObjectHighLight(osg::ref_ptr<osg::Node> node, osg::Vec4d highLightColor);				//高亮物体

	virtual void CancelObjectHighLight(osg::ref_ptr<osg::Node> node);										//取消高亮物体
	
	virtual void ReCalPagedLodCenter(osg::ref_ptr<osg::Node> node);											//重新计算pagedLod中心

	virtual void setGeometryDepthTest(osg::ref_ptr<osg::Node> node);
};
