// osgExtractOutlineEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <shellapi.h>
#include <tchar.h> 
#include "InterfaceBaseDrawer.h"
#include "osgExtractTriangleBaseOutline.h"
#include "outline_3.h"
#include <osgDB/ReadFile>
#include <osgViewer/Renderer>
#include "osgUtil/RayIntersector"
#include <libpq-fe.h>
#include "InterfaceCOsgExtractObject.h"
#include <osg\ComputeBoundsVisitor>
#include <osgUtil/IntersectVisitor>
#include "osgUtil/RayIntersector"
#include <osgUtil/LineSegmentIntersector>
#include "cv.h"
#include "nbspAlgorithm.h"
using namespace cv;

class Point2D {
public:
	float x;
	float y;
	Point2D()
	{
		x = 0; y = 0;
	}
	Point2D(float x, float y)
	{
		this->x = x; this->y = y;
	}
	friend Point2D operator +(Point2D a, Point2D b)
	{
		Point2D p;
		p.x = a.x + b.x;
		p.y = a.y + b.y;
		return p;
	};
	friend Point2D operator -(Point2D a, Point2D b)
	{
		Point2D p;
		p.x = a.x - b.x;
		p.y = a.y - b.y;
		return p;
	};
	friend float operator *(Point2D a, Point2D b)
	{
		float p = a.x*b.x + a.y*b.y;
		return p;
	};
	friend Point2D operator *(Point2D a, float value)
	{
		Point2D p;
		p.x = a.x*value;
		p.y = a.y*value;
		return p;
	};
	friend float xlJi(Point2D a, Point2D b)
	{
		float p = ((a.x)*(b.y)) - ((a.y)*(b.x));
		return p;
	};
};

double distanceFromPointToPlane(osg::Vec3d pt, double A, double B, double C, double D)
{
	double dis = abs(A*pt.x() + B*pt.y() + C*pt.z() + D) / sqrt(A*A + B*B + C*C);
	return dis;
}

void getPlaneFromFile(osg::Vec3d pt,char* file,double& a,double& b,double& c,double& d,double lim)
{
	char line[256];
	double val[4];
	ifstream in(file);
	while (in.good())
	{
		in.getline(line, 256);
		istringstream iss(line);
		iss >> val[0] >> val[1] >> val[2]>> val[3];
		if (distanceFromPointToPlane(pt, val[0], val[1], val[2], val[3]) <= lim)
		{
			a = val[0];
			b = val[1];
			c = val[2];
			d = val[3];
			in.close();
			return;
		}
	}
	in.close();
}
double getAngle(double a1, double b1, double c1, double a2, double b2, double c2)
{
	//v(0,0,1)
	double m1 = sqrt(a1*a1 + b1*b1 + c1*c1);
	double m2 = sqrt(a2*a2 + b2*b2 + c2*c2);

	double angle = acos((a1*a2 + b1 * b2 + c1*c2) / (m1*m2)) * 180 / 3.1415926;

	if (angle > 90)
	{
		return 180 - angle;
	}
	return angle;
}

void getPointProject(osg::Vec3d& pt, osg::Vec3d& ptPrj, double A, double B, double C,double D)//计算投影点
{
	double k = -distanceFromPointToPlane(pt, A,B,C,D);
	ptPrj.x() = pt.x() + k*A;
	ptPrj.y() = pt.y() + k*B;
	ptPrj.z() = pt.z() + k*C;
}

void COsgExtractTriangleBaseOutLineEvent::showPlane(double A, double B, double C, double D, osg::Vec3d pt, double h, double w)
{
	double angle = getAngle(A, B, C, 1, 0, 0);
	double L = w / sin(angle / 180.0*osg::PI);
	double L1 = h / sin(angle / 180.0*osg::PI);
	double x1 = pt.x() - L;
	double x2 = pt.x() + L;
	double y = pt.y();
	double z1 = pt.z() - h;
	double z2 = pt.z() + h;
	std::vector<osg::Vec3d> vecPt;
	/*osg::Vec3d p;
	getPointProject(osg::Vec3d(x1,-(A*x1 +C*z1 + D)/B,z1),p,A,B,C,D);
	vecPt.push_back(p);
	getPointProject(osg::Vec3d(x1, -(A*x1 + C*z2 + D) / B, z2), p, A, B, C, D);
	vecPt.push_back(p);
	getPointProject(osg::Vec3d(x2, -(A*x2 + C*z2 + D) / B, z2), p, A, B, C, D);
	vecPt.push_back(p);
	getPointProject(osg::Vec3d(x2, -(A*x2 + C*z1 + D) / B, z1), p, A, B, C, D);
	vecPt.push_back(p);*/
	vecPt.push_back(osg::Vec3d(x1, -(A*x1 + C*z1 + D) / B, z1));
	vecPt.push_back(osg::Vec3d(x1, -(A*x1 + C*z2 + D) / B, z2));
	vecPt.push_back(osg::Vec3d(x2, -(A*x2 + C*z2 + D) / B, z2));
	vecPt.push_back(osg::Vec3d(x2, -(A*x2 + C*z1 + D) / B, z1));

	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	IDrawer->BaseDrawPolygon(vecPt, osg::Vec4(0,1,1,1), mapperGroup, worldMatrix);
}

