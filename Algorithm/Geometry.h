#pragma once
#include <cmath>
#include <vector>
#include <OSG_Plus.h>
#ifndef DLL_API
#define DLL_API __declspec(dllexport)
#endif

namespace CL
{	
	#define EPSILON 0.000001

	//二维double矢量
	typedef struct  Vector2d
	{
		double x,y;
		
		Vector2d() 
		{ 
			x = 0.0;
			y = 0.0;
		}
		Vector2d(double dx, double dy) 
		{ 
			x = dx;
			y = dy; 
		}
		void Set(double dx, double dy) 
		{ 
			x = dx;
			y = dy; 
		}

		//矢量的模
		double Mod() const
		{
			return sqrt(x * x + y * y);
		}

		// 矢量点积
		double Dot(const Vector2d& v) const
		{
			return x * v.x + y * v.y;
		}
		//向量外积
		double Cross(const Vector2d& rhs) const
		{
			return (x * rhs.y - y * rhs.x);
		}

		//符号重载	
		Vector2d operator + (const Vector2d& v) const
		{
			return Vector2d(x + v.x, y + v.y) ;
		}	
		Vector2d operator - (const Vector2d& v) const
		{
			return Vector2d(x - v.x, y - v.y) ;
		}
		bool operator == (const Vector2d& rhs)
		{			
			return ((fabs(x - rhs.x) < EPSILON) && (fabs(y - rhs.y) < EPSILON));			
		}
		bool operator != (const Vector2d& rhs)
		{			
			return ((fabs(x - rhs.x) > EPSILON) || (fabs(y - rhs.y) > EPSILON));			
		}
		bool operator < (const Vector2d& rhs) 
		{
			if (fabs(x - rhs.x) < EPSILON)
			{
				return y < rhs.y;
			}			
			else
			{
				return x < rhs.x;
			}
		}	
	}Vec2d;

	//三维double矢量
	typedef struct Vector3d
	{
		double x,y,z; 

		Vector3d() 
		{ 
			x=0.0;
			y=0.0;
			z=0.0;
		}
		Vector3d(double dx,double dy,double dz) 
		{ 
			x=dx;
			y=dy;
			z=dz; 
		}
		void Set(double dx, double dy,double dz) 
		{ 
			x = dx;
			y = dy; 
			z = dz;
		}
		
		//矢量数乘
		Vector3d Scalar(double c) const
		{
			return Vector3d(c*x,c*y,c*z) ;
		}		

		//矢量的模
		double Mod() const
		{
			return sqrt(x * x + y * y + z * z);
		}

		// 矢量点积
		double Dot(const Vector3d& v) const
		{
			return x * v.x + y * v.y + z * v.z ;
		}

		// 矢量叉积
		Vector3d Cross(const Vector3d& v) const
		{		
			return Vector3d(y * v.z - z * v.y,  z * v.x - x * v.z,  x * v.y - y * v.x) ;
		} 		

		//符号重载	
		Vector3d operator + (const Vector3d& v) const
		{
			return Vector3d(x + v.x, y + v.y, z + v.z) ;
		}	
		Vector3d operator - (const Vector3d& v) const
		{
			return Vector3d(x - v.x, y - v.y, z - v.z) ;
		}
 		bool operator == (const Vector3d& rhs)
 		{			
 			return ((fabs(x - rhs.x)<EPSILON) && (fabs(y - rhs.y)<EPSILON) && (fabs(z - rhs.z)<EPSILON));			
 		}
 		bool operator != (const Vector3d& rhs)
 		{			
 			return ((fabs(x - rhs.x)>EPSILON) || (fabs(y - rhs.y)>EPSILON) || (fabs(z - rhs.z)>EPSILON));			
 		}
 		bool operator < (const Vector3d& rhs) 
 		{
			if (fabs(x - rhs.x)<EPSILON)
			{
				if (fabs(y - rhs.y)<EPSILON)
				{
					return z < rhs.z;
				}
				else
				{
					return y < rhs.y;
				}				
			}			
			else
			{
				return x < rhs.x;
			} 			
 		}	
	}Vec3d;

	struct Rect2d
	{
		double minX, minY, maxX, maxY;
	};

	DLL_API bool IsPolygonIntersetPolygon(std::vector<CL::Vec2d> pol1, std::vector<CL::Vec2d> pol2);
	//判断点在多边形内
	DLL_API bool Point_In_Polygon_2D(double x, double y, const std::vector<Vec2d> &POL);
	//计算点到直线的距离 p1 p2 为直线上两点
	DLL_API double getDisFromPointToLine(osg::Vec3d& pt, osg::Vec3d& p1, osg::Vec3d& p2);
	//点到面的距离abcd为平面方程参数
	DLL_API double getDisFromPointToPlane(osg::Vec3d& pt, double& a, double& b, double& c, double& d);
	//计算向量夹角
	DLL_API double getAngleForVector(osg::Vec3d& v1, osg::Vec3d& v2);
	//
	DLL_API bool Point_In_Rectangle_3D(osg::Vec3d& pt, osg::Vec3d& pt1, std::vector<osg::Vec3d>& coord);
	//判断两线段相交
	DLL_API bool IsIntersect(double px1, double py1, double px2, double py2, double px3, double py3, double px4, double py4);

	//计算两点距离
	DLL_API double distance_of_2_Point3D(osg::Vec3d& pt1, osg::Vec3d& pt2);
	//两矩形是否相交
	bool Rect_Intersect_2D(const Rect2d &rect1, const Rect2d &rect2);

	//ray-triangle intersection algorithm
	//参数说明：V1,V2,V3,三角形三点；O,射线原点；D,射线方向。
	bool Ray_Triangle_Intersection_3D(const Vector3d &V1, const Vector3d &V2, const Vector3d &V3, const Vector3d &O, const Vector3d &D, Vector3d &I);

	//计算三点成面的法向量
	void Cal_Normal_3D(const Vec3d& v1, const Vec3d& v2, const Vec3d& v3, Vec3d &vn);

	//计算两线段交点,请判线段是否相交(同时还是要判断是否平行!)
	DLL_API Vec2d GetLineSegmentIntersection(Vec2d u1, Vec2d u2, Vec2d v1, Vec2d v2);

	inline bool DBL_Equal(double f1, double f2) 
	{
		return (fabs(f1 - f2) < EPSILON);
	}	

	/*
	// 判断点P是否在三角形ABC内(同向法)
	bool PointinTriangle(Vector3d A, Vector3d B, Vector3d C, Vector3d P);

	*/
};



