#pragma once
#include <string>
#include <vector>

#include "coord.hpp"
#include "SevenParameter.h"
#include "OSG_Plus.h"
#include "InterfaceOsgOrient.h"

class CAbsoluteOrientationor : public InterfaceOsgOrient
{
public:
	CAbsoluteOrientationor(void);
	~CAbsoluteOrientationor(void);

	//接口
	void LoadControlPoint(std::string fileName);																								//加载控制点

	//接口
	BOOL AbsOrient(osg::ref_ptr<osg::Node> oriNode, std::string outputFilePath, osg::Matrix &absMatrix);										//绝对定向

	//接口
	void ObtainModelRelativePoint(int ptNum, double xRelaPt, double yRelaPt, double zRelaPt, bool isCheckPt);									//获取模型上相对点

	//接口
	void ModelOrientation(CSevenParameter &sevenParamCalulator, osg::ref_ptr<osg::Node> oriNode, osg::Matrix &absMatrix);						//模型定向

	//接口
	void ReverseModelOrientation(osg::ref_ptr<osg::Node> oriNode, osg::Matrix &osgMatrix);														//模型反向定向

	std::vector<CControlPoint> GetVecCtrlPt();																									//获得模型上的控制点

	std::vector<CControlPoint> GetVecRelativePt();																								//获得模型上的相对点

	void SetControlPtStatus(int ptNum, bool isCheckPt);																							//设置控制点状态

	void EraseRelativePtIndex(int index);	
																																				//清除相对点序号
	void ClearVecRelativePt();

	void ClearVecAbsolutePt();																													//清除绝对定向点号

private:
	std::vector<CControlPoint> vecCtrlPt;																										//控制点
	std::vector<CControlPoint> vecRelaPt;																										//相对点

	osg::Matrix osgMatrix;

private:
	void OutputPrecisionReport(std::string reportFileName, CSevenParameter &sevenParamCalulator, std::vector<CoordDouble3D> vecAbsoluteCoord);  //精度输出

};