void fitLineLikeWater(osg::ref_ptr<osg::Node> root, osg::Vec3d ps, osg::Vec3d pe, double HatchLenth, double step, std::vector<osg::Vec3d>& vecPtOut)
{
	double diff = 0;
	double defaultH = ps.z();
	std::vector<osg::Vec3d> vec3d;
	if (ps.x() == pe.x())
	{
		int k = 0;
		if (ps.y() > pe.y())
		{
			while (ps.y() - k*step >= pe.y())
			{
				osg::Vec3d p1(ps.x() - HatchLenth / 2, ps.y() - k*step - diff, defaultH);
				osg::Vec3d p2(ps.x() + HatchLenth / 2, ps.y() - k*step - diff, defaultH);
				vec3d.push_back(p1);
				vec3d.push_back(p2);
				k++;
			}
		}
		else
		{
			while (ps.y() + k*step <= pe.y())
			{
				osg::Vec3d p1(ps.x() - HatchLenth / 2, ps.y() + k*step - diff, defaultH);
				osg::Vec3d p2(ps.x() + HatchLenth / 2, ps.y() + k*step - diff, defaultH);
				vec3d.push_back(p1);
				vec3d.push_back(p2);
				k++;
			}
		}
	}
	else if (ps.y() == pe.y())
	{
		int k = 0;
		if (ps.x() < pe.x())
		{
			while (ps.x() + k*step <= pe.x())
			{
				osg::Vec3d p1(ps.x() + k*step, ps.y() + HatchLenth / 2, defaultH);
				osg::Vec3d p2(ps.x() + k*step, ps.y() - HatchLenth / 2, defaultH);
				vec3d.push_back(p1);
				vec3d.push_back(p2);
				k++;
			}
		}
		else
		{
			while (ps.x() - k*step >= pe.x())
			{
				osg::Vec3d p1(ps.x() - k*step, ps.y() + HatchLenth / 2, defaultH);
				osg::Vec3d p2(ps.x() - k*step, ps.y() - HatchLenth / 2, defaultH);
				vec3d.push_back(p1);
				vec3d.push_back(p2);
				k++;
			}
		}
	}
	else
	{
		float k = (ps.y() - pe.y()) / (ps.x() - pe.x());
		float b = ps.y() - k*ps.x();
		float cosa = abs(ps.x() - pe.x()) / (ps - pe).length();
		int m = 0;
		if (ps.x() < pe.x())
		{
			while (ps.x() + m*(step*cosa) <= pe.x())
			{
				double cx = ps.x() + m*(step*cosa);
				double cy = k*cx + b;

				double y1 = cy + cosa*HatchLenth / 2;
				double y2 = cy - cosa*HatchLenth / 2;
				float nk = -1 / k;
				float nb = cy - nk*cx;
				osg::Vec3d p1((y1 - nb) / nk, y1, defaultH);
				osg::Vec3d p2((y2 - nb) / nk, y2, defaultH);
				vec3d.push_back(p1);
				vec3d.push_back(p2);
				m++;
			}
		}
		else
		{
			while (ps.x() - m*(step*cosa) >= pe.x())
			{
				double cx = ps.x() - m*(step*cosa);
				double cy = k*cx + b;

				double y1 = cy + cosa*HatchLenth / 2;
				double y2 = cy - cosa*HatchLenth / 2;
				float nk = -1 / k;
				float nb = cy - nk*cx;
				osg::Vec3d p1((y1 - nb) / nk, y1, defaultH);
				osg::Vec3d p2((y2 - nb) / nk, y2, defaultH);
				vec3d.push_back(p1);
				vec3d.push_back(p2);
				m++;
			}
		}
	}
	
	for (int i = 0;i<vec3d.size()-1;++i)
	{
		osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(vec3d[i], vec3d[i+1]);
		osgUtil::IntersectionVisitor iv(ls);
		root->accept(iv);
		std::vector<osg::Vec3d> vecIntersect;
		osg::Vec3d mid = (vec3d[i] + vec3d[i + 1]) / 2;
		if (ls.valid() && ls->containsIntersections())
		{
			osgUtil::LineSegmentIntersector::Intersections intersections = ls->getIntersections();
			for (osgUtil::LineSegmentIntersector::Intersections::iterator iter = intersections.begin(); iter != intersections.end(); iter++)
			{
				vecIntersect.push_back(iter->getWorldIntersectPoint());
			}
			if (vecIntersect.empty())
			{
				continue;
			}
			else if (vecIntersect.size() == 1)
			{
				vecPtOut.push_back(vecIntersect[0]);
			}
			else
			{
				osg::Vec3d pt;
				double dis = 999;
				for (int j = 0;j<vecIntersect.size();++j)
				{
					double d = (vecIntersect[j] - mid).length();
					if (d<dis)
					{
						dis = d;
						pt = vecIntersect[j];
					}
				}
				vecPtOut.push_back(pt);
			}
		}
	}
	ofstream out;
	out.open("d:\\123.txt");
	for (int i = 0;i<vecPtOut.size();++i)
	{
		out << vecPtOut[i].x() << " " << vecPtOut[i].y() << " " << vecPtOut[i].z() << std::endl;
	}
	out.close();
}

void polygonCornerFilter(std::vector<osg::Vec3d>& vecIn)
{
	if (vecIn.size() < 4)
		return;
	std::vector<osg::Vec3d> vecFilter;
	CPGController pgControl;
	osg::Vec3d p1 = vecIn.back() - vecIn.front();
	osg::Vec3d p2 = vecIn[1] - vecIn[0];
	while ( abs(180 - pgControl.getAngle(point3D(p1.x(),p1.y(),p1.z()),point3D(p2.x(),p2.y(),p2.z()))) <= 1)
	{
		vecIn.erase(vecIn.begin());
		p1 = vecIn.back() - vecIn.front();
		p2 = vecIn[1] - vecIn[0];
	}
	vecFilter.push_back(vecIn.front());
	p1 = vecIn.front() - vecIn.back();
	p2 = vecIn[vecIn.size() - 2] - vecIn.back();
	while (abs(180 - pgControl.getAngle(point3D(p1.x(), p1.y(), p1.z()), point3D(p2.x(), p2.y(), p2.z()))) <= 1)
	{
		vecIn.pop_back();
		p1 = vecIn.front() - vecIn.back();
		p2 = vecIn[vecIn.size() - 2] - vecIn.back();
	}
	
	
	for(int i = 1;i<vecIn.size() - 1;++i)
	{ 
		p1 = vecIn[i-1] - vecIn[i];
		p2 = vecIn[i+1] - vecIn[i];
		if (abs(180 - pgControl.getAngle(point3D(p1.x(), p1.y(), p1.z()), point3D(p2.x(), p2.y(), p2.z()))) > 1)
		{
			vecFilter.push_back(vecIn[i]);
		}
	}
	vecFilter.push_back(vecIn.back());

	vecIn.assign(vecFilter.begin(), vecFilter.end());
}

