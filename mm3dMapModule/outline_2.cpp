#include "stdafx.h"
#include "outline_2.h"
#include <osgUtil/IntersectVisitor>
#include <osg/LineSegment>
#include <osg/Point>

namespace outline_2
{
	double STEP = 0.0;

	void cacuDeltaXY(osg::Vec3d pt1,osg::Vec3d pt2,double &deltaX,double &deltaY)
	{
		double angle;
		if(pt1.x() == pt2.x())
		{
			deltaX = STEP;
			deltaY = 0;
			return;
		}
		if (pt1.y() == pt2.y())
		{
			deltaX = 0;
			deltaY = STEP;
			return;
		}
		angle  = atan( abs(pt2.y()-pt1.y()) / abs(pt2.x()-pt1.x()));
		if (pt1.x() < pt2.x())
		{
			deltaX = STEP*cos(angle);
		}
		else
			deltaX = -STEP*cos(angle);
		if (pt1.y() < pt2.y())
		{
			deltaY = STEP*sin(angle);
		}
		else
			deltaY = -STEP*sin(angle);
		return;
	}
	
	
	DLL_OUTLINE_API_2 void ComputeOutline(std::vector<osg::Vec3d> Area,osg::Vec3d Pt,osg::Group *mRoot,std::vector<osg::Vec3d>& vec)
	{
		if (Area.size() < 3)
		{
			return;
		}
		if (Pt.x() > 100000 || Pt.y() > 100000)//绝对定向坐标采用大步长
		{
			STEP = 0.1;
		}
		else
			STEP = 0.01;
		Area.push_back(*(Area.begin()));
		osg::Vec3d _start,_end,center;
		double X=0,Y=0;//平均值计算中心点
		for (std::vector<osg::Vec3d>::iterator ite = Area.begin(); ite != Area.end() - 1; ++ite)
		{
			X += (*ite).x();
			Y += (*ite).y();
		}
		center.x() = X/(Area.size()-1);
		center.y() = Y/(Area.size()-1);
		center.z() = Pt.z();

		_start = *(Area.begin());//获取第一个点
		_start.z() = Pt.z();
		osg::ref_ptr<osgUtil::LineSegmentIntersector> ls;
		for (std::vector<osg::Vec3d>::iterator ite = Area.begin() + 1; ite != Area.end(); ++ite)
		{
			_end = *ite;
			_end.z() = Pt.z();

			cacuDeltaXY(_start,_end,deltaX,deltaY);//计算步长
			/////////////碰撞检测///////////////
			while (dist2(_start,_end)>2*STEP)
			{
				ls = new osgUtil::LineSegmentIntersector(_start, center);
				osgUtil::IntersectionVisitor iv(ls);
				mRoot->accept(iv);
				if (ls.valid() && ls->containsIntersections())
				{
					vec.push_back(ls->getFirstIntersection().getWorldIntersectPoint());//如果碰撞，存储碰撞点
				}
				_start.x()+=deltaX;_start.y()+=deltaY;
			}
			////////////////////////////////////////
			_start = _end;
		}
	}


}