#include "stdafx.h"
#include "outline_1.h"
#include <osgUtil/IntersectVisitor>
#include <osg/LineSegment>
#include <osg/Point>
#include<conio.h>


#define DIS 100.0

namespace outline
{
	double STEP = 0.1;
	osg::StateSet* makePtState(int size)
{
	//设置样式
	osg::StateSet* set = new osg::StateSet();
	osg::ref_ptr<osg::Point> point = new osg::Point();
	point->setSize(size);
	set->setAttribute(point);
	set->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	set->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	return set;
}

osg::Geode* drawPoint(osg::Vec3d p, double r, double g, double b, double a, double size)
{
	double x = p.x();double y = p.y();double z = p.z();
	//绘制点
	osg::Geode* geode = new osg::Geode();
	osg::Geometry* geomPt = new osg::Geometry();
	//设定点
	osg::Vec3Array* ptArray = new osg::Vec3Array();
	ptArray->push_back(osg::Vec3d(x, y, z));
	geomPt->setVertexArray(ptArray);
	//设定颜色
	osg::Vec4Array* colorArray = new osg::Vec4Array();
	colorArray->push_back(osg::Vec4f(r, g, b, a));
	geomPt->setColorArray(colorArray);
	geomPt->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	//设定点样式
	geomPt->setStateSet(makePtState(size));

	geomPt->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, ptArray->size()));
	geode->addDrawable(geomPt);
	return geode;
}
bool testIntersection(osg::Vec3d &p1,osg::Vec3d &p2,osg::Group *mRoot,osg::Group *mMeasureResultGroup)
{
	osg::ref_ptr<osg::LineSegment> line;
	osgUtil::IntersectVisitor iv;
	osg::Geode* geode;
	line = new osg::LineSegment(p1,p2);
	iv.addLineSegment(line);
	mRoot->accept(iv);
	if (iv.hits())
	{
		osgUtil::IntersectVisitor::HitList& hitList = iv.getHitList(line.get());	
		/*if(hitList.size()%2 == 0)
			return false;*/
		if(!hitList.empty())
		{
			//p1 = hitList.front().getWorldIntersectPoint();
			return true;
		}
		return false;
	}	
	return false;			
}