void COsgExtractTriangleBaseOutLineEvent::rectExtend(std::vector<osg::Vec3d>& vecRect, float dist)
{
	std::vector<Point2D> pList;
	std::vector<Point2D> DpList;
	std::vector<Point2D> nDpList;
	std::vector<Point2D> newList;
	Point2D  p2d, p2;

	for (size_t i = 0; i < vecRect.size(); i++)
	{
		p2d.x = vecRect[i].x(); p2d.y = vecRect[i].y();
		pList.push_back(p2d);
	}
	double zz = vecRect[0].z();
	int  index, count;
	count = pList.size();
	for (int i = 0; i < count; i++)
	{
		index = (i + 1) % count;
		p2d = pList[index] - pList[i];
		DpList.push_back(p2d);
	}
	float r;
	for (int i = 0; i < count; i++)
	{
		r = sqrt(DpList[i] * DpList[i]);
		r = 1 / r;
		p2d = DpList[i] * r;
		nDpList.push_back(p2d);
	}

	float lenth;
	vecRect.clear();
	int startindex, endindex;
	for (int i = 0; i < count; i++)
	{
		startindex = i == 0 ? count - 1 : i - 1;
		endindex = i;
		float sina = xlJi(nDpList[startindex], nDpList[endindex]);
		lenth = dist / sina;
		p2d = nDpList[endindex] - nDpList[startindex];
		p2 = pList[i] + p2d*lenth;
		vecRect.push_back(osg::Vec3d(p2.x, p2.y, zz));
	}
}


void COsgExtractTriangleBaseOutLineEvent::replace_all(std::string& s, std::string const & t, std::string const & w)
{
	string::size_type pos = s.find(t), t_size = t.size(), r_size = w.size();
	while (pos != string::npos) { // found   
		s.replace(pos, t_size, w);
		pos = s.find(t, pos + r_size);
	}
}

void COsgExtractTriangleBaseOutLineEvent::outRect(const vector<osg::Vec3d>& vecOsgPts, vector<osg::Vec3d>& vec)
{
	vec.clear();
	double z = vecOsgPts[0].z();
	vector<Point2f>points;
	for (int i = 0; i < vecOsgPts.size(); i++)
	{
		Point2f pt;
		pt.x = vecOsgPts[i].x();
		pt.y = vecOsgPts[i].y();
		points.push_back(pt);
	}
	RotatedRect box = minAreaRect(Mat(points));//点集的最小外接旋转矩形
	Point2f vertices[4];      //定义矩形的4个顶点  
	box.points(vertices);   //计算矩形的4个顶点  
	for (size_t i = 0; i < 4; i++)
	{
		vec.push_back(osg::Vec3d(vertices[i].x, vertices[i].y, z));
	}
}

void COsgExtractTriangleBaseOutLineEvent::createCircle(osg::Vec3d center, double radiu, std::vector<osg::Vec3d>& _vertices)
{
	double offs = radiu / 20.0;
	for (double d = center.x() - radiu; d < center.x() + radiu; d += offs)
	{
		_vertices.push_back(osg::Vec3d(d, sqrt(radiu*radiu - (d - center.x())*(d - center.x())) + center.y(), center.z()));
	}
	for (double d = center.x() + radiu; d > center.x() - radiu; d -= offs)
	{
		_vertices.push_back(osg::Vec3d(d, -sqrt(radiu*radiu - (d - center.x())*(d - center.x())) + center.y(), center.z()));
	}

}

void COsgExtractTriangleBaseOutLineEvent::create8Polygon(osg::Vec3d center, double radiu, std::vector<osg::Vec3d>& _vertices)
{
	double Lx = sin(22.5 / 180 * osg::PI)*radiu;
	double Ly = cos(22.5 / 180 * osg::PI)* radiu;
	_vertices.push_back(osg::Vec3d(center.x() + Lx, center.y() + Ly, center.z()));
	_vertices.push_back(osg::Vec3d(center.x() + Ly, center.y() + Lx, center.z()));
	_vertices.push_back(osg::Vec3d(center.x() + Ly, center.y() - Lx, center.z()));
	_vertices.push_back(osg::Vec3d(center.x() + Lx, center.y() - Ly, center.z()));
	_vertices.push_back(osg::Vec3d(center.x() - Lx, center.y() - Ly, center.z()));
	_vertices.push_back(osg::Vec3d(center.x() - Ly, center.y() - Lx, center.z()));
	_vertices.push_back(osg::Vec3d(center.x() - Ly, center.y() + Lx, center.z()));
	_vertices.push_back(osg::Vec3d(center.x() - Lx, center.y() + Ly, center.z()));

}

double COsgExtractTriangleBaseOutLineEvent::caculateError(const std::vector<osg::Vec3d>& vecOri, const std::vector<osg::Vec3d>& vecSimp)
{
	if (vecSimp.size() < 4)
		return -1;
	double err = 0, dis = 999;
	for (int i = 0; i < vecOri.size(); ++i)//遍历所有真实点
	{
		dis = 999;
		for (int j = 0; j < vecSimp.size() - 1; ++j)
		{
			if (vecSimp[j + 1].x() == vecSimp[j].x())
			{
				double d = pow(abs(vecOri[i].x() - vecSimp[j].x()), 2);
				if (d < dis)
					dis = d;
				continue;
			}
			float k = (vecSimp[j + 1].y() - vecSimp[j].y()) / (vecSimp[j + 1].x() - vecSimp[j].x());
			double b = vecSimp[j].y() - k*vecSimp[j].x();
			double d = abs(k*vecOri[i].x() - vecOri[i].y() + b) / sqrt(1.0 + k*k);
			d *= d;
			if (d < dis)
				dis = d;
		}
		err += dis;
	}
	err /= vecOri.size();
	return err;
}

void COsgExtractTriangleBaseOutLineEvent::showExtractArea(const osg::Vec3d& worldPt, const osg::Matrix& matrix, osg::ref_ptr<osg::Group> drawGroup)
{
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	ClearGroupResult(drawGroup);

	std::vector<osg::Vec3d> vecCoord;
	create8Polygon(worldPt, m_offs, vecCoord);
	IDrawer->BaseDrawLineLoopOperate(vecCoord, osg::Vec4(1, 1, 0, 1), 2.5, drawGroup, matrix);
}


bool COsgExtractTriangleBaseOutLineEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			worldPt = pickResult.worldPt;
			worldMatrix = pickResult.matrix;
			osg::Vec3d normal = pickResult.worldNm;
			osg::NodePath np = pickResult.nodePath;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			osg::Vec4d color(1.0, 0.0, 0.0, 1.0);
			double size = 3.5;
			
			LeftClickSearchPostGis(ea.getX(), ea.getY(), worldPt, np, normal);
			/*double a, b, c, d;
			getPlaneFromFile(worldPt, "E:\\svn_code2.0\\MM3D_0222\\x64\\Release\\PCL_BIN\\cvFitPlane\\fileCache\\VerticalWalls.txt",a,b,c,d,0.011);
			showPlane(a,b,c,d,worldPt,1,1);*/
		}
		else if (IsLeftClick(ea) && GetKeyState(VK_SHIFT) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			worldPt = pickResult.worldPt;
			worldMatrix = pickResult.matrix;
			float x = ea.getX();
			float y = ea.getY();
			int pixsize = 5;
			InterfaceOsgFinderFactory IFinderFactory;
			InterfaceOsgFinder* IFinder = IFinderFactory.create();
			IFinder->replacePointFound(mViewer,mRoot,mapperGroup,worldMatrix,x,y,pixsize,0.2);

			return false;
		}
		else if (IsMouseMove(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			worldPt = pickResult.worldPt;
			worldMatrix = pickResult.matrix;
			if (worldPt == osg::Vec3d(0, 0, 0))
			{
				return false;
			}

			showExtractArea(worldPt, worldMatrix, tempGroup);
		}
		else if (IsMouseMove(ea) && GetKeyState(VK_CONTROL) >= 0)
		{
			ClearGroupResult(tempGroup);
		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{
				int childNum = mapperGroup->getNumChildren();

				if (childNum > 0)
				{
					mapperGroup->removeChild(childNum - 1);
					char strMsg[99];
					sprintf(strMsg, "无范围提取：按Ctrl + 左键选点，自动提取轮廓线，按ESC退出事件");
					iSigEventCallBack->EmitSigShowOpsHint(strMsg);
				}

			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				ClearGroupResult(mapperGroup);
				char strMsg[99];
				sprintf(strMsg, "无范围提取：按Ctrl + 左键选点，自动提取轮廓线，按ESC退出事件 ");
				iSigEventCallBack->EmitSigShowOpsHint(strMsg);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Equals) //+
			{
				m_offs += 0.1;
				CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
				worldPt = pickResult.worldPt;
				worldMatrix = pickResult.matrix;
				showExtractArea(worldPt, worldMatrix, tempGroup);
				return false;
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Minus) //-
			{
				m_offs -= 0.1;
				CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
				worldPt = pickResult.worldPt;
				worldMatrix = pickResult.matrix;
				showExtractArea(worldPt, worldMatrix, tempGroup);
				return false;
			}
		}
	}

	return false;
}

void COsgExtractTriangleBaseOutLineEvent::getTrianglePointsInScope(osg::ref_ptr<osg::Node> sceneNode, std::vector<osg::Vec3d>& vecWorldCoord, std::vector<CLine>& vecPoints, double h)
{
	std::vector<CScope> vecClipScope;
	CScope scope;
	scope.vecCoord = vecWorldCoord;
	vecClipScope.push_back(scope);
	vecPoints.clear();
	osg::ref_ptr<osg::MatrixTransform> accurateNode = new osg::MatrixTransform;
	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	InterfaceCOsgExtractObjectFactory iOsgExtractObjectFactory;
	InterfaceCOsgExtractObject* iOsgExtractObject = iOsgExtractObjectFactory.create();
	bool isSuccess = iOsgExtractObject->AccurateClipBottomScene(sceneNode, vecClipScope, trans,false);

	if (!isSuccess)
	{
		return;
	}
	osg::Matrix transMatrix = trans->getMatrix();
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();

	InterfaceOsgFinderFactory IOsgFinderFactory;
	InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();

	std::vector<osg::Geode*> vecGeode = IOsgFinder->FindAndGetGeode(trans);
	if (vecGeode.size() == 0)
	{
		vecGeode = IOsgFinder->FindAndGetGeode(sceneNode);
	}
	osg::ref_ptr<osg::Group> group = new osg::Group;
	for (size_t i = 0; i < vecGeode.size(); ++i)
	{
		std::vector<osg::Vec3d> vec0;
		if (!vecGeode[i])
		{
			continue;
		}

		int num = vecGeode[i]->getNumDrawables();

		for (int j = num - 1; j >= 0; --j)
		{
			osg::Geometry* geom = dynamic_cast<osg::Geometry*>(vecGeode[i]->getDrawable(j));

			if (!geom)
				continue; 
			osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
			int a = 0, b = 0;
			for (size_t k = 0; k < geom->getNumPrimitiveSets(); k++)
			{
				osg::PrimitiveSet* priSet = geom->getPrimitiveSet(k);
				osg::ref_ptr<osg::DrawElementsUInt> newDrawElementsTriangle = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
				osg::ref_ptr<osg::DrawElements> drawElementTriangle = priSet->getDrawElements();
				int cnt = drawElementTriangle->getNumIndices();
				int triCnt, mode = 0;
				if (priSet->getMode() == osg::PrimitiveSet::TRIANGLE_STRIP)
				{
					triCnt = cnt - 2;
					mode = 1;
				}
				if (priSet->getMode() == osg::PrimitiveSet::TRIANGLES)
				{
					triCnt = cnt / 3;
				}
				for (int w = 0; w < triCnt; w++)
				{
					int index1 = drawElementTriangle->index(w);
					int index2 = drawElementTriangle->index(w + 1);
					int index3 = drawElementTriangle->index(w + 2);
					if (mode == 0)
					{
						index1 = drawElementTriangle->index(w * 3);
						index2 = drawElementTriangle->index(w * 3 + 1);
						index3 = drawElementTriangle->index(w * 3 + 2);
					}
					newDrawElementsTriangle->push_back(index1);
					newDrawElementsTriangle->push_back(index2);
					newDrawElementsTriangle->push_back(index3);
					geom->setPrimitiveSet(k, newDrawElementsTriangle);
					double x1 = verts->at(index1).x();
					double y1 = verts->at(index1).y();
					double z1 = verts->at(index1).z();
					osg::Vec3d pt1(x1, y1, z1);
					pt1 = pt1 * transMatrix;

					double x2 = verts->at(index2).x();
					double y2 = verts->at(index2).y();
					double z2 = verts->at(index2).z();
					osg::Vec3d pt2(x2, y2, z2);
					pt2 = pt2 * transMatrix;

					double x3 = verts->at(index3).x();
					double y3 = verts->at(index3).y();
					double z3 = verts->at(index3).z();
					osg::Vec3d pt3(x3, y3, z3);
					pt3 = pt3 * transMatrix;
					std::vector<osg::Vec3d> vecSelect, vecres;
					vecSelect.push_back(pt1);
					vecSelect.push_back(pt2);
					vecSelect.push_back(pt3);	
					outline_3::computeOutlinebyPoint(vecSelect, worldPt.z(), worldMatrix, vecres);
					if (vecres.size() == 2)
					{
						vecPoints.push_back(CLine(vecres[0], vecres[1]));
					}
				}
			}
		}
	}
}

