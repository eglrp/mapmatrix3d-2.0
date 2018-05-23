#include "stdafx.h"
#include "geometry.h"
#include <algorithm>

using namespace std;

#define GEOMETRYLIBDLL  

namespace CL
{	
	//判断点在线段上
	bool IsPointOnLine(double px0, double py0, double px1, double py1, double px2, double py2)
	{
		bool flag = false;   
		double d1 = (px1 - px0) * (py2 - py0) - (px2 - px0) * (py1 - py0);
		if ((abs(d1) < EPSILON)&&((px0 - px1) * (px0 - px2) <= 0)&&((py0 - py1) * (py0 - py2) <= 0)) 
		{   
			flag = true;   
		}
		return flag;   
	}

	//判断两线段相交
	bool IsIntersect(double px1, double py1, double px2, double py2, double px3, double py3, double px4, double py4) 
	{         
		bool flag = false;   
		double d = (px2 - px1) * (py4 - py3) - (py2 - py1) * (px4 - px3);   
		if (d != 0) 
		{   
			double r = ((py1 - py3) * (px4 - px3) - (px1 - px3) * (py4 - py3))/d;   
			double s = ((py1 - py3) * (px2 - px1) - (px1 - px3) * (py2 - py1))/d;   
			if ((r >= 0) && (r <= 1) && (s >= 0) && (s <= 1)) 
			{ 
				flag = true;   
			}	
		} 		
		return flag; 
	}   

	bool IsPolygonIntersetPolygon(std::vector<CL::Vec2d> pol1, std::vector<CL::Vec2d> pol2)
	{
		std::vector<CL::Vec2d> rectPol2;
		rectPol2 = pol2;
		rectPol2.push_back(pol2[0]);

		std::vector<CL::Vec2d> rectPol1;
		rectPol1 = pol1;
		rectPol1.push_back(pol1[0]);
		//判断顶点是否在里面
		for(int i = 0; i < pol1.size(); i++)
		{
			if(Point_In_Polygon_2D(pol1.at(i).x, pol1.at(i).y, rectPol2))
			{
				return true;
			}
		}

		//判断顶点是否在里面
		for(int i = 0; i < pol2.size(); i++)
		{
			if(Point_In_Polygon_2D(pol2.at(i).x, pol2.at(i).y, rectPol1))
			{
				return true;
			}
		}

		//判断是否相交
		for (int i = 0; i < pol1.size(); i++)
		{
			for (int j = 0; j < pol2.size(); j++)
			{
				int pol1Index1 = i;
				int pol1Index2 = i + 1;
				int pol2Index1 = j;
				int pol2Index2 = j + 1;

				if (i == pol1.size() - 1)
				{
					pol1Index2 = 0;
				}

				if (j == pol2.size() - 1)
				{
					pol2Index2 = 0;
				}

				if(CL::IsIntersect(pol1.at(pol1Index1).x, pol1.at(pol1Index1).y, pol1.at(pol1Index2).x, pol1.at(pol1Index2).y, pol2.at(pol2Index1).x, pol2.at(pol2Index1).y, pol2.at(pol2Index2).x, pol2.at(pol2Index2).y))
				{
					return true;
				};
			}

		}

		return false;
	}


	//判断点在多边形内
	bool Point_In_Polygon_2D(double x, double y, const vector<CL::Vec2d> &POL)
	{
		bool isInside = false; 
		int count = 0;   

		double px = x;
		double py = y;
		double linePoint1x = x;   
		double linePoint1y = y;   
		double linePoint2x = -99999999.9;//0;       
		double linePoint2y = y;      

		for(int i = 0; i < POL.size()-1; i++)
		{
			double cx1 = POL[i].x;   
			double cy1 = POL[i].y;    
			double cx2 = POL[i+1].x;    
			double cy2 = POL[i+1].y;  

			if(IsPointOnLine(px, py, cx1, cy1, cx2, cy2))    
			{
				return true; 		
			}

			if (fabs(cy2 - cy1) < EPSILON)   //平行则不相交
			{
				continue;   
			}   

			if (IsPointOnLine(cx1, cy1, linePoint1x, linePoint1y, linePoint2x, linePoint2y)) 
			{   
				if (cy1 > cy2)   
				{
					count++; 
				}
			} 
			else if (IsPointOnLine(cx2, cy2, linePoint1x, linePoint1y, linePoint2x, linePoint2y))
			{ 
				if (cy2 > cy1)  
				{
					count++; 
				}
			} 
			else if (IsIntersect(cx1, cy1, cx2, cy2, linePoint1x, linePoint1y, linePoint2x, linePoint2y))   //已经排除平行的情况
			{ 
				count++;  
			}   
		}

		if (count % 2 == 1) 
		{   
			isInside = true;
		}   

		return isInside;		
	}

