#include "stdafx.h"
#include "TriangleRelation.h"


bool CTriangle::neighborWith(CTriangle& other)
{
	if (!(*this == other))
	{
		if (pointInTriangle(this->pt1, other) || pointInTriangle(this->pt2, other) || pointInTriangle(this->pt3, other))
		{
			return true;
		}
	}
	return false;
}

bool CTriangle::operator == (const CTriangle& other)
{
	if (pointInTriangle(this->pt1,other) && pointInTriangle(this->pt2, other) && pointInTriangle(this->pt3, other))
	{
		return true;
	}
	return false;
}

CTriangle& CTriangle::operator = (const CTriangle &triangle)
{
	pt1 = triangle.pt1;
	pt2 = triangle.pt2;
	pt3 = triangle.pt3;
	color = triangle.color;
	normal = triangle.normal;
	return *this;
}

bool CTriangle::pointInTriangle(osg::Vec3d pt, const CTriangle& triangle)
{
	if (pt == triangle.pt1 || pt == triangle.pt2 || pt == triangle.pt3)
	{
		return true;
	}
	return false;
}