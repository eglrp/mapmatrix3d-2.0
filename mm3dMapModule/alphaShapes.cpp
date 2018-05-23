// alphaShapes.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <math.h>
#include "alphaShapes.h"
#include "ThreadSafeUpdateCallback.h"

bool CAlphaShapes::doCheck(osg::Vec3d& p1, osg::Vec3d& p2, double alpha, std::vector<osg::Vec3d>& vecPoints)//Alpha Shapes检测
{
	if (distance_2Point_2D(p1.x(),p1.y(),p2.x(),p2.y()) > 2*alpha)
	{
		return false;
	}
	double ox, oy;//圆心坐标
	double x1 = p1.x();
	double y1 = p1.y();
	//两点坐标
	double x2 = p2.x();
	double y2 = p2.y();

	double s_2 = (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);

	double H1 = sqrt(alpha*alpha/s_2 - 0.25);
	double H2 = -H1;

	//判断圆1
	ox = x1 + (x2 - x1) / 2 + H1*(y2 - y1);
	oy = y1 + (y2 - y1) / 2 + H1*(x1 - x2);

	bool failed = false;

	for (std::vector<osg::Vec3d>::iterator ite = vecPoints.begin(); ite != vecPoints.end();++ite)
	{
		if (*ite == p1 || *ite == p2)
		{
			continue;
		}
		if (distance_2Point_2D(ox,oy,(*ite).x(),(*ite).y()) < alpha)
		{
			failed = true;
			break;
		}
	}
	if (failed == false)
	{
		return true;
	}
	
	//判断圆2
	ox = x1 + (x2 - x1) / 2 + H2*(y2 - y1);
	oy = y1 + (y2 - y1) / 2 + H2*(x1 - x2);
	for (std::vector<osg::Vec3d>::iterator ite = vecPoints.begin(); ite != vecPoints.end(); ++ite)
	{
		if (*ite == p1 || *ite == p2)
		{
			continue;
		}
		if (distance_2Point_2D(ox, oy, (*ite).x(), (*ite).y()) < alpha)
		{
			return false;
		}
	}
	return true;
}

double CAlphaShapes::distance_2Point_2D(double x1, double y1, double x2, double y2)
{
	return sqrt((x1-x2)*(x1 - x2) + (y1-y2)*(y1-y2));
}

void CAlphaShapes::dataOrganization(std::vector<osg::Vec3d> vecPointsIn, std::vector<osg::Vec3d>& vecPointsOut, double a)
{
	//统计点云数据的最大最小坐标
	if (vecPointsIn.empty())
	{
		return;
	}
	vecPointsOut.clear();
	double minX = vecPointsIn[0].x();
	double minY = vecPointsIn[0].y();
	double maxX = vecPointsIn[0].x();
	double maxY = vecPointsIn[0].y();

	for (size_t i = 1;i<vecPointsIn.size();++i)
	{
		if (vecPointsIn[i].x()<minX)
		{
			minX = vecPointsIn[i].x();
		}
		else if (vecPointsIn[i].x()>maxX)
		{
			maxX = vecPointsIn[i].x();
		}
		if (vecPointsIn[i].y() < minY)
		{
			minY = vecPointsIn[i].y();
		}
		else if (vecPointsIn[i].y() > maxY)
		{
			maxY = vecPointsIn[i].y();
		}
	}
	//初始化格网
	size_t Rows = int((maxY - minY) / a) + 1;//格网行数
	size_t Cols = int((maxX - minX) / a) + 1;//格网列数
	int* grid = new int[Rows * Cols];
	for (int i = 0;i<Rows * Cols;++i)
	{
		grid[i] = 0;
	}
	//填充格网数据
	MY_MAP my_map;
	my_map.clear();

	for (size_t i = 0;i<vecPointsIn.size();++i)
	{
		int row = int((vecPointsIn[i].y() - minY) / a);
		int col = int((vecPointsIn[i].x() - minX) / a);
		my_map[Cols * row + col].push_back(vecPointsIn[i]);
		grid[Cols * row + col]++;
	}

	for (size_t i = 0;i<Rows; ++i)
	{
		for (size_t j = 0;j<Cols;++j)
		{
			if (grid[i*Cols + j] != 0)
			{
				//i行j列中有数据
				if (i-1<0 || i+1>=Rows || j-1<0 || j+1>=Cols)//边缘网格
				{
					for (int k = 0;k<my_map[i*Cols + j].size();++k)
					{
						vecPointsOut.push_back((my_map[i*Cols + j])[k]);
					}
				}
				else//非边缘网格
				{
					if (grid[(i - 1)*Cols + j - 1] == 0 || grid[(i - 1)*Cols + j] == 0 || grid[(i - 1)*Cols + j + 1] == 0 ||
						grid[i*Cols + j - 1] == 0 || grid[i*Cols + j + 1] == 0 ||
						grid[(i + 1)*Cols + j - 1] == 0 || grid[(i + 1)*Cols + j] == 0 || grid[(i + 1)*Cols + j + 1] == 0)
					{
						for (int k = 0; k < my_map[i*Cols + j].size(); ++k)
						{
							vecPointsOut.push_back((my_map[i*Cols + j])[k]);
						}
					}
				}
			}
		}
		
	}
	delete grid;
}

void CAlphaShapes::extractPtCloudEdge(std::vector<osg::Vec3d>&vecWorldCoord, std::vector<osg::Vec3d>&vecEdge, double alpha)
{
	vecEdge.clear();
	osg::ref_ptr<osg::Group> group = new osg::Group;
	CAlphaShapes alphaShapes;
	std::vector<osg::Vec3d> vecWorldCoord1;
	size_t m_sum = vecWorldCoord.size();
	for (size_t i = 0; i < m_sum; i++)//删除重复的点
	{
		for (size_t j = i + 1; j < m_sum; j++)
		{
			if (distance2(vecWorldCoord[i], vecWorldCoord[j]) < EPS)
			{
				vecWorldCoord[j].x() = vecWorldCoord[m_sum - 1].x();
				vecWorldCoord[j].y() = vecWorldCoord[m_sum - 1].y();
				vecWorldCoord[j].z() = vecWorldCoord[m_sum - 1].z();

				m_sum--;
				j--;
			}
		}
	}
	vecWorldCoord.erase(vecWorldCoord.begin() + m_sum, vecWorldCoord.end());
	//alphaShapes.dataOrganization(vecWorldCoord0, vecWorldCoord, 1);


	/*for (size_t i = 0; i < vecWorldCoord0.size(); ++i)
	{
	IDrawer->BaseDrawPtOperate(vecWorldCoord[i], osg::Vec4d(1, 0, 1, 1), 3.5, group, worldMatrix);

	}
	osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
	if (spChildCallback != NULL)
	{
	spChildCallback->AddChild(mapperGroup, group);
	}
	return;*/

	for (size_t i = 0; i < vecWorldCoord.size(); ++i)
	{
		for (size_t j = i + 1; j < vecWorldCoord.size(); ++j)
		{
			if (alphaShapes.doCheck(vecWorldCoord[i], vecWorldCoord[j], alpha, vecWorldCoord))
			{
				vecEdge.push_back(vecWorldCoord[i]);
				vecEdge.push_back(vecWorldCoord[j]);
			}
		}
	}
}