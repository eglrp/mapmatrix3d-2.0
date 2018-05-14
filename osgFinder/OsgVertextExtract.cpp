#include "StdAfx.h"
#include "qcomm.h"
#include <osg/Texture2D>
#include <osgViewer/Renderer>
#include "OsgVertextExtract.h"

COsgVertextExtract::COsgVertextExtract():osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
	extracted_verts = new osg::Vec3Array; 
}

COsgVertextExtract::~COsgVertextExtract(void)
{
}

void COsgVertextExtract::ExtractVerts(osg::ref_ptr<osg::Node> node, osg::ref_ptr<osg::Vec3Array> &extracted_verts)
{
	COsgVertextExtract ive;
	node->accept(ive);
	extracted_verts = ive.extracted_verts;
}

//apply 
void COsgVertextExtract::apply(osg::Geode& geode)
{ 
	//看看有多少可绘制结点 
	int numDrawables = geode.getNumDrawables();

	for( unsigned int i=0; i < geode.getNumDrawables(); ++i ) 
	{ 
		osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(i)); 

		if( !geom ) 
			continue; //得到可绘制结点的顶点序列 
		
		osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray()); 

		if( !verts ) 
			continue; //把顶点序列插入到顶点集中以便输出 
		extracted_verts->insert(extracted_verts->end(), verts->begin(), verts->end()); 
	} 
}

//apply 
void COsgVertextReplace::apply(osg::Geode& geode)
{
	osg::ref_ptr<osg::Camera> camera = _viewer->getCamera();
	osgViewer::Renderer *render = dynamic_cast<osgViewer::Renderer *>(camera->getRenderer());
	osgUtil::SceneView *sceneView = render->getSceneView(0);
	osg::Vec3 vScreen;
	//看看有多少可绘制结点 
	int numDrawables = geode.getNumDrawables();

	for (unsigned int i = 0; i < geode.getNumDrawables(); ++i)
	{
		osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));

		if (!geom)
			continue; //得到可绘制结点的顶点序列 

		osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		if (!verts)
			continue; 
		for (size_t j = 0; j < verts->size(); ++j)
		{
			osg::Vec3d pt = verts->at(j);
			//pt = pt * _matrix;
			sceneView->projectObjectIntoWindow(pt, vScreen);
			if (vScreen.x() <= _x + _pixSize && vScreen.x() >= _x - _pixSize && vScreen.y() >= _y - _pixSize && vScreen.y() <= _y + _pixSize)
			{
				getNearestInsect(_scenceNode, pt, _disErr);
				verts->at(j) = pt;
				geom->setVertexArray(verts);
				return;
			}
		}
	}
}

void COsgVertextReplace::getNearestInsect(osg::ref_ptr<osg::Node> node, osg::Vec3d& worldPt, double radiu)
{
	osg::Vec3d pt = worldPt,pt1, norm(0, 0, 0);
	double dis = radiu;
	std::vector<std::vector<osg::Vec3d> > vecVec;
	std::vector<osg::Vec3d> vec;
	for (float a = 0; a <= 90; ++a)//1相限
	{
		norm.x() = radiu*cos(a / 180.0*osg::PI);
		norm.y() = radiu*sin(a / 180.0*osg::PI);
		osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(worldPt, worldPt + norm);
		osgUtil::IntersectionVisitor iv(ls);
		node->accept(iv);
		if (ls.valid() && ls->containsIntersections())
		{
			osg::Vec3d p = ls->getFirstIntersection().getWorldIntersectPoint();//如果碰撞，存储碰撞点
			if ((worldPt - p).length() < radiu)
			{
				if (dis > (worldPt - p).length())
				{
					dis = (worldPt - p).length();
					pt = p;
				}
				
				vec.push_back(p);
			}
		}
		else
		{
			if (vec.size() > 2)
			{
				vecVec.push_back(vec);
			}
			vec.clear();
		}
	}

	for (float a = 91; a <= 180; ++a)//2相限
	{
		norm.x() = -radiu*cos((180 - a) / 180.0*osg::PI);
		norm.y() = radiu*sin((180 - a) / 180.0*osg::PI);
		osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(worldPt, worldPt + norm);
		osgUtil::IntersectionVisitor iv(ls);
		node->accept(iv);
		if (ls.valid() && ls->containsIntersections())
		{
			osg::Vec3d p = ls->getFirstIntersection().getWorldIntersectPoint();//如果碰撞，存储碰撞点
			if ((worldPt - p).length() < radiu)
			{
				if (dis > (worldPt - p).length())
				{
					dis = (worldPt - p).length();
					pt = p;
				}
				
				vec.push_back(p);
			}
			
		}
		else
		{
			if (vec.size() > 2)
			{
				vecVec.push_back(vec);
			}
			vec.clear();
		}
	}

	for (float a = 181; a <= 270; ++a)//3相限
	{
		norm.x() = -radiu*cos((a - 180) / 180.0*osg::PI);
		norm.y() = -radiu*sin((a - 180) / 180.0*osg::PI);
		osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(worldPt, worldPt + norm);
		osgUtil::IntersectionVisitor iv(ls);
		node->accept(iv);
		if (ls.valid() && ls->containsIntersections())
		{
			osg::Vec3d p = ls->getFirstIntersection().getWorldIntersectPoint();//如果碰撞，存储碰撞点
			if ((worldPt - p).length() < radiu)
			{
				if (dis > (worldPt - p).length())
				{
					dis = (worldPt - p).length();
					pt = p;
				}
				
				vec.push_back(p);
			}
			
		}
		else
		{
			if (vec.size() > 2)
			{
				vecVec.push_back(vec);
			}
			vec.clear();
		}
	}

	for (float a = 271; a < 360; ++a)//4相限
	{
		norm.x() = radiu*cos((360 - a) / 180.0*osg::PI);
		norm.y() = -radiu*sin((360 - a) / 180.0*osg::PI);
		osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(worldPt, worldPt + norm);
		osgUtil::IntersectionVisitor iv(ls);
		node->accept(iv);
		if (ls.valid() && ls->containsIntersections())
		{
			osg::Vec3d p = ls->getFirstIntersection().getWorldIntersectPoint();//如果碰撞，存储碰撞点
			if ((worldPt - p).length() < radiu)
			{
				if (dis > (worldPt - p).length())
				{
					dis = (worldPt - p).length();
					pt = p;
				}
				
				vec.push_back(p);
			}
			
		}
		else
		{
			if (vec.size() > 2)
			{
				vecVec.push_back(vec);
			}
			vec.clear();
		}
	}
	/*if (dis < radiu)
		worldPt = pt;*/
	float ql = 180;
	for (int i = 0; i < vecVec.size(); ++i)
	{
		for (int j = 1; j < vecVec[i].size() - 1; ++j)
		{
			float a = getAngle(vecVec[i][j - 1] - vecVec[i][j], vecVec[i][j + 1] - vecVec[i][j]);
			if (a < ql)
			{
				ql = a;
				pt1 = vecVec[i][j];
			}
		}
	}
	if (ql < 150)
		worldPt = pt1;
	else
		worldPt = pt;
}


float COsgVertextReplace::getAngle(osg::Vec3d p1, osg::Vec3d p2)
{
	double m_normal = p2.length();
	double mo = p1.length();
	double ji = p1.x() * p2.x() + p1.y() * p2.y() + p1.z() * p2.z();
	double angle = acos(ji / (mo * m_normal));
	angle = angle * 180 / osg::PI;
	
	return angle;
}