	//点到直线距离
	double getDisFromPointToLine(osg::Vec3d& pt, osg::Vec3d& p1, osg::Vec3d& p2)
	{
		double pX = pt.x();
		double pY = pt.y();
		double pZ = pt.z();
		double lineBackX = p1.x();
		double lineBackY = p1.y();
		double lineBackZ = p1.z();
		double lineFrontX = p2.x();
		double lineFrontY = p2.y();
		double lineFrontZ = p2.z();
		double sa, sb, sc, cosA, sinA, H;
		sa = sqrt((pX - lineBackX)*(pX - lineBackX) + (pY - lineBackY)*(pY - lineBackY) + (pZ - lineBackZ)*(pZ - lineBackZ));
		sb = sqrt((lineBackX - lineFrontX) *(lineBackX - lineFrontX) + (lineBackY - lineFrontY) *(lineBackY - lineFrontY) + (lineBackZ - lineFrontZ) *(lineBackZ - lineFrontZ));
		sc = sqrt((pX - lineFrontX)*(pX - lineFrontX) + (pY - lineFrontY)*(pY - lineFrontY) + (pZ - lineFrontZ)*(pZ - lineFrontZ));
		cosA = (sb*sb + sc*sc - sa*sa) / (2.0 * sb * sc);
		sinA = sqrt(1.0 - cosA*cosA);
		H = sc * sinA;
		return H;
	}

	//点到面的距离
	double getDisFromPointToPlane(osg::Vec3d& pt, double& a, double& b, double& c, double& d)
	{
		return (a * pt.x() + b * pt.y() + c * pt.z() + d) / sqrt(a * a + b*b + c * c);
	}

	//计算向量夹角
	double getAngleForVector(osg::Vec3d& v1, osg::Vec3d& v2)
	{
		double m1 = sqrt(v1.x()*v1.x() + v1.y()*v1.y() + v1.z()*v1.z());
		double m2 = sqrt(v2.x()*v2.x() + v2.y()*v2.y() + v2.z()*v2.z());

		double angle = acos((v1.x()*v2.x() + v1.y() * v2.y() + v1.z()*v2.z()) / (m1*m2)) * 180 / osg::PI;
		return angle;
	}

	bool Point_In_Rectangle_3D(osg::Vec3d& pt, osg::Vec3d& pt1, std::vector<osg::Vec3d>& coord)
	{
		if (coord.size() != 4)
		{
			return false;
		}
		double minx, miny, minz, maxx, maxy, maxz;
		minx = maxx = coord[0].x();
		miny = maxy = coord[0].y();
		minz = maxz = coord[0].z();

		for (size_t i = 1; i < coord.size(); ++i)
		{
			if (coord[i].x() < minx)
			{
				minx = coord[i].x();
			}
			else if (coord[i].x() > maxx)
			{
				maxx = coord[i].x();
			}
			if (coord[i].y() < miny)
			{
				miny = coord[i].y();
			}
			else if (coord[i].y() > maxy)
			{
				maxy = coord[i].y();
			}
			if (coord[i].z() < minz)
			{
				minz = coord[i].z();
			}
			else if (coord[i].z() > maxz)
			{
				maxz = coord[i].z();
			}
		}
		if ((pt.x() >= minx) && (pt.x() <= maxx) && (pt.y() >= miny) && (pt.y() <= maxy) && (pt.z() >= minz) && (pt.z() <= maxz))
		{
			double dis = 9999;
			double t;
			int index = -1;
			for (int i = 0; i < coord.size(); ++i)
			{
				if ((t = distance_of_2_Point3D(pt, coord[i])) <= dis)
				{
					dis = t;
					index = i;
				}
			}
			switch (index)
			{
			case 0:pt1 = coord[2]; break;
			case 1:pt1 = coord[3]; break;
			case 2:pt1 = coord[0]; break;
			case 3:pt1 = coord[1]; break;
			default:return false;
			}
			return true;
		}
		return false;
	}

	double distance_of_2_Point3D(osg::Vec3d& p1, osg::Vec3d& p2)
	{
		return sqrt((p1.x() - p2.x())*(p1.x() - p2.x()) + (p1.y() - p2.y())*(p1.y() - p2.y()) + (p1.z() - p2.z())*(p1.z() - p2.z()));
	}

