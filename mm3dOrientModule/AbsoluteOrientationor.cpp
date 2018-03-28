#include "StdAfx.h"
#include "AbsoluteOrientationor.h"
#include "projection_matrix_transform.hpp"
#include "InterfaceOsgEditor.h"
#include "InterfaceOsgAlgorithm.h"
#include "InterfaceMFCExcute.h"
#include "qcomm.h"


CAbsoluteOrientationor::CAbsoluteOrientationor(void)
{
}


CAbsoluteOrientationor::~CAbsoluteOrientationor(void)
{
}

void CAbsoluteOrientationor::SetControlPtStatus(int ptNum, bool isCheckPt)
{
	for (int i = 0; i < vecRelaPt.size(); i++)
	{
		if (vecRelaPt[i].ptSerialNum == ptNum)
		{
			vecRelaPt[i].isCheckPoint = isCheckPt;
		}
	}
}

void CAbsoluteOrientationor::LoadControlPoint(std::string fileName)
{	
	FILE* fpCtrlPt = fopen(fileName.c_str(), "r");

	if (fpCtrlPt == NULL)
	{
		InterfaceMFCExcuteFactory IExcuteFactory;
		InterfaceMFCExcute* IExcute = IExcuteFactory.create();
		IExcute->PopUpMessage("选择路径不正确");
		return;
	}

	int cnt = 0;
	fscanf(fpCtrlPt, "%d\n", &cnt);

	for (int i = 0; i < cnt; i++)
	{
		int ptNum = 0;
		double ptX = 0;
		double ptY = 0;
		double ptZ = 0;
		char str[30];
		fscanf(fpCtrlPt, "%s %lf %lf %lf\n", &str, &ptX, &ptY, &ptZ);
		
		for (int i = 0;i<30;i++)
		{
			if (isalpha(str[i]))
			{
				InterfaceMFCExcuteFactory IExcuteFactory;
				InterfaceMFCExcute* IExcute = IExcuteFactory.create();
				IExcute->PopUpMessage("控制点编号仅支持数字，请修改");
				vecCtrlPt.clear();
				return;
			}
		}
		ptNum = atoi(str);
		vecCtrlPt.push_back(CControlPoint(ptNum, ptX, ptY, ptZ, FALSE));
	}

}

BOOL CAbsoluteOrientationor::AbsOrient(osg::ref_ptr<osg::Node> oriNode, std::string outputFilePath, osg::Matrix &absMatrix)
{
	// TODO: 在此添加控件通知处理程序代码
	std::vector<CoordDouble3D> vecAbsoluteCoord;
	std::vector<CoordDouble3D> vecRelaCoord;
	std::vector<CoordDouble3D> vecAllAbsCoord;

	if (vecCtrlPt.size() == 0)
	{
		return FALSE; 
		//AfxMessageBox(_T("没有加载控制点"));
	}

	for (int i = 0; i < vecRelaPt.size(); i++)
	{
		for (int j = 0; j < vecCtrlPt.size(); j++)
		{
			if (vecCtrlPt[j].ptSerialNum == vecRelaPt[i].ptSerialNum)
			{
				if (vecRelaPt[i].isCheckPoint == FALSE)
				{
					vecRelaCoord.push_back(CoordDouble3D(vecRelaPt[i].ptX, vecRelaPt[i].ptY, vecRelaPt[i].ptZ));
					vecAbsoluteCoord.push_back(CoordDouble3D(vecCtrlPt[j].ptX, vecCtrlPt[j].ptY, vecCtrlPt[j].ptZ));
				}
					
				vecAllAbsCoord.push_back(CoordDouble3D(vecCtrlPt[j].ptX, vecCtrlPt[j].ptY, vecCtrlPt[j].ptZ));
			}
		}

	}

	if (vecRelaCoord.size() < 3)
	{
		InterfaceMFCExcuteFactory IExcuteFactory;
		InterfaceMFCExcute* IExcute = IExcuteFactory.create();
		IExcute->PopUpMessage("控制点数量小于3，无法进行绝对定向计算");
		return FALSE;
	}

	CSevenParameter sevenParamCalulator;
	sevenParamCalulator.LoadControlPoints(vecAbsoluteCoord, vecRelaCoord);
	sevenParamCalulator.CalculateSevenParameter();

	InterfaceMFCExcuteFactory IExcuteFactory;
	InterfaceMFCExcute* IExcute = IExcuteFactory.create();

	if (sevenParamCalulator.s_ < 0.5)
	{
		IExcute->PopUpMessage("模型尺度小于0.5，定向后模型可能过小而无法显示！");
		return FALSE;
	}

	//绝对定向
	ModelOrientation(sevenParamCalulator, oriNode, absMatrix);

	std::string outFileReportFileName = outputFilePath + "report.txt";
	//输出精度报告
	OutputPrecisionReport(outFileReportFileName, sevenParamCalulator, vecAllAbsCoord);

	IExcute->PopUpMessage("绝对定向完成！精度报告输出在场景索引位置");
	return TRUE;
}