void COsgExtractTriangleBaseOutLineEvent::linkLine(PGconn* conn, const osg::Vec3d& worldPt, std::vector<osg::Vec3d>& vecPt)
{
	char sql[9999];
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	std::string curLine, startLine;
	std::vector<std::vector<point3D> > vecVec;
	std::vector<point3D> vec;
	CPGController pgControl;
	//首先找到起始的线段，即离鼠标最近的
	sprintf_s(sql, "select *,st_astext(line),st_distance(line,'point(%f %f)') as dis from tb_temp_line order by dis limit 1 ", worldPt.x(), worldPt.y());
	PGresult* res = PQexec(conn, sql);
	double x, y, z;
	double X, Y, Z;
	std::string sLine;
	int nCal = 0;
	std::vector<osg::Vec3d> vectemp;
	if (PQntuples(res) > 0)
	{
		PQexec(conn, "truncate table tb_temp_triline");
		X = atof(PQgetvalue(res, 0, 2));
		Y = atof(PQgetvalue(res, 0, 3));
		Z = atof(PQgetvalue(res, 0, 4));
		//IDrawer->BaseDrawPtOperate(osg::Vec3d(X,Y,Z),osg::Vec4(0,1,0,1),5.0,mapperGroup,worldMatrix);
		//记录线段的末端坐标
		x = atof(PQgetvalue(res, 0, 5));
		y = atof(PQgetvalue(res, 0, 6));
		z = atof(PQgetvalue(res, 0, 7));
		//获得线段描述
		sLine = PQgetvalue(res, 0, 8);
		startLine = sLine;
		curLine = sLine;
		pgControl.analysisGeometryDataString(sLine, vec, vecVec, GEOMETRY_TYPE::PG_LINESTRING);
		//解析并保存曲线段坐标
		for (int i = 0; i < vec.size(); ++i)
		{
			vecPt.push_back(osg::Vec3d(vec[i].x, vec[i].y, vec[i].z)/* - osg::Vec3d(0,0,0.02)*/);
			vectemp.push_back(osg::Vec3d(vec[i].x, vec[i].y, vec[i].z/* + nCal*0.02*/));
		}
		//IDrawer->BaseDrawCurLineOperate(vectemp, osg::Vec4(1, 0, 0, 1), 2.5, mapperGroup, worldMatrix);
		//删除当前曲线记录
		sLine = "delete from tb_temp_line where st_astext(line) = '" + std::string(PQgetvalue(res, 0, 8)) + "'";
		PQexec(conn, sLine.c_str());
		if ((vecPt.front() - vecPt.back()).length() < 0.001)
		{
			return;
		}
	}
	else
	{
		vecPt.clear();
		return;
	}

	//下面开始连接其他曲线段

	//搜索与上一个末端点最近的曲线
	sprintf_s(sql, "select *,distance3d(x1,y1,0,%f,%f,0) as dis ,st_astext(line) from tb_temp_line order by dis", x, y);
	res = PQexec(conn, sql);
	if (PQntuples(res)>0)
	{
		double ds = atof(PQgetvalue(res, 0, 8));
		sprintf_s(sql, "select *,distance3d(x2,y2,0,%f,%f,0) as dis ,st_astext(line) from tb_temp_line order by dis", x, y);
		res = PQexec(conn, sql);
		double de = atof(PQgetvalue(res, 0, 8));
		if (ds < de)
		{
			sprintf_s(sql, "select *,distance3d(x1,y1,0,%f,%f,0) as dis ,st_astext(line) from tb_temp_line order by dis", x, y);
		}
		else
			sprintf_s(sql, "select *,distance3d(x2,y2,0,%f,%f,0) as dis ,st_astext(line) from tb_temp_line order by dis", x, y);

		res = PQexec(conn, sql);
	}

	while (PQntuples(res) > 0)//如果还有未连接的线
	{
		nCal++;
		string curLine_ = PQgetvalue(res, 0, 9);

		//获得曲线两端
		double x1 = atof(PQgetvalue(res, 0, 2));
		double y1 = atof(PQgetvalue(res, 0, 3));
		double z1 = atof(PQgetvalue(res, 0, 4));
		double x2 = atof(PQgetvalue(res, 0, 5));
		double y2 = atof(PQgetvalue(res, 0, 6));
		double z2 = atof(PQgetvalue(res, 0, 7));
		double ds1 = atof(PQgetvalue(res, 0, 8));

		double ds2 = (vecPt.front() - vecPt.back()).length();
		if (ds2<ds1)
		{
			break;
		}

		curLine = curLine_;
		double dis1 = (osg::Vec3d(x, y, z) - osg::Vec3d(x1, y1, z1)).length();
		double dis2 = (osg::Vec3d(x, y, z) - osg::Vec3d(x2, y2, z2)).length();
		vectemp.clear();
		osg::Vec4 color;
		color = osg::Vec4(1, 0, 1, 1);
		if (dis1 < dis2)
		{
			sLine = curLine;
			vec.clear();
			pgControl.analysisGeometryDataString(sLine, vec, vecVec, GEOMETRY_TYPE::PG_LINESTRING);
			for (int i = 0; i < vec.size(); ++i)
			{
				vecPt.push_back(osg::Vec3d(vec[i].x, vec[i].y, vec[i].z));
				vectemp.push_back(osg::Vec3d(vec[i].x, vec[i].y, vec[i].z));
			}
			//IDrawer->BaseDrawCurLineOperate(vectemp, color, 2.5, mapperGroup, worldMatrix);
			sLine = "delete from tb_temp_line where st_astext(line) = '" + curLine + "'";
			PQexec(conn, sLine.c_str());

		}
		else
		{
			sLine = curLine;
			vec.clear();
			pgControl.analysisGeometryDataString(sLine, vec, vecVec, GEOMETRY_TYPE::PG_LINESTRING);
			for (int i = vec.size() - 1; i >= 0; --i)
			{
				vecPt.push_back(osg::Vec3d(vec[i].x, vec[i].y, vec[i].z));
				vectemp.push_back(osg::Vec3d(vec[i].x, vec[i].y, vec[i].z));
			}
			
			//IDrawer->BaseDrawCurLineOperate(vectemp, color, 2.5, mapperGroup, worldMatrix);
			sLine = "delete from tb_temp_line where st_astext(line) = '" + curLine + "'";
			PQexec(conn, sLine.c_str());
		}

		//更新末端
		x = vecPt.back().x();
		y = vecPt.back().y();
		z = vecPt.back().z();

		sprintf_s(sql, "select *,distance3d(x1,y1,0,%f,%f,0) as dis ,st_astext(line) from tb_temp_line order by dis", x, y);
		res = PQexec(conn, sql);
		if (PQntuples(res) > 0)
		{
			double ds = atof(PQgetvalue(res, 0, 8));
			sprintf_s(sql, "select *,distance3d(x2,y2,0,%f,%f,0) as dis ,st_astext(line) from tb_temp_line order by dis", x, y);
			res = PQexec(conn, sql);
			double de = atof(PQgetvalue(res, 0, 8));
			if (ds < de)
			{
				sprintf_s(sql, "select *,distance3d(x1,y1,0,%f,%f,0) as dis ,st_astext(line) from tb_temp_line order by dis", x, y);
			}
			else
				sprintf_s(sql, "select *,distance3d(x2,y2,0,%f,%f,0) as dis ,st_astext(line) from tb_temp_line order by dis", x, y);
			res = PQexec(conn, sql);
		}
	}
}

