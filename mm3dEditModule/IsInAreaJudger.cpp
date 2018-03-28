#include "stdafx.h"
#include "IsInAreaJudger.h"

bool CIsInAreaJudger::isPtInArea(osg::Vec3d pt)
{
	bool bIsInPolygon = false;

	for (int i = 0; i < vecScope.size(); i++)
	{
		std::vector<CL::Vec2d> pol = obtainArea(i);

		if (Point_In_Polygon_2D(pt.x(), pt.y(), pol))
		{
			bIsInPolygon = true;
		}
	}

	return bIsInPolygon;
}


bool CIsInAreaJudger::isPtInScope(osg::Vec3d pt)
{
	bool bIsInPolygon = false;

	for (int i = 0; i < vecScope.size(); i++)
	{
		std::vector<CL::Vec2d> pol = obtainArea(i);

		double minH = vecScope[i].minH;
		double maxH = vecScope[i].maxH;

		if (Point_In_Polygon_2D(pt.x(), pt.y(), pol))
		{
			if ((pt.z() <= maxH && pt.z() >= minH))
			{
				bIsInPolygon = true;
			}
			
		}
	}

	return bIsInPolygon;
}

void CIsInAreaJudger::setScope(std::vector<CScope> VecScope) 
{
	vecScope = VecScope;
}

bool CIsInAreaJudger::isTriangleInScope(CVertsTriangle vertsTri)
{
	//ÅÐ¶ÏÊÇ·ñÔÚ·¶Î§ÄÚ
	bool isInPolygon = false;

	for (int i = 0; i < vecScope.size(); i++)
	{
		std::vector<CL::Vec2d> pol = obtainArea(i);

		double minH = vecScope[i].minH;
		double maxH = vecScope[i].maxH;

		if (isTriangleInPolygon(vertsTri, pol, minH, maxH))
		{
			isInPolygon = true;
		}
	}

	return isInPolygon;
}

bool CIsInAreaJudger::isTriangleInAbsoluteScope(CVertsTriangle vertsTri)
{
	//ÅÐ¶ÏÊÇ·ñÔÚ·¶Î§ÄÚ
	bool isInPolygon = false;

	for (int i = 0; i < vecScope.size(); i++)
	{
		std::vector<CL::Vec2d> pol = obtainArea(i);

		double minH = vecScope[i].minH;
		double maxH = vecScope[i].maxH;

		if (isTriangleInPolygon(vertsTri, pol, minH, maxH))
		{
			isInPolygon = true;
		}
	}

	return isInPolygon;
}



bool CIsInAreaJudger::isTriangleInPolygon(CVertsTriangle vertsTri, std::vector<CL::Vec2d> pol, double minH, double maxH)
{
	bool isInPolygon = false;

	if (Point_In_Polygon_2D(vertsTri.pt1.x(), vertsTri.pt1.y(), pol) ||
		Point_In_Polygon_2D(vertsTri.pt2.x(), vertsTri.pt2.y(), pol) ||
		Point_In_Polygon_2D(vertsTri.pt3.x(), vertsTri.pt3.y(), pol))
	{
		if (minH >= maxH)
		{
			isInPolygon = true;
		}
		else
		{
			if ((vertsTri.pt1.z() < maxH && vertsTri.pt1.z() > minH) || (vertsTri.pt2.z() < maxH && vertsTri.pt2.z() > minH) || (vertsTri.pt3.z() < maxH && vertsTri.pt3.z() > minH))
			{
				isInPolygon = true;
			}
		}
	}

	return isInPolygon;
}

bool CIsInAreaJudger::isTriangleInAbsolutePolygon(CVertsTriangle vertsTri, std::vector<CL::Vec2d> pol, double minH, double maxH)
{
	bool isInPolygon = false;

	if (Point_In_Polygon_2D(vertsTri.pt1.x(), vertsTri.pt1.y(), pol) ||
		Point_In_Polygon_2D(vertsTri.pt2.x(), vertsTri.pt2.y(), pol) ||
		Point_In_Polygon_2D(vertsTri.pt3.x(), vertsTri.pt3.y(), pol))
	{
		if (minH >= maxH)
		{
			isInPolygon = true;
		}
		else
		{
			if ((vertsTri.pt1.z() < maxH && vertsTri.pt1.z() > minH) || (vertsTri.pt2.z() < maxH && vertsTri.pt2.z() > minH) || (vertsTri.pt3.z() < maxH && vertsTri.pt3.z() > minH))
			{
				isInPolygon = true;
			}
		}
	}

	return isInPolygon;
}

std::vector<CL::Vec2d> CIsInAreaJudger::obtainArea(int i) 
{
	std::vector<CL::Vec2d> pol;

	for (int t = 0; t < vecScope[i].vecCoord.size(); t++)
	{
		double x = vecScope[i].vecCoord[t].x();
		double y = vecScope[i].vecCoord[t].y();
		double z = vecScope[i].vecCoord[t].z();
		pol.push_back(CL::Vec2d(x, y));
	}

	double x = vecScope[i].vecCoord[0].x();
	double y = vecScope[i].vecCoord[0].y();
	double z = vecScope[i].vecCoord[0].z();
	pol.push_back(CL::Vec2d(x, y));

	return pol;
}