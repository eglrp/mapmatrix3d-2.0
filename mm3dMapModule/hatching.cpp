#include "stdafx.h"
#include"hatching.h"
#include <osgUtil/IntersectVisitor>
#include <osg/LineSegment>
#include <osg/Point>
#include<conio.h>
#include <osg/MatrixTransform>
#include "InterfaceMFCExcute.h"
#include "publicDefined.h"
#define DIS 100

double STEP;
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

//画线函数
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

void MoveUp(osg::Vec3d &p,std::vector<osg::Vec3d>& vecLine,osg::Group *mRoot)//上移
{
	p.z()+=STEP;
}



void MoveDown(osg::Vec3d &p,std::vector<osg::Vec3d>& vecLine,osg::Group *mRoot,osg::Matrix transMatrix)//下移
{
	osg::Vec3d pu,p1;
	pu = osg::Vec3d(p.x(), p.y(), p.z() - DIS) ;//下移一格
	//碰撞检测
	osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(p, pu);
	// 创建一个IV
	osgUtil::IntersectionVisitor iv(ls);
	//iv.setTraversalMask(0xffffffff);
	// 把线段添加到IV中
	mRoot->accept(iv);
	if (ls.valid() && ls->containsIntersections())
	{
		restart = 0;
		p1 =ls->getFirstIntersection().getWorldIntersectPoint();
		if((p1.z()+STEP)<p.z())
		{
			pu = p;
			int i = 2;
			for(pu.z()-=STEP;pu.z()>p1.z();i++)
			{
				osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(
					pu,
					osg::Vec3d(pu.x()-DIS*deltaX, pu.y()-DIS*deltaY, pu.z()));
				osgUtil::IntersectionVisitor iv(ls);
				//iv.setTraversalMask(0xffffffff);
				// 把线段添加到IV中
				mRoot->accept(iv);
				if (ls.valid() && ls->containsIntersections())
				{
					osg::Vec3d p2 =ls->getFirstIntersection().getWorldIntersectPoint();
					vecLine.push_back(p2);
				}
				pu.z()-=STEP;
			}
		}
		vecLine.push_back(p1);
		p = p1;//如果有碰撞，返回碰撞点,否则继续下移
		return;
	}
	else
	{
		restart+=1;
		p.x()-=deltaX;
		p.y()-=deltaY;
		pu = osg::Vec3d(p.x()-DIS*deltaX, p.y()-DIS*deltaY, p.z()) ;//shang移一格
		ls = new osgUtil::LineSegmentIntersector(p, pu);
		osgUtil::IntersectionVisitor iv(ls);
		iv.setTraversalMask(0xffffffff);
		// 把线段添加到IV中
		mRoot->accept(iv);
		if (ls.valid() && ls->containsIntersections())
		{
			p1 =ls->getFirstIntersection().getWorldIntersectPoint();
			vecLine.push_back(p1);
			//p = p1;//如果有碰撞，返回碰撞点,否则继续下移
			return;
		}
	}
}

void MoveForward(osg::Vec3d &p,double &dx,double &dy,std::vector<osg::Vec3d>& vecLine,osg::Group *mRoot,osg::Matrix transMatrix)//前进
{
	osg::Vec3d pu;
	pu = osg::Vec3d(p.x()+1.1*dx, p.y()+1.1*dy, p.z() ) ;//前进一格
	//碰撞检测
	osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(p, pu);
	// 创建一个IV
	osgUtil::IntersectionVisitor iv(ls);
	//iv.setTraversalMask(0xffffffff);
	// 把线段添加到IV中
	mRoot->accept(iv);
	if (ls.valid() && ls->containsIntersections())
	{
		osg::Vec3d p1 =ls->getFirstIntersection().getWorldIntersectPoint();
		isDown = false;
		vecLine.push_back(p1);
		p.x() = p1.x();//如果有碰撞，返回碰撞点
		p.y() = p1.y();
		return;
	}
	isDown = true ;
	p.x()+=dx;//无碰撞   返回前进点
	p.y()+=dy;	
}

void MoveBack(osg::Vec3d &p1,osg::Vec3d &p2,osg::Group *mRoot,osg::Matrix transMatrix)
{
	//碰撞检测
	osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(p1, p2);
	// 创建一个IV
	osgUtil::IntersectionVisitor iv(ls);
	iv.setTraversalMask(0xffffffff);
	// 把线段添加到IV中
	mRoot->accept(iv);
	if (ls.valid() && ls->containsIntersections())
	{
		p2 =ls->getFirstIntersection().getWorldIntersectPoint();
		p1.x()+=(0.5*(p2.x()-p1.x()));
		p1.y()+=(0.5*(p2.y()-p1.y()));
		return;
	}
	else
		p1 = p2;
}