bool drawSection(osg::Vec3d &p1,osg::Vec3d &p2,osg::Group *mRoot,int s)//画线段交点
{
	//碰撞检测
	osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(p1, p2);
	// 创建一个IV
	osgUtil::IntersectionVisitor iv(ls);
	//iv.setTraversalMask(0xffffffff);
	// 把线段添加到IV中
	mRoot->accept(iv);
	if (ls.valid() && ls->containsIntersections())
	{
		osg::Vec3d pp =ls->getFirstIntersection().getWorldIntersectPoint();

		if(s==1)//下
		{
			if (pp.y()+2*STEP<p1.y())//如果下打点超过一个步长
			{
				for (double y =2* STEP;p1.y()-y>pp.y();y+=STEP)
				{
					osg::ref_ptr<osgUtil::LineSegmentIntersector> ls1 = new osgUtil::LineSegmentIntersector(
						osg::Vec3d(p1.x(),p1.y()-y,p1.z()),
						osg::Vec3d(p1.x()-DIS,p1.y()-y,p1.z()));
					osgUtil::IntersectionVisitor iv1(ls1);
					mRoot->accept(iv1);
					if (ls1.valid() && ls1->containsIntersections())
					{
						vecPoints.push_back(ls1->getFirstIntersection().getWorldIntersectPoint());
					}
					//delete ls1;
				}
			}
		}
		else if(s==2)//左
		{
			if (pp.x()+2*STEP<p1.x())//如果下打点超过一个步长
			{
				for (double x = 2*STEP;p1.x()-x>pp.x();x+=STEP)
				{
					osg::ref_ptr<osgUtil::LineSegmentIntersector> ls2 = new osgUtil::LineSegmentIntersector(
						osg::Vec3d(p1.x()-x,p1.y(),p1.z()),
						osg::Vec3d(p1.x()-x,p1.y()+DIS,p1.z()));
					osgUtil::IntersectionVisitor iv2(ls2);
					mRoot->accept(iv2);
					if (ls2.valid() && ls2->containsIntersections())
					{
						vecPoints.push_back(ls2->getFirstIntersection().getWorldIntersectPoint());
					}
					//delete ls2;
				}
			}
		}
		else if(s==3)//上
		{
			if (pp.y()-2*STEP>p1.y())//如果下打点超过一个步长
			{
				for (double y = 2*STEP;p1.y()+y<pp.y();y+=STEP)
				{
					osg::ref_ptr<osgUtil::LineSegmentIntersector> ls3 = new osgUtil::LineSegmentIntersector(
						osg::Vec3d(p1.x(),p1.y()+y,p1.z()),
						osg::Vec3d(p1.x()+DIS,p1.y()+y,p1.z()));
					osgUtil::IntersectionVisitor iv3(ls3);
					mRoot->accept(iv3);
					if (ls3.valid() && ls3->containsIntersections())
					{
						vecPoints.push_back(ls3->getFirstIntersection().getWorldIntersectPoint());
					}
					//delete ls3;
				}
			}
		}
		else if(s==4)//右
		{
			if (pp.x()-2*STEP>p1.x())//如果下打点超过一个步长
			{
				for (double x = 2*STEP;p1.x()+x<pp.x();x+=STEP)
				{
					osg::ref_ptr<osgUtil::LineSegmentIntersector> ls4 = new osgUtil::LineSegmentIntersector(
						osg::Vec3d(p1.x()+x,p1.y(),p1.z()),
						osg::Vec3d(p1.x()+x,p1.y()-DIS,p1.z()));
					osgUtil::IntersectionVisitor iv4(ls4);
					mRoot->accept(iv4);
					if (ls4.valid() && ls4->containsIntersections())
					{
						vecPoints.push_back(ls4->getFirstIntersection().getWorldIntersectPoint());
					}
					//delete ls4;
				}
			}
		}
		vecPoints.push_back(pp);
		p1 = pp;
		//delete ls;
		return true;
	}
	else
	{	
		return false;
	}
}


