#include "stdafx.h"
#include "osgExtractFarmLandByDsmEvent.h"
#include "ThreadSafeUpdateCallback.h"
#include <fstream>
#include "InterfaceBaseDrawer.h"
#include <osgUtil/IntersectVisitor>
#include <osg/LineSegment>
#include "GDAL_PLUS.h"
#include "cvLine.h"

const double heightErr = 0.01;         //高度差
const int N = 10;                      //直线拟合的连续点数

void COsgExtractFarmLandByDsmEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}
	isActivated = isActivate(vecOpState, OPS_EXTRACT_FARMLAND_BY_DSM);

	if (isActivated == false && flag == true)
	{
		ISigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		isFirstLine = true;
		coordFileName = "E:\\osg数据\\Data\\dsm0406.tfw";
		dsmFileName = "E:\\osg数据\\Data\\dsm0406.tif";

		if (!imageTool.getCoordInfo(coordFileName,leftX,leftY,resX,resY))
		{
			isActivated = false;
			return;
		}
		cvReleaseImage(&contourImage);
		ISigEventCallBack->EmitSigShowOpsHint("提取田块");
	}
}

bool COsgExtractFarmLandByDsmEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pResult.worldPt;
			osg::Matrix worldMatrix = pResult.matrix;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			getFarmEdge(worldPt, worldMatrix);
		}
		else if (IsRightClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			//cvReleaseImage(&contourImage);
		}
		else if (IsKeyPress(ea))
		{
			osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				if (spChildCallback != NULL)
				{
					int num = mapperGroup->getNumChildren();
					osg::ref_ptr<osg::Node> childNode;
					for (int i = 0; i < num; i++)
					{
						childNode = mapperGroup->getChild(i);
						spChildCallback->RemoveChild(mapperGroup, childNode);
					}
				}
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
			{
				if (spChildCallback != NULL)
				{
					int num = mapperGroup->getNumChildren();
					osg::ref_ptr<osg::Node> childNode;
					if (num > 0)
					{
						childNode = mapperGroup->getChild(num - 1);
						spChildCallback->RemoveChild(mapperGroup, childNode);
					}
				}
			}
		}
	}
	return false;
}

void COsgExtractFarmLandByDsmEvent::getFarmEdge(osg::Vec3d& worldPt, osg::Matrix worldMatrix)
{
	double* dsmImage = NULL;
	int wid, hei;
	imageTool.ReadImageToBuff(dsmFileName, dsmImage, wid, hei);

	//鼠标点击的像素点
	int x = int((worldPt.x() - leftX) / resX);
	int y = int((worldPt.y() - leftY) / resY);
	//种子高度
	double seedH = imageTool.getHeightInDSM(dsmImage, x, y, wid);

	//按高程搜索轮廓点
	std::vector<osg::Vec2i> vecPoints;
	Coord coord(x,y,wid,hei);
	if (contourImage == NULL)
	{
		contourImage = cvCreateImage(cvSize(wid, hei), 8, 1);
	}
	imageTool.searchByHeight(contourImage, dsmImage, coord, seedH, heightErr, vecPoints);
	
	//vecPoints为像素坐标，转换为三维坐标
	//坐标转换
	std::vector<osg::Vec3d> vecPt3D;
	for (size_t i = 0; i < vecPoints.size(); ++i)
	{
		vecPt3D.push_back(osg::Vec3d(leftX + resX*(vecPoints[i].x()), leftY + resY*(vecPoints[i].y()), worldPt.z()));
	}
	
	std::vector<osg::Vec3d> vecResult;
	imageTool.pointsToContour3D(mRoot, vecPt3D, N, vecResult);

	if (!isFirstLine)
	{
		osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
		if (spChildCallback != NULL)
		{
			int num = mapperGroup->getNumChildren();
			osg::ref_ptr<osg::Node> childNode;
			if (num > 0)
			{
				childNode = mapperGroup->getChild(num - 1);
				spChildCallback->RemoveChild(mapperGroup, childNode);
			}
		}
	}
	
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	vecResult.push_back(vecResult.front());
	IDrawer->BaseDrawCurLineOperate(vecResult, osg::Vec4(1, 0, 0, 1), 2.5, mapperGroup, worldMatrix);
	isFirstLine = false;
	return;
}
