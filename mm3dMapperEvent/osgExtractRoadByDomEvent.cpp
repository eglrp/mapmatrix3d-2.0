#include "stdafx.h"
#include "osgExtractRoadByDomEvent.h"
#include "ThreadSafeUpdateCallback.h"
#include <fstream>
#include "InterfaceBaseDrawer.h"
#include <osgUtil/IntersectVisitor>
#include <osg/LineSegment>
#include "cvLine.h"
#include "GDAL_PLUS.h"

using namespace cv;
using namespace std;
 
const int N = 10;

void COsgExtractRoadByDomEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}
	isActivated = isActivate(vecOpState, OPS_EXTRACT_ROAD_BY_DOM);

	if (isActivated == false && flag == true)
	{
		ISigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		imageFileName = "E:\\osg数据\\Data\\dom327.tif";
		dsmFileName = "E:\\osg数据\\Data\\dsm0406.tif";
		coordFileName = "E:\\osg数据\\Data\\dsm0406.tfw";

		isFirstLine = true;

		if (!imageTool.getCoordInfo(coordFileName,leftX,leftY,resX,resY))
		{
			isActivated = false;
			ISigEventCallBack->EmitSigShowOpsHint("找不到坐标文件");
			return;
		}
		cvReleaseImage(&contourImage);
		ISigEventCallBack->EmitSigShowOpsHint("提取道路");
	}
}

bool COsgExtractRoadByDomEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
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

			getRoadEdge(image, worldPt, worldMatrix);
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
					if(num>0)
					{
						childNode = mapperGroup->getChild(num-1);
						spChildCallback->RemoveChild(mapperGroup, childNode);
					}
				}
			}
		}
	}
	return false;
}

void COsgExtractRoadByDomEvent::getRoadEdge(IplImage* image, osg::Vec3d& worldPt, osg::Matrix worldMatrix)
{
	double* dsmImage = NULL;
	int wid, hei, channel = 1;
	imageTool.ReadImageToBuff(dsmFileName, dsmImage, wid, hei);

	int xx = int((worldPt.x() - leftX) / resX);
	int yy = int((worldPt.y() - leftY) / resY);

	//鼠标点击的像素点
	int x = int((worldPt.x() - leftX) / resX);
	int y = int((worldPt.y() - leftY) / resY);
	IplImage* image0 = cvLoadImage(imageFileName, CV_LOAD_IMAGE_COLOR);
	image = imageTool.EqualizeHistColorImage(image0);
	cvReleaseImage(&image0);
	uchar* ptr = cvPtr2D(image, y, x, NULL);

	//种子颜色
	osg::Vec3d seedColor(ptr[0], ptr[1], ptr[2]);

	std::vector<osg::Vec2i> vecPoints;
	Coord coord(x,y,wid,hei);
	if (contourImage == NULL)
	{
		contourImage = cvCreateImage(cvSize(wid, hei), 8, 1);
	}
	imageTool.searchByColor(contourImage,image,dsmImage,coord,seedColor,0.1,vecPoints);

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
	cvReleaseImage(&image);
	return;
}



