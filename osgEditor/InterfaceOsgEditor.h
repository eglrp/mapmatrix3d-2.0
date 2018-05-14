#pragma once
#include "OSG_Plus.h"

#ifdef EDITORLIBDLL  
#define EDITORAPI _declspec(dllexport)  
#else  
#define EDITORAPI  _declspec(dllimport)  
#endif  


class EDITORAPI InterfaceOsgEditor
{
public:
	virtual void RemoveEmtyLod(osg::ref_ptr<osg::Node> node) = 0;												//移除空lod

	virtual void MoveLodCenter(osg::ref_ptr<osg::Node> node, double XMove, double YMove, double ZMove) = 0;		//移动lod中心

	virtual void CombineScene(osg::ref_ptr<osg::Node> node, std::string groupFileFolder) = 0;					//合并场景

	virtual void SplitSceneLod(osg::ref_ptr<osg::Node> node, std::string inFileName) = 0;						//场景分离

	virtual void SetLodFileName(osg::ref_ptr<osg::Node> node, std::string fileName) = 0;						//设置lod文件名

	virtual void ChangeTransMatrix(osg::ref_ptr<osg::Node> node, double XMove, double YMove, double ZMove) = 0;	//改变节点四元素矩阵

	virtual void MultiplyTransMatrix(osg::ref_ptr<osg::Node> node,osg::Matrix PosMulMatrix) = 0;				//节点乘以四元素矩阵

	virtual void ComputeVertex(osg::ref_ptr<osg::Node> node, double *Rr, double *FWK, double s) = 0;			//顶点乘以七参数

	virtual void SetImageDataVariance(osg::ref_ptr<osg::Node> node) = 0;										//设置影像数据variance

	virtual std::vector<osg::ref_ptr<osg::Geometry>> SetObjectHighLight(osg::ref_ptr<osg::Node> node, osg::Vec4d highLightColor) = 0;				//高亮物体

	virtual void CancelObjectHighLight(osg::ref_ptr<osg::Node> node) = 0;										//取消高亮物体

	virtual void ReCalPagedLodCenter(osg::ref_ptr<osg::Node> node) = 0;											//重新计算pagedLod中心

	virtual void setGeometryDepthTest(osg::ref_ptr<osg::Node> node) = 0;										//设置深度检测
};

class EDITORAPI InterfaceOsgEditorFactory
{
public:		
	InterfaceOsgEditor* create();

};