void MoveUP(osg::Vec3d &p,osg::Group *mRoot,int s)//y轴正向
{
	//_cprintf("up!\n");
	osg::Geode* geode;
	osg::ref_ptr<osg::LineSegment> line;
	osgUtil::IntersectVisitor iv;
	osg::Vec3d p1 = p,pp = p;
	
	if(s == 1)
	{
		p1.y()+=STEP;
		
		//while(testIntersection(p,p1,mRoot,mMeasureResultGroup))
		//{
		//	p1.y()+=100;
		//	line = new osg::LineSegment(p1,p);
		//	iv.addLineSegment(line);
		//	mRoot->accept(iv);
		//	osgUtil::IntersectVisitor::HitList& hitList = iv.getHitList(line.get());
		//	hitList.pop_back();//hitList.pop_back();
		//	p = hitList.back().getWorldIntersectPoint();
		//	p1 = p;
		//	p1.y()+=STEP;
		//}

		/*osg::Vec3d p1 = p;
		p.y()+= 0.000005;
		p1.y()+=STEP;
		if(testIntersection(p,p1,mRoot,mMeasureResultGroup))
		{
			while(1)
			{
				p = p1;
				p1.y()+=STEP;
				if(testIntersection(p,p1,mRoot,mMeasureResultGroup))
				{
					p = p1;
					break;
				}
				else
				{
					
				}
			}
		}
		else*/
		
		p = p1;
		
		return;		
	}
	if(s == 2)
	{
		p1.y() -= STEP;
		
		//while(testIntersection(p,p1,mRoot,mMeasureResultGroup))
		//{
		//	p1.y()-=100;
		//	line = new osg::LineSegment(p1,p);
		//	iv.addLineSegment(line);
		//	mRoot->accept(iv);
		//	osgUtil::IntersectVisitor::HitList& hitList = iv.getHitList(line.get());
		//	hitList.pop_back();//hitList.pop_back();
		//	p = hitList.back().getWorldIntersectPoint();
		//	p1 = p;
		//	p1.y()-=STEP;
		//}
		//while(drawSection(p,osg::Vec3d(p.x(),p.y()-STEP,p.z()),mRoot,mMeasureResultGroup))
		//{
		//	//vecPoints.push_back(p);
		//	p.x()+=STEP;
		//}
		
		/*osg::Vec3d p1 = p;
		p1.y()-=STEP;
		p.y()-=0.000005;
		if(testIntersection(p,p1,mRoot,mMeasureResultGroup))
		{
			while(1)
			{
				p = p1;
				p1.y()-=STEP;
				if(testIntersection(p,p1,mRoot,mMeasureResultGroup))
				{
					p = p1;
					break;
				}
				else
				{
					
				}
			}
		}
		else*/
		
		p = p1;
		
		return;				
	}
	if(s == 3)
	{
		p1.x() -= STEP;
		
		//while(testIntersection(p,p1,mRoot,mMeasureResultGroup))
		//{
		//	p1.x()-=100;
		//	line = new osg::LineSegment(p1,p);
		//	iv.addLineSegment(line);
		//	mRoot->accept(iv);
		//	osgUtil::IntersectVisitor::HitList& hitList = iv.getHitList(line.get());
		//	hitList.pop_back();//hitList.pop_back();
		//	p = hitList.back().getWorldIntersectPoint();
		//	p1 = p;
		//	p1.x()-=STEP;
		//}
		//while(drawSection(p,osg::Vec3d(p.x()-STEP,p.y(),p.z()),mRoot,mMeasureResultGroup))
		//{
		//	//vecPoints.push_back(p);
		//	p.y()-=STEP;
		//}
		
		/*osg::Vec3d p1 = p;
		p1.x()-=STEP;
		p.x()-=0.000005;
		if(testIntersection(p,p1,mRoot,mMeasureResultGroup))
		{
			while(1)
			{
				p = p1;
				p1.x()-=STEP;
				if(testIntersection(p,p1,mRoot,mMeasureResultGroup))
				{
					p = p1;
					break;
				}
				else
				{
					
				}
			}
		}
		else*/
	
			p = p1;	
		
		return;				
		
	}
	if(s == 4)
	{
		p1.x() += STEP;

		
		//while(drawSection(p,osg::Vec3d(p.x()+STEP,p.y(),p.z()),mRoot,mMeasureResultGroup))
		//{
		//	//vecPoints.push_back(p);
		//	p.y()+=STEP;
		//}
	    
		/*osg::Vec3d p1 = p;
		p1.x()+=STEP;
		p.x()+=0.000005;
		if(testIntersection(p,p1,mRoot,mMeasureResultGroup))
		{
			while(1)
			{
				p = p1;
				p1.x()+=STEP;
				if(testIntersection(p,p1,mRoot,mMeasureResultGroup))
				{
					p = p1;
					break;
				}
				else
				{
					
				}
			}
		}
		else*/
		p = p1;
		
		return;		
		
	}
}