void CAbsoluteOrientationor::ModelOrientation(CSevenParameter &sevenParamCalulator, osg::ref_ptr<osg::Node> oriNode, osg::Matrix &absMatrix)
{
	// TODO: 在此添加控件通知处理程序代码
	
	double matDB[16] = {0};
	int cnt = 0;
	//设置变换矩阵，R是转置输入，T除以scale，scale：1/scale
	matDB[0] = sevenParamCalulator.R_[0]; matDB[1] = sevenParamCalulator.R_[3]; matDB[2] =	sevenParamCalulator.R_[6];
	matDB[4] = sevenParamCalulator.R_[1]; matDB[5] = sevenParamCalulator.R_[4]; matDB[6] =	sevenParamCalulator.R_[7];
	matDB[8] = sevenParamCalulator.R_[2]; matDB[9] = sevenParamCalulator.R_[5]; matDB[10] = sevenParamCalulator.R_[8];
	matDB[12] = sevenParamCalulator.t_[0] / sevenParamCalulator.s_; matDB[13] = sevenParamCalulator.t_[1] / sevenParamCalulator.s_; matDB[14] = sevenParamCalulator.t_[2] / sevenParamCalulator.s_; matDB[15] = 1 / sevenParamCalulator.s_;

	absMatrix.set(matDB);
	double* newDB = (double*) absMatrix.ptr();

	for (int i = 0; i < 16; i++)
	{
		newDB[i] *= sevenParamCalulator.s_;
	}

	InterfaceOsgEditorFactory  IEditorFactory;
	InterfaceOsgEditor* IEditor = IEditorFactory.create();
	IEditor->MultiplyTransMatrix(oriNode, absMatrix);
}

void CAbsoluteOrientationor::ReverseModelOrientation(osg::ref_ptr<osg::Node> oriNode, osg::Matrix &osgMatrix)
{
	// TODO: 在此添加控件通知处理程序代码

	osg::Matrix irOsgMatrix;
	irOsgMatrix = irOsgMatrix.inverse(osgMatrix);

	InterfaceOsgEditorFactory  IEditorFactory;
	InterfaceOsgEditor* IEditor = IEditorFactory.create();
	IEditor->MultiplyTransMatrix(oriNode, irOsgMatrix);

	osg::Matrix matrix;
	osgMatrix = matrix;
}

void CAbsoluteOrientationor::OutputPrecisionReport(std::string reportFileName, CSevenParameter& sevenParamCalulator, 
	std::vector<CoordDouble3D> vecAllAbsoluteCoord)
{
	FILE* fpReport = fopen(reportFileName.c_str(), "w");

	if (fpReport == NULL)
	{
		InterfaceMFCExcuteFactory IExcuteFactory;
		InterfaceMFCExcute* IExcute = IExcuteFactory.create();
		IExcute->PopUpMessage("报告选择路径有误！");
		return;
	}

	InterfaceOsgAlgorithmFactory osgAlgorithm;
	InterfaceOsgAlgorithm* IOsgAlgorithm = osgAlgorithm.create();

	//获取旋转角
	double fwk[3] ={0};

	IOsgAlgorithm->RtoAngle(sevenParamCalulator.R_, fwk[0], fwk[1], fwk[2]);
	fprintf(fpReport, "\t\t\t精度报告\n");
	fprintf(fpReport, "俯仰角：%lf 横滚角：%lf 航向角:%lf\n", fwk[0], fwk[1], fwk[2]);
	//获取位移
	fprintf(fpReport, "位移X: %lf 位移Y: %lf 位移Z: %lf\n", sevenParamCalulator.t_[0], sevenParamCalulator.t_[1], sevenParamCalulator.t_[2]);
	fprintf(fpReport, "尺度：%lf\n", sevenParamCalulator.s_);
	fprintf(fpReport, "点号\t是否为检查点\t计算绝对X\t计算绝对Y\t计算绝对Z\t绝对X\t绝对Y\t绝对Z\t误差X\t误差Y\t误差Z\n");

	for (int i = 0; i < vecRelaPt.size(); i++)
	{
		char strCheckPt[20]; 

		if (vecRelaPt[i].isCheckPoint == true)
		{
			sprintf_s(strCheckPt, "是");
		}
		else
		{
			sprintf_s(strCheckPt, "否");
		}

		CoordDouble3D absXYZ;
		sevenParamCalulator.CalculateRelativeToAbsolute(CoordDouble3D(vecRelaPt[i].ptX, vecRelaPt[i].ptY, vecRelaPt[i].ptZ), absXYZ);
		fprintf(fpReport, "%d\t%s\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n", vecRelaPt[i].ptSerialNum, strCheckPt, absXYZ.x, absXYZ.y, absXYZ.z, 
			vecAllAbsoluteCoord[i].x, vecAllAbsoluteCoord[i].y, vecAllAbsoluteCoord[i].z, vecAllAbsoluteCoord[i].x - absXYZ.x, vecAllAbsoluteCoord[i].y - absXYZ.y, vecAllAbsoluteCoord[i].z - absXYZ.z);
	}

	fclose(fpReport);
}

void CAbsoluteOrientationor::ObtainModelRelativePoint(int ptNum, double xRelaPt, double yRelaPt, double zRelaPt, bool isCheckPt)
{
	vecRelaPt.push_back(CControlPoint(ptNum, xRelaPt, yRelaPt, zRelaPt, isCheckPt));
}

std::vector<CControlPoint> CAbsoluteOrientationor::GetVecCtrlPt()
{
	return vecCtrlPt;
}

std::vector<CControlPoint> CAbsoluteOrientationor::GetVecRelativePt()
{
	return vecRelaPt;
}

void CAbsoluteOrientationor::ClearVecRelativePt()
{
	vecRelaPt.clear();
}

void CAbsoluteOrientationor::ClearVecAbsolutePt()
{
	vecCtrlPt.clear();
}

void CAbsoluteOrientationor::EraseRelativePtIndex(int index)
{
	std::vector<CControlPoint>::iterator it = vecRelaPt.begin() + index;
	vecRelaPt.erase(it);
}