//void drawHatching(osg::Vec3d p1,osg::Vec3d p2,osg::Group *mRoot,osg::Group *mMeasureResultGroup,osg::Matrix transMatrix)
//{
//	std::vector<osg::Vec3d> vecLine;
//	computeHatching(p1, p2, mRoot, vecLine, transMatrix);
//
//	for(std::vector<osg::Vec3d>::iterator ite = vecLine.begin();ite!=vecLine.end()-1;ite++)
//	{
//		osg::ref_ptr<osg::Geode> geode = drawLine(osg::Vec3d((*ite).x(), (*ite).y(), (*ite).z()),osg::Vec3d((*(ite+1)).x(), (*(ite+1)).y(), (*(ite+1)).z()),1, 0,0, 1, 3.5);
//		mMeasureResultGroup->addChild(geode);
//	}
//	////osg::Vec3d xyzTrans = worldMatrix.getTrans();
//	//for(std::vector<osg::Vec3d>::iterator ite = vecLine.begin();ite!=vecLine.end();ite++)
//	//{
//
//	//	//geode = drawPoint(osg::Vec3d((*ite).x()-xyzTrans.x(), (*ite).y()-xyzTrans.y(), (*ite).z()-xyzTrans.z()),1, 0,0, 2, 3.5);
//	//	
//	//	geode = drawPoint(osg::Vec3d((*ite).x()-xyzTrans.x(), (*ite).y()-xyzTrans.y(), (*ite).z()-xyzTrans.z()),1, 0,0, 1, 3.5);
//	//	osg::Matrix drawMatrix;
//	//	drawMatrix.setTrans(xyzTrans);
//
//	//	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
//	//	trans->setMatrix(drawMatrix);
//	//	trans->addChild(geode);
//	//	mMeasureResultGroup->addChild(trans);
//	//}
//}

DLL_HATCH_API void computeHatching(osg::Vec3d p1,osg::Vec3d p2,osg::Group *mRoot,std::vector<osg::Vec3d>& vecLine, osg::Matrix transMatrix)//计算剖面线，输出点集
{

	if (p1.x() > 100000 || p1.y() > 100000)//绝对定向坐标采用大步长
	{
		STEP = 0.1;
	}
	else
		STEP = 0.01;
	worldMatrix = transMatrix;
	double point1X = p1.x();double point2X = p2.x();
	double point1Y = p1.y();double point2Y = p2.y();
	double point1Z = p1.z();double point2Z = p2.z();
	
	double start_x,start_y,start_z; 
	double end_x ,end_y,end_z;
	double angle;//夹角

	restart = 0;
	if(point1X<point2X)
	{
		start_x = point1X;
		end_x = point2X;
		start_y = point1Y;
		end_y = point2Y;
		start_z = point1Z;
		end_z = point2Z;
	}
	if(point1X>point2X)
	{
		start_x = point2X;
		end_x = point1X;
		start_y = point2Y;
		end_y = point1Y;
		start_z = point2Z;
		end_z = point1Z;
	}
	if(point1X==point2X)//1x = 2x
	{
		start_y = (point1Y>point2Y)?point2Y:point1Y;
		end_y = (point1Y>point2Y)?point1Y:point2Y;
		start_x = point1X;
		end_x = point2X;
		start_z = (point1Y>point2Y)?point2Z:point1Z;
		end_z = (point1Y>point2Y)?point1Z:point2Z;
	}
	
	//斜率大于0
	if(start_y < end_y)
	{
		angle = atan((end_y-start_y)/(end_x-start_x));
		//angle = osg::PI/180*angle;
		deltaX = STEP * cos(angle);
		deltaY = STEP * sin(angle);
	}
	//斜率小于0
	if(start_y > end_y)
	{
		angle = atan(-(end_y-start_y)/(end_x-start_x));
		//angle = osg::PI/180*angle;
		deltaX = STEP * cos(angle);
		deltaY = -STEP * sin(angle);
		
	}
	if(start_y == end_y)
	{
		deltaX = STEP;
		deltaY = 0;
		
	}
	osg::Vec3d pl = osg::Vec3d(start_x, start_y,start_z) ;//前位置
	osg::Vec3d pn = osg::Vec3d(start_x, start_y, start_z) ;//后位置
	vecLine.clear();
	vecLine.push_back(pl);

	bool isBack = false;
	//_cprintf("%lf,%lf,%lf\n",pn.x(),pn.y(),pn.z());
	while(distance2(pn,osg::Vec3d(end_x,end_y,end_z))>(STEP))
	{
		if(isBack)
			MoveBack(pn,osg::Vec3d(pn.x()-0.1*deltaX,pn.y()-0.1*deltaY,pn.z()),mRoot,worldMatrix);
		MoveUp(pn,vecLine,mRoot);//上移
		MoveForward(pn,deltaX,deltaY,vecLine,mRoot,worldMatrix);

		if(isDown == true)
		{
			MoveDown(pn,vecLine,mRoot,worldMatrix);
			if (restart == 9999)//若果原地连续下打超9999次则计算剖面线失败
			{
				InterfaceMFCExcuteFactory IExcuteFactory;
				InterfaceMFCExcute* IExcute = IExcuteFactory.create();
				IExcute->PopUpMessage("剖面线生成失败!");
				//vecLine.clear();
				return;
			}
			isBack = false;
		}
		else
		{
			isBack = true;
		}
		if (pn.x()>=end_x)
		{
			break;
		}
	}
	
	vecLine.push_back(osg::Vec3d(end_x,end_y,end_z));	
}