void MoveBack(osg::Vec3d &p1,osg::Vec3d &p2,osg::Group *mRoot,int s)//后退
{
	//碰撞检测
	osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(p1, p2);
	// 创建一个IV
	osgUtil::IntersectionVisitor iv(ls);
	//iv.setTraversalMask(0xffffffff);
	// 把线段添加到IV中
	mRoot->accept(iv);
	if (ls.valid() && ls->containsIntersections())
	{
		p2 =ls->getFirstIntersection().getWorldIntersectPoint();
		switch(s)
		{
		case 1:{p1.x()+=(0.5*(p2.x()-p1.x()));break;}
		case 4:{p1.y()+=(0.5*(p2.y()-p1.y()));break;}
		case 2:{p1.x()+=(0.5*(p2.x()-p1.x()));break;}
		case 3:{p1.y()+=(0.5*(p2.y()-p1.y()));break;}
		}
		//delete ls;
		return;
	}
	else
		p1 = p2;
}

void MoveForward(osg::Vec3d &p,osg::Group *mRoot,int s)//前进 分4个方向
{
	osg::Vec3d pu;
	osg::Geode* geode;
	//_cprintf("dx:%lf dy:%lf\n",dx,dy);
	switch(s)
	{
	case 1:
		pu = osg::Vec3d(p.x()+1.1*STEP, p.y(), p.z() ) ;break;//前进一格
	case 2:
		pu = osg::Vec3d(p.x()-1.1*STEP, p.y(), p.z() ) ;break;//前进一格
	case 3:
		pu = osg::Vec3d(p.x(), p.y()+1.1*STEP, p.z() ) ;break;//前进一格
	case 4:
		pu = osg::Vec3d(p.x(), p.y()-1.1*STEP, p.z() ) ;break;//前进一格
	default:return;
	}
	//碰撞检测
	osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(p, pu);
	osgUtil::IntersectionVisitor iv(ls);
	//iv.setTraversalMask(0xffffffff);
	// 把线段添加到IV中
	mRoot->accept(iv);
	if (ls.valid() && ls->containsIntersections())
	{
		osg::Vec3d p1 =ls->getFirstIntersection().getWorldIntersectPoint();
		isDown__ = false;
		vecPoints.push_back(p1);
		p = p1;//如果有碰撞，返回碰撞点
	}
	else
	{
		isDown__ = true ;
		p = pu;//无碰撞   返回前进点
	}	
}


