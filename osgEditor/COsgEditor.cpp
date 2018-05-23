// osgEditor.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "COsgEditor.h"
#include "OsgEmptyPagedLodRemover.h"
#include "OsgPagedLodCenterMove.h"
#include "OsgPagedLodCombiner.h"
#include "OsgPagedLodSplitter.h"
#include "OsgPageLodSetter.h"
#include "OsgTransMatrixChanger.h"
#include "OsgTransMatrixMultiplier.h"
#include "OsgVertexCompute.h"
#include "OsgStateDynamicSetter.h"
#include "OsgObjectHighlighter.h"
#include "OsgObjectCancelHighLighter.h"
#include "OsgPageLodCenterReCalculator.h"
#include "NodeDepthTestSetter.h"

void OsgEditor::RemoveEmtyLod(osg::ref_ptr<osg::Node> node)											//移除空lod
{
	COsgEmptyPagedLodRemover ive;
	node->accept(ive);

	ive.StartRemovePagedLod();

	return;
}

void OsgEditor::MoveLodCenter(osg::ref_ptr<osg::Node> node, double XMove, double YMove, double ZMove)	 //移动lod中心
{
	COsgPagedLodCenterMove ive(XMove, YMove, ZMove);
	node->accept(ive);

	return;
}

void OsgEditor::CombineScene(osg::ref_ptr<osg::Node> node, std::string groupFileFolder)				//合并场景
{
	COsgPagedLodCombiner ive(groupFileFolder);
	node->accept(ive);

	return;
}

void OsgEditor::SplitSceneLod(osg::ref_ptr<osg::Node> node, std::string inFileName)					//场景分离
{
	COsgPagedLodSplitter ive(inFileName);
	node->accept(ive);
	return;
}

void OsgEditor::SetLodFileName(osg::ref_ptr<osg::Node> node, std::string fileName)						//设置lod文件名
{
	COsgPageLodSetter ive(fileName);
	node->accept(ive);
	return;
}

void OsgEditor::ChangeTransMatrix(osg::ref_ptr<osg::Node> node, double XMove, double YMove, double ZMove)	//改变节点四元素矩阵
{
	COsgTransMatrixChanger ive(XMove, YMove, ZMove);
	node->accept(ive);
	return;
}

void OsgEditor::MultiplyTransMatrix(osg::ref_ptr<osg::Node> node,osg::Matrix PosMulMatrix)				//节点乘以四元素矩阵
{
	COsgTransMatrixMultiplier ive(PosMulMatrix);
	node->accept(ive);
	return;
}

void OsgEditor::ComputeVertex(osg::ref_ptr<osg::Node> node, double *Rr, double *FWK, double s)			//顶点乘以七参数
{
	COsgVertexCompute ive(Rr, FWK, s);
	node->accept(ive);
	return;
}

void OsgEditor::SetImageDataVariance(osg::ref_ptr<osg::Node> node)
{
	COsgStateDynamicSetter iveSetter;
	node->accept(iveSetter);
}

std::vector<osg::ref_ptr<osg::Geometry>> OsgEditor::SetObjectHighLight(osg::ref_ptr<osg::Node> node, osg::Vec4d highLightColor)
{
	COsgObjectHighlighter iveHighlighter(highLightColor);
	node->accept(iveHighlighter);
	return iveHighlighter.vecHighLightGeometry;
}

void OsgEditor::CancelObjectHighLight(osg::ref_ptr<osg::Node> node)
{
	COsgObjectCancelHighLighter iveCancelHighLighter;
	node->accept(iveCancelHighLighter);
}


void OsgEditor::ReCalPagedLodCenter(osg::ref_ptr<osg::Node> node)
{
	CPageLodCenterReCalculator iveReCalculator;
	node->accept(iveReCalculator);
}

void OsgEditor::setGeometryDepthTest(osg::ref_ptr<osg::Node> node)
{
	CNodeDepthTestSetter iveDepthTestSetter;
	node->accept(iveDepthTestSetter);
}