bool COsgExtractTriangleBaseOutLineEvent::LeftClickSearchPostGis(float x, float y, osg::Vec3d worldPt, osg::NodePath np, osg::Vec3d nm)
{
	double dd1, dd2;
	CPGController pgControl;
	
	std::vector<osg::Vec3d> vecCoord;
	
	create8Polygon(worldPt, m_offs, vecCoord);
	std::vector<CLine> vecLine;
	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);
	iSigEventCallBack->EmitSigShowOpsHint("正在提取轮廓线");
	getTrianglePointsInScope(sceneNode, vecCoord, vecLine,  worldPt.z());
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	osg::ref_ptr<osg::Group> outlineGroup = new osg::Group;
	int k = 0;
	PQexec(conn, "CREATE TABLE tb_temp_line(id integer,line geometry,x1 double precision,y1 double precision,z1 double precision,x2 double precision,y2 double precision,z2 double precision)");
	PQexec(conn, "truncate table tb_temp_line");
	if (vecLine.size()>0)
	{
		for (int i = 0; i<vecLine.size() - 1; ++i)
		{
			for (int j = i + 1; j<vecLine.size();)
			{
				if (vecLine[i] == vecLine[j])
				{
					vecLine.erase(vecLine.begin() + j);
				}
				else
					++j;
			}
		}
	}
	
	while (vecLine.size() > 0)
	{
		std::list<osg::Vec3d> lineRes;
		CLine line;
		line.p1 = vecLine.front().p1;
		line.p2 = vecLine.front().p2;
		vecLine.erase(vecLine.begin());
		lineRes.push_back(line.p1);
		lineRes.push_back(line.p2);
		for (std::vector<CLine>::iterator ite = vecLine.begin(); ite != vecLine.end();)
		{
			if (((*ite).p1 - lineRes.front()).length()<0.001)
			{
				lineRes.push_front((*ite).p2);
			}
			else if (((*ite).p1 - lineRes.back()).length()<0.001)
			{
				lineRes.push_back((*ite).p2);
			}
			else if (((*ite).p2 - lineRes.front()).length()<0.001)
			{
				lineRes.push_front((*ite).p1);
			}
			else if (((*ite).p2 - lineRes.back()).length()<0.001)
			{
				lineRes.push_back((*ite).p1);
			}
			else
			{
				++ite;
				continue;
			}
			vecLine.erase(ite);
			ite = vecLine.begin();
		}
		//当曲线点数大于2并且首尾不相连时，将曲线保存到数据库
		if (lineRes.size() > 2)
		{
			std::string sLineString("linestring(");
			for (auto pt : lineRes)
			{
				sLineString += (std::to_string(pt.x()) + " " + std::to_string(pt.y()) + " " + std::to_string(pt.z()) + ",");
			}
			sLineString = sLineString.substr(0, sLineString.size() - 1);
			sLineString += ")";


			char sql[9999];
			sprintf_s(sql, "insert into tb_temp_line values(%d,'para',%f,%f,%f,%f,%f,%f)", lineRes.size(),
				lineRes.front().x(), lineRes.front().y(), lineRes.front().z(), lineRes.back().x(), lineRes.back().y(), lineRes.back().z());
			string s(sql);
			replace_all(s, "para", sLineString);
			PQexec(conn, s.c_str());
		}

	}
	vecCoord.clear();
	linkLine(conn, worldPt, vecCoord);
	PQexec(conn, "drop table tb_temp_line");
	std::vector<osg::Vec3d> vecTemp;
	outRect(vecCoord, vecTemp);

	
	//IDrawer->BaseDrawLineLoopOperate(vecTemp, osg::Vec4(0, 0, 1, 1), 2.5, mapperGroup, worldMatrix);
	getTrianglePointsInScope(mRoot->getChild(0)->asGroup()->getChild(0), vecTemp, "");
	
	//getTrianglePointsInScope(vecAllPoints, vecTemp);
	double height = vecTemp[0].z();
	ifstream in;
	char buffer[256];
	double intarr[6];
	std::vector<nbsp::pointXYZ> vecPts;
	in.open(mEXEPath+"\\PCL_BIN\\cvFitPlane\\fileCache\\lines.txt");
	if (in.is_open())
	{
		while (!in.eof())
		{
			in.getline(buffer, 256);
			if (in.fail())
				break;
			istringstream iss(buffer);
			iss >> intarr[0] >> intarr[1] >> intarr[2] >> intarr[3] >> intarr[4] >> intarr[5];
			double x = (height - intarr[2]) / (intarr[5] - intarr[2]) * (intarr[3] - intarr[0]) + intarr[0];
			double y = (height - intarr[2]) / (intarr[5] - intarr[2]) * (intarr[4] - intarr[1]) + intarr[1];
			vecPts.push_back(nbsp::pointXYZ(x, y, height));
			//IDrawer->BaseDrawPtOperate(osg::Vec3d(intarr[0], intarr[1], intarr[2]), osg::Vec4(0, 0, 1, 1), 3.5, mapperGroup, worldMatrix);
		}
	}
	in.close();
	std::vector<osg::Vec3d> vecpts;
	if (vecPts.size() > 0)
	{
		std::vector<nbsp::pointXYZ> vecPoly;
		nbsp::CNbspAlgorithm ca;
		ca.computePoly(vecPts, vecPoly);

		for (auto pt : vecPoly)
		{
			vecpts.push_back(osg::Vec3d(pt.x, pt.y, pt.z));
			//IDrawer->BaseDrawPtOperate(osg::Vec3d(pt.x, pt.y, pt.z), osg::Vec4(0, 0, 1, 1), 5.5, mapperGroup, worldMatrix);
		}
		
		polygonCornerFilter(vecpts);
		IDrawer->BaseDrawLineLoopOperate(vecpts, osg::Vec4(1, 0, 0, 1), 3.0, mapperGroup, worldMatrix);

	}

	iSigEventCallBack->EmitSigShowOpsHint("完成");

	//mapperGroup->addChild(outlineGroup.get());
	double err = caculateError(vecCoord, vecpts);
	char msg[100];
	return true;
}



