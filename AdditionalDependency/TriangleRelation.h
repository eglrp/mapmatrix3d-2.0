#pragma once
#include "OSG_Plus.h"

#ifdef TRIANGLERELATIONLIBDLL  
#define TRIANGLERELATIONAPI _declspec(dllexport)  
#else  
#define TRIANGLERELATIONAPI  _declspec(dllimport)  
#endif  

class TRIANGLERELATIONAPI CTriangle
{
public:
	osg::Vec3d pt1;
	osg::Vec3d pt2;
	osg::Vec3d pt3;

	osg::Vec4 color;//面片颜色
	osg::Vec3d normal; //三角面法向量 
public:
	bool neighborWith(CTriangle& other);//是否相邻
	bool operator == (const CTriangle& other);//重载 ==  
	CTriangle & operator = (const CTriangle &triangle); //赋值运算符的重载  
	
private:
	bool pointInTriangle(osg::Vec3d pt, const CTriangle& triangle);
};

class TRIANGLERELATIONAPI CTriangleRelation
{
public:
	CTriangleRelation()
	{
		neighbor.clear();
	}

	CTriangle self;                             //中心面片
	std::vector<CTriangle> neighbor;            //相邻三角面片
};

//自定义类用于泛型查找算法谓词
class findTriangle
{
public:

	findTriangle(const CTriangle& triangle) { tri = triangle; }
	bool operator()(CTriangleRelation& dValue)
	{
		if (dValue.self == tri)
			return true;
		else
			return false;
	}
	CTriangle tri;
};