//由一个碰撞点计算轮廓线
DLL_OUTLINE_API void ComputeOutline(osg::Vec3d pt,osg::Vec3d nm,osg::Group *mRoot,std::vector<osg::Vec3d> &vec)
{
	if (pt.x() > 100000 || pt.y() > 100000)//绝对定向坐标采用大步长
	{
		STEP = 0.1;
	}
	else
		STEP = 0.01;
	osg::ref_ptr<osg::LineSegment> line;
	osgUtil::IntersectVisitor iv;
	osg::Geode* geode;
	int director;
	isDown__ = false;
	vecPoints.clear();
	vecPoints.push_back(pt);
	//第一步  判断点的初始移动方向
	
	osg::Vec3d pt_= pt;

	if (nm.y()<0)//如果有交点  初始方向沿y负
	{
		director = 1;
	}
	else
		director = 2;//否则初始方向沿y正

	if(director == 1)
	{
		_cprintf("director = 1!\n");
		bool isBack = false;
		crashCount = 0;
		while(1)//开始以x负为前进方向
		{
			while(!isDown__)
			{
				if(isBack)
					//pt_.x() += 0.01*STEP;
					MoveBack(pt_,osg::Vec3d(pt_.x()+0.1*STEP,pt_.y(),pt_.z()),mRoot,2);
				MoveUP(pt_, mRoot, 2);//上
				isDown__ = false;
				MoveForward(pt_, mRoot, 2);//前进
				if(isDown__)
					break;
				isBack = true;
			}
			if(!drawSection(pt_, osg::Vec3d(pt_.x(),pt.y(),pt_.z()), mRoot,3))
				break;

			isDown__ = false;
		}

		pt_.y() += (STEP);
		//pt_.x() -= STEP;
		isDown__ = false;
		isBack = false;
		crashCount = 0;
		while(1)//开始以y正为前进方向
		{
			while(!isDown__)
			{
				if(isBack)
					//pt_.y() -= 0.01*STEP;
					MoveBack(pt_, osg::Vec3d(pt_.x(),pt_.y()-0.1*STEP,pt_.z()), mRoot, 3);
				MoveUP(pt_, mRoot, 3);//上
				isDown__ = false;
				MoveForward(pt_,mRoot,3);//前进
				if(isDown__)
					break;
				isBack = true;
			}
			if(!drawSection(pt_,osg::Vec3d(pt.x(),pt_.y(),pt_.z()),mRoot,4))
				break;
			isDown__ = false;
		}
		//pt_.y() -= (STEP);
		pt_.x() += STEP;
		isDown__ = false;
		isBack = false;
		crashCount = 0;
		while(1)//开始以x正为前进方向
		{
			while(!isDown__)
			{
				if(isBack)
					MoveBack(pt_,osg::Vec3d(pt_.x()-0.1*STEP,pt_.y(),pt_.z()),mRoot,1);
				MoveUP(pt_,mRoot,1);//上
				isDown__ = false;
				MoveForward(pt_,mRoot,1);//前进
				if(isDown__)
					break;
				isBack = true;
			}
			if(!drawSection(pt_,osg::Vec3d(pt_.x(),pt.y(),pt_.z()),mRoot,1))
				break;
			isDown__ = false;
		}
	
		pt_.y() -= (STEP);
		//pt_.x() -= STEP;
		isDown__ = false;
		isBack = false;
		crashCount = 0;
		while(1)//开始以y负为前进方向
		{
			while(!isDown__)
			{
				if(isBack)
					/*pt_.y() += 0.01*STEP;*/
					MoveBack(pt_,osg::Vec3d(pt_.x(),pt_.y()+0.1*STEP,pt_.z()),mRoot,4);
				MoveUP(pt_,mRoot,4);//上
				isDown__ = false;
				MoveForward(pt_,mRoot,4);//前进
				if(isDown__)
					break;
				isBack = true;
			}
			if(!drawSection(pt_,osg::Vec3d(pt.x(),pt_.y(),pt_.z()),mRoot,2))
				break;
			isDown__ = false;
		}
	}
	else
	{
		_cprintf("director = 2!\n");
		bool isBack = false;
		isDown__ = false;
		crashCount = 0;
		while(1)//开始以x正为前进方向
		{
			while(!isDown__)
			{
				if(isBack)
					MoveBack(pt_,osg::Vec3d(pt_.x()-0.1*STEP,pt_.y(),pt_.z()),mRoot,1);
					//pt_.x() -= 0.01*STEP;
				MoveUP(pt_,mRoot,1);//上
				
				
				MoveForward(pt_,mRoot,1);//前进
				if(isDown__)
					break;
				isBack = true;
			}
			if(!drawSection(pt_,osg::Vec3d(pt_.x(),pt.y(),pt_.z()),mRoot,1))
			{
				if(crashCount == 0 )
					break;
				else
				{
					pt_.x()-=STEP;
				}
			}
			isBack = false;
			isDown__ = false;
		}

		pt_.y() -= (STEP);
		//pt_.x() -= STEP;
		isDown__ = false;
		isBack = false;
		crashCount = 0;
		while(1)//开始以y负为前进方向
		{
			while(!isDown__)
			{
				if(isBack)
					/*pt_.y() += 0.01*STEP;*/
					MoveBack(pt_,osg::Vec3d(pt_.x(),pt_.y()+0.1*STEP,pt_.z()),mRoot,4);
				MoveUP(pt_,mRoot,4);//上
				//isDown__ = false;
				//pt_.y() += 0.01*STEP;
				MoveForward(pt_,mRoot,4);//前进
				if(isDown__)
					break;
				isBack = true;
			}
			if(!drawSection(pt_,osg::Vec3d(pt.x(),pt_.y(),pt_.z()),mRoot,2))
			{
				if(crashCount == 0 )
					break;
				else
				{
					pt_.y()+=STEP;
				}
			}
			isDown__ = false;
			isBack = false;
		}

		pt_.x() -= STEP;
		//pt_.y() += STEP;
		isDown__ = false;
		isBack = false;
		crashCount = 0;
		while(1)//开始以x负为前进方向
		{
			while(!isDown__)
			{
				if(isBack)
					//pt_.x() += 0.01*STEP;
					MoveBack(pt_,osg::Vec3d(pt_.x()+0.1*STEP,pt_.y(),pt_.z()),mRoot,2);
				MoveUP(pt_,mRoot,2);//上
				//isDown__ = false;
				//pt_.x() += 0.01*STEP;
				MoveForward(pt_,mRoot,2);//前进
				if(isDown__)
					break;
				isBack = true;
			}
			if(!drawSection(pt_,osg::Vec3d(pt_.x(),pt.y(),pt_.z()),mRoot,3))
			{
				if(crashCount == 0 )
					break;
				else
				{
					pt_.x()+=STEP;
				}
			}
			isDown__ = false;
			isBack = false;
		}

		//pt_.x() += STEP;
		pt_.y() += STEP;
		isDown__ = false;
		isBack = false;
		crashCount = 0;
		while(1)//开始以y正为前进方向
		{
			while(!isDown__)
			{
				if(isBack)
					//pt_.y() -= 0.01*STEP;
					MoveBack(pt_,osg::Vec3d(pt_.x(),pt_.y()-0.1*STEP,pt_.z()),mRoot,3);
				MoveUP(pt_,mRoot,3);//上
				//isDown__ = false;
				//pt_.y() -= 0.01*STEP;
				MoveForward(pt_,mRoot,3);//前进
				if(isDown__)
					break;
				isBack = true;
			}
			if(!drawSection(pt_,osg::Vec3d(pt.x(),pt_.y(),pt_.z()),mRoot,4))
			{
				if(crashCount == 0 )
					break;
				else
				{
					pt_.y()-=STEP;
				}
			}
			isDown__ = false;
			isBack = false;
		}
		
	
	}

	/*for(std::vector<osg::Vec3d>::iterator ite = vecPoints.begin();ite!=vecPoints.end();ite++)
	{
		geode = drawPoint(osg::Vec3d((*ite).x(), (*ite).y(), (*ite).z()), 1, 0, 0, 1, 2.5); 
		mMeasureResultGroup->addChild(geode);
	}*/
	vec.assign(vecPoints.begin(),vecPoints.end());

}