	//ray-triangle intersection algorithm
	//参数说明：V1,V2,V3,三角形三点；O,射线原点；D,射线方向。
	bool Ray_Triangle_Intersection_3D(const Vector3d &V1, const Vector3d &V2, const Vector3d &V3, const Vector3d &O, const Vector3d &D, Vector3d &I)
	{
		//Find vectors for two edges sharing V1
		Vector3d e1 = V2 - V1;
		Vector3d e2 = V3 - V1;

		//Begin calculating determinant - also used to calculate u parameter
		Vector3d P = D.Cross(e2);
		//if determinant is near zero, ray lies in plane of triangle
		double det = e1.Dot(P);
		//NOT CULLING
		if(det > -EPSILON && det < EPSILON) 
		{	
			return false;
		}
		double inv_det = 1.0 / det;

		//calculate distance from V1 to ray origin
		Vector3d T = O - V1;

		//Calculate u parameter and test bound
		double u = T.Dot(P) * inv_det;
		//The intersection lies outside of the triangle
		if(u < 0.0 || u > 1.0) 
		{
			return false;
		}

		//Prepare to test v parameter
		Vector3d Q = T.Cross(e1);
		//Calculate V parameter and test bound
		double v =  D.Dot(Q) * inv_det;

		//The intersection lies outside of the triangle
		if(v < 0.0 || u + v  > 1.0) 
		{
			return false;
		}

		double t =  e2.Dot(Q) * inv_det;

		//ray intersection
		if(t > EPSILON) 
		{ 		
			I = O + D.Scalar(t);		
			return true;
		}

		return false;
	}

	//两矩形是否相交
	bool Rect_Intersect_2D(const Rect2d &rect1, const Rect2d &rect2)
	{
		bool flag = false;

		double minx = (std::max) (rect1.minX, rect2.minX);  
		double miny = (std::max) (rect1.minY, rect2.minY);  
		double maxx = (std::min) (rect1.maxX, rect2.maxX);  
		double maxy = (std::min) (rect1.maxY,	rect2.maxY);

		if (minx>maxx||miny>maxy)
		{
			flag = false;
		}
		else
		{
			flag =true;
		}

		return flag;
	}

	//计算三点成面的法向量
	void Cal_Normal_3D(const Vec3d& v1, const Vec3d& v2, const Vec3d& v3, Vec3d &vn)
	{
		//v1(n1,n2,n3);
		//平面方程: na * (x – n1) + nb * (y – n2) + nc * (z – n3) = 0 ;
		double na = (v2.y - v1.y) * (v3.z - v1.z) - (v2.z - v1.z) * (v3.y - v1.y);
		double nb = (v2.z - v1.z) * (v3.x - v1.x) - (v2.x - v1.x) * (v3.z - v1.z);
		double nc = (v2.x - v1.x) * (v3.y - v1.y) - (v2.y - v1.y) * (v3.x - v1.x);

		//平面法向量
		vn.Set(na, nb, nc);
	}

	//计算两线段交点,请判线段是否相交(同时还是要判断是否平行!)
	Vec2d GetLineSegmentIntersection(Vec2d u1, Vec2d u2, Vec2d v1, Vec2d v2) 
	{
		Vec2d ret = u1;
		double t = ((u1.x - v1.x) * (v1.y - v2.y) - (u1.y - v1.y) * (v1.x - v2.x)) / ((u1.x - u2.x) * (v1.y - v2.y) - (u1.y - u2.y) * (v1.x - v2.x));
		ret.x += (u2.x - u1.x) * t;
		ret.y += (u2.y - u1.y) * t;
		
		return ret;
	}


	/*
	// v1 = Cross(AB, AC)
	// v2 = Cross(AB, AP)
	// 判断矢量v1和v2是否同向
	bool SameSide(Vector3d A, Vector3d B, Vector3d C, Vector3d P)
	{
		//if (d_vi==2375&&fi==339977)
		//{
		//	cout<<"1111111"<<endl;
		//}
		Vector3d AB = B - A ;
		Vector3d AC = C - A ;
		Vector3d AP = P - A ;


		Vector3d v1 = AB.Cross(AC) ;
		Vector3d v2 = AB.Cross(AP) ;	

		//if (d_vi==2375&&fi==339977)
		//{
		//	cout<<"2222222"<<endl;
		//	cout<<flag<<endl;
		//}

		// v1 and v2 should point to the same direction
		//return v1.Dot(v2) >= 0 ;
		return v1.Dot(v2) > 0 ;		
	}

	// 判断点P是否在三角形ABC内(同向法)
	bool PointinTriangle(Vector3d A, Vector3d B, Vector3d C, Vector3d P)
	{	
		return SameSide(A, B, C, P) && SameSide(B, C, A, P) && SameSide(C, A, B, P) ;
	}
	*/
};

