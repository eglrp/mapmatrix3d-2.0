#pragma once
#include "SevenParameter.h"
#include "OSG_Plus.h"
#include "ControlPoint.h"

#ifdef OSGORIENTLIBDLL  
#define OSGORIENTAPI _declspec(dllexport)  
#else  
#define OSGORIENTAPI  _declspec(dllimport)  
#endif  


class OSGORIENTAPI InterfaceOsgOrient
{
public:
	//接口
	virtual void LoadControlPoint(std::string fileName) = 0;																								//加载控制点

	//接口
	virtual BOOL AbsOrient(osg::ref_ptr<osg::Node> oriNode, std::string outputFilePath, osg::Matrix &absMatrix) = 0;										//绝对定向

	//接口
	virtual void ObtainModelRelativePoint(int ptNum, double xRelaPt, double yRelaPt, double zRelaPt, bool isCheckPt) = 0;									//获取模型上相对点

	//接口
	virtual void ModelOrientation(CSevenParameter &sevenParamCalulator, osg::ref_ptr<osg::Node> oriNode, osg::Matrix &absMatrix) = 0;						//模型定向

	//接口
	virtual void ReverseModelOrientation(osg::ref_ptr<osg::Node> oriNode, osg::Matrix &osgMatrix) = 0;														//模型反向定向

	virtual std::vector<CControlPoint> GetVecCtrlPt() = 0;																									//获得模型上的控制点

	virtual std::vector<CControlPoint> GetVecRelativePt() = 0;																								//获得模型上的相对点

	virtual void SetControlPtStatus(int ptNum, bool isCheckPt) = 0;																							//设置控制点状态

	virtual void EraseRelativePtIndex(int index) = 0;																										//清除相对点序号	

	virtual void ClearVecRelativePt() = 0;																													//清除相对点组

	virtual void ClearVecAbsolutePt() = 0;																													//清除绝对点组
};

class OSGORIENTAPI InterfaceOsgOrientFactory
{
public:
	InterfaceOsgOrient* create();
};