osg::Geode* drawLine(osg::Vec3d p1,osg::Vec3d p2, double r, double g, double b, double a, double size)
{
	double x = p1.x();double x1 = p2.x();
	double y = p1.y();double y1 = p2.y();
	double z = p1.z();double z1 = p2.z();

	//绘制线
	osg::Geode* geode = new osg::Geode();
	osg::Geometry* geomPt = new osg::Geometry();
	//设定点
	osg::Vec3Array* ptArray = new osg::Vec3Array();
	ptArray->push_back(osg::Vec3d(x, y, z));
	ptArray->push_back(osg::Vec3d(x1, y1, z1));
	geomPt->setVertexArray(ptArray);
	//设定颜色
	osg::Vec4Array* colorArray = new osg::Vec4Array();
	colorArray->push_back(osg::Vec4f(r, g, b,a));
	geomPt->setColorArray(colorArray);
	//geomPt->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	//设定线样式
	osg::StateSet* set = new osg::StateSet();
	osg::ref_ptr<osg::LineSegment> line = new osg::LineSegment();
	set->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	geomPt->setStateSet(set);
	//geomPt->setNormalBinding(osg::Geometry::BIND_OVERALL);
	geomPt->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, ptArray->size()));
	geode->addDrawable(geomPt);
	return geode;
}
}