void COsgExtractTriangleBaseOutLineEvent::getTrianglePointsInScope(osg::ref_ptr<osg::Node> sceneNode, std::vector<osg::Vec3d>& vecWorldCoord, std::string id, double height)
{

	std::vector<CScope> vecClipScope;
	CScope scope;
	rectExtend(vecWorldCoord, -0.0001);
	scope.vecCoord = vecWorldCoord;
	std::vector<CL::Vec2d> pol;
	for (auto pt : vecWorldCoord)
	{
		pol.push_back(CL::Vec2d(pt.x(), pt.y()));
	}
	pol.push_back(CL::Vec2d(vecWorldCoord[0].x(), vecWorldCoord[0].y()));
	vecClipScope.push_back(scope);

	osg::ref_ptr<osg::MatrixTransform> accurateNode = new osg::MatrixTransform;
	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	InterfaceCOsgExtractObjectFactory iOsgExtractObjectFactory;
	InterfaceCOsgExtractObject* iOsgExtractObject = iOsgExtractObjectFactory.create();
	bool isSuccess = iOsgExtractObject->AccurateClipBottomScene(sceneNode, vecClipScope, trans,false);
	
	if (!isSuccess)
	{
		return;
	}

	osg::Matrix transMatrix = trans->getMatrix();

	InterfaceOsgFinderFactory IOsgFinderFactory;
	InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();
	std::vector<osg::Geode*> vecGeode = IOsgFinder->FindAndGetGeode(trans);

	ofstream outFile;
	outFile.open(mEXEPath + "\\pcd.txt");

	for (size_t i = 0; i < vecGeode.size(); ++i)
	{
		if (!vecGeode[i])
		{
			continue;
		}

		int num = vecGeode[i]->getNumDrawables();

		for (int j = num - 1; j >= 0; --j)
		{
			osg::Geometry* geom = dynamic_cast<osg::Geometry*>(vecGeode[i]->getDrawable(j));

			if (!geom)
				continue; //得到可绘制结点的顶点序列 

			osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
			/*for (int ii = 0;ii<verts->size();++ii)
			{
			double x1 = verts->at(ii).x();
			double y1 = verts->at(ii).y();
			double z1 = verts->at(ii).z();
			osg::Vec3d pt1(x1, y1, z1);
			pt1 = pt1 * transMatrix;
			if(CL::Point_In_Polygon_2D(x1,y1,pol))
			outFile << pt1.x() << " " << pt1.y() << " " << pt1.z() << std::endl;
			}*/
			//std::vector<size_t> vecIndex;
			for (size_t k = 0; k < geom->getNumPrimitiveSets(); k++)
			{
				osg::PrimitiveSet* priSet = geom->getPrimitiveSet(k);
				osg::ref_ptr<osg::DrawElements> drawElementTriangle = priSet->getDrawElements();
				osg::ref_ptr<osg::DrawElementsUInt> newDrawElementsTriangle = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
				int cnt = drawElementTriangle->getNumIndices();
				int triCnt = cnt / 3;

				for (int w = 0; w < triCnt; w++)
				{
					int index1 = drawElementTriangle->index(w * 3);
					int index2 = drawElementTriangle->index(w * 3 + 1);
					int index3 = drawElementTriangle->index(w * 3 + 2);
					/*vecIndex.push_back(index1);
					vecIndex.push_back(index2);
					vecIndex.push_back(index3);*/
					double x1 = verts->at(index1).x();
					double y1 = verts->at(index1).y();
					double z1 = verts->at(index1).z();
					osg::Vec3d pt1(x1, y1, z1);
					pt1 = pt1 * transMatrix;

					double x2 = verts->at(index2).x();
					double y2 = verts->at(index2).y();
					double z2 = verts->at(index2).z();
					osg::Vec3d pt2(x2, y2, z2);
					pt2 = pt2 * transMatrix;

					double x3 = verts->at(index3).x();
					double y3 = verts->at(index3).y();
					double z3 = verts->at(index3).z();
					osg::Vec3d pt3(x3, y3, z3);
					pt3 = pt3 * transMatrix;

					osg::Vec3d p1 = pt1;
					if (CL::Point_In_Polygon_2D(p1.x(), p1.y(), pol))
						outFile << p1.x() << " " << p1.y() << " " << p1.z() << std::endl;
					//outFile << p1.x() << " " << p1.y() << " " << p1.z() << std::endl;
					osg::Vec3d p2 = pt2;
					if (CL::Point_In_Polygon_2D(p2.x(), p2.y(), pol))
						outFile << p2.x() << " " << p2.y() << " " << p2.z() << std::endl;
					//outFile << p2.x() << " " << p2.y() << " " << p2.z() << std::endl;

					osg::Vec3d p3 = pt3;
					if (CL::Point_In_Polygon_2D(p3.x(), p3.y(), pol))
						outFile << p3.x() << " " << p3.y() << " " << p3.z() << std::endl;
					//outFile << p3.x() << " " << p3.y() << " " << p3.z() << std::endl;
				}
			}
			/*sort(vecIndex.begin(), vecIndex.end());
			vecIndex.erase(unique(vecIndex.begin(), vecIndex.end()), vecIndex.end());
			for (size_t i = 0; i<vecIndex.size(); ++i)
			{
				double x1 = verts->at(vecIndex[i]).x();
				double y1 = verts->at(vecIndex[i]).y();
				double z1 = verts->at(vecIndex[i]).z();
				osg::Vec3d pt1(x1, y1, z1);
				pt1 = pt1 * transMatrix;
				outFile << pt1.x() << " " << pt1.y() << " " << pt1.z() << std::endl;
			}*/
		}
	}
	outFile.close();
	wchar_t *buffer, *buffer1;
	std::string str;
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = _T("open");
	str = mEXEPath + "\\PCL_BIN\\pcdConverter.exe";
	size_t size = str.length();
	buffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), size, buffer, size * sizeof(wchar_t));
	buffer[size] = 0;  //确保以 '\0' 结尾 
	ShExecInfo.lpFile = buffer;

	str = mEXEPath + "\\pcd.txt " + mEXEPath + "\\house.pcd";
	size = str.length();
	buffer1 = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), size, buffer1, size * sizeof(wchar_t));
	buffer1[size] = 0;  //确保以 '\0' 结尾 
	ShExecInfo.lpParameters = buffer1;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	delete buffer;
	delete buffer1;


	SHELLEXECUTEINFO ShExecInfo1 = { 0 };
	ShExecInfo1.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo1.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo1.hwnd = NULL;
	ShExecInfo1.lpVerb = _T("open");
	str = mEXEPath + "\\PCL_BIN\\cloudSegmention.exe";
	size = str.length();
	buffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), size, buffer, size * sizeof(wchar_t));
	buffer[size] = 0;  //确保以 '\0' 结尾 
	ShExecInfo1.lpFile = buffer;
	str = mEXEPath + "\\house.pcd";
	size = str.length();
	buffer1 = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), size, buffer1, size * sizeof(wchar_t));
	buffer1[size] = 0;  //确保以 '\0' 结尾 
	ShExecInfo1.lpParameters = buffer1;
	ShExecInfo1.lpDirectory = NULL;
	ShExecInfo1.nShow = SW_HIDE;
	ShExecInfo1.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo1);
	WaitForSingleObject(ShExecInfo1.hProcess, INFINITE);
	delete buffer;
	delete buffer1;
	return;
}
void COsgExtractTriangleBaseOutLineEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}

	bool isActivated1 = isActivate(vecOpState, OPS_EXTRACT_TRIANGLE_OUTLINE);
	bool isActivated2 = isActivate(vecOpState, OPS_EXTRACT_TRIANGLE_OUTLINE_CHANGE_HEIGHT);
	if (isActivated1 || isActivated2)
	{
		isActivated = true;
	}
	else
		isActivated = false;

	if (isActivated2)
	{

	}
	if (isActivated == false && flag == true)
	{
		if (conn != NULL)
		{
			PQfinish(conn);
			conn = NULL;
		}
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{

		char strMsg[99];
		sprintf(strMsg, "无范围提取：按Ctrl + 左键选点，自动提取轮廓线，按ESC退出事件");
		iSigEventCallBack->EmitSigShowOpsHint(strMsg);

		CPGController pgControl;
		conn = pgControl.connectToDB();

		//获得exe路径
		char szapipath[MAX_PATH];
		memset(szapipath, 0, MAX_PATH);
		GetModuleFileNameA(NULL, szapipath, MAX_PATH);
		mEXEPath = string(szapipath);
		int pos = mEXEPath.rfind('\\');
		mEXEPath = mEXEPath.substr(0, pos);
		/*std::vector<osg::Vec3d> vec;
		fitLineLikeWater(mRoot, osg::Vec3d(-3.657732, 36.53576, -19.40049), osg::Vec3d(2.90793, 37.276639, -19.40049), 10, 0.01, vec);
		InterfaceBaseDrawerFactory IDrawerFactory;
		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
		for (int i = 0;i<vec.size();++i)
		{
			IDrawer->BaseDrawPtOperate(vec[i], osg::Vec4(1, 0, 0, 1), 2.5, mapperGroup, worldMatrix);
		}
		IDrawer->BaseDrawLineOperate(osg::Vec3d(-3.657732,37.208921, -19.40049), osg::Vec3d(2.90793,37.926842, -19.40049), osg::Vec4(0, 0, 1, 1), 3, mapperGroup, worldMatrix);*/
	}
}


