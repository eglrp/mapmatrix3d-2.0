#include "stdafx.h"
#include "IsInScope.h"
#include "Geometry.h"


bool CIsInScope::PtInTriangle(osg::Vec3d pt, osg::Vec3d& t1, osg::Vec3d& t2, osg::Vec3d& t3)
{
	osg::Vec3d vec1 = t1 - pt;
	osg::Vec3d vec2 = t2 - pt;
	osg::Vec3d vec3 = t3 - pt;

	double ang1 = CL::getAngleForVector(vec1, vec2);
	double ang2 = CL::getAngleForVector(vec1, vec3);
	double ang3 = CL::getAngleForVector(vec3, vec2);

	if (ang1 + ang2 + ang3 <= 360 + 5 && ang1 + ang2 + ang3 >= 360 - 5)
	{
		return true;
	}
	return false;
}


bool CIsInScope::PtInVector(osg::Vec3d& pt1, osg::Vec3d& pt2, osg::Vec3d& pt3, std::vector<osg::Vec3d> &vecPoints)
{
	bool t1 = false, t2 = false, t3 = false;
	for (size_t i = 0; i < vecPoints.size(); ++i)
	{
		if (pt1 == vecPoints[i])
		{
			t1 = true;
		}
		if (pt2 == vecPoints[i])
		{
			t2 = true;
		}
		if (pt3 == vecPoints[i])
		{
			t3 = true;
		}
		if (t1&&t2&&t3)
		{
			return true;
		}
	}
	return false;
}

bool CIsInScope::higherThan(CTriangle& triangle, double h)
{
	if (triangle.pt1.z() >= h && triangle.pt2.z() >= h && triangle.pt3.z() >= h)
	{
		return true;
	}

	return false;
}

//ÅÐ¶Ï½Ç¶ÈÆ«²î
bool CIsInScope::checkNomal(osg::Vec3d nom, osg::Vec3d normal, double err)
{
	double mo = sqrt(nom.x()*nom.x() + nom.y()*nom.y() + nom.z()*nom.z());
	double m_normal = sqrt(normal.x()*normal.x() + normal.y()*normal.y() + normal.z()*normal.z());
	double ji = nom.x() * normal.x() + nom.y() * normal.y() + nom.z() * normal.z();
	double angle = acos(ji / (mo * m_normal));
	angle = angle * 180 / osg::PI;

	if (angle < err || angle>180 - err)
	{
		return true;
	}
	return false;
}

bool CIsInScope::checkNormalForMono(osg::Vec3d nom, osg::Vec3d normal, double err)
{
	double mo = sqrt(nom.x()*nom.x() + nom.y()*nom.y() + nom.z()*nom.z());
	double m_normal = sqrt(normal.x()*normal.x() + normal.y()*normal.y() + normal.z()*normal.z());
	double ji = nom.x() * normal.x() + nom.y() * normal.y() + nom.z() * normal.z();
	double angle = acos(ji / (mo * m_normal));
	angle = angle * 180 / osg::PI;

	if (angle < 90 + err &&angle >90 - err)
	{
		return true;
	}
	return false;
}