bool COsgExtractTriangleBaseOutLineEvent::extractContour(double high)//提取等高线
{
	char strMsg[MAX_PATH];
	sprintf_s(strMsg, "正在提取等高线...    高度：%f米", high);
	iSigEventCallBack->EmitSigShowOpsHint(strMsg);
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);

	std::vector<std::string> vecChildFileName;
	IFinder->FindLodBottomLevelFile(sceneNode, vecChildFileName);

	if (vecChildFileName.size() == 0)
	{
		return false;
	}
	std::vector<osg::MatrixTransform*> vecTrans;
	IFinder->FindTransNode(sceneNode, vecTrans);
	if (vecTrans.size() != 1)
	{
		return false;
	}
	worldMatrix = vecTrans[0]->getMatrix();
	std::vector<osg::Vec3d> vecLine;
	for (int i = 0; i < vecChildFileName.size(); ++i)
	{
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(vecChildFileName[i]);

		if (!node)
		{
			continue;
		}

		InterfaceOsgFinderFactory iOsgFinderFactory;
		InterfaceOsgFinder* iOsgFinder = iOsgFinderFactory.create();
		std::vector<osg::Geode*> vecPtr = iOsgFinder->FindAndGetGeode(node);

		for (size_t i = 0; i< vecPtr.size(); ++i)
		{
			outline_3::computeOutline(vecPtr[i], high, worldMatrix, vecLine);
		}

	}

	if (vecLine.size() > 2)
	{
		InterfaceBaseDrawerFactory IDrawerFactory;
		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
		osg::ref_ptr<osg::Group> outlineGroup = new osg::Group;
		osg::Vec4d color(1.0, 0.0, 0.0, 1.0);
		double size = 1.5;
		for (int i = 0; i < vecLine.size(); i += 2)
		{
			IDrawer->BaseDrawLineOperate(vecLine[i], vecLine[i + 1], color, size, outlineGroup, worldMatrix);
		}

		mapperGroup->addChild(outlineGroup);
	}
	iSigEventCallBack->EmitSigShowOpsHint("");
	return true;
}

