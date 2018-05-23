#pragma once
#include "OSG_Plus.h"
#include <vector>
#include "publicDefined.h"
#include "InterfaceUserEventCallBack.h"

class CSceneExtractWall
{
public:
	
	bool getTriangleSelect(std::vector<std::string>& vecChildFileName, osg::Vec4& color, osg::Matrix worldMatrix, osg::Vec3d worldPt, std::vector<osg::Vec3d> &vecIndex, std::vector<osg::Vec3d> &vecTriangle);			//获取选择三角形

	bool getTrianglePoints(osg::Geode* geode, osg::Matrix matrix, std::vector<osg::Vec3d> &vec, std::vector<osg::Vec3d> &vecIndex, osg::Vec4& color, double colorErr, osg::Vec3d normal, float ANGLE);				//获得三角面片顶点

	bool getTrianglePointsEx(osg::Geode* geode, osg::Matrix matrix, std::vector<osg::Vec3d> &vec, std::vector<osg::Vec3d> &vecIndex, std::vector<osg::Vec4>& vecColor, double colorErr, std::vector<osg::Vec3d> & normal, float ANGLE);				//获得三角面片顶点,增加历史特征判断

	int indexInTriangle(osg::Vec3d& pt, std::vector<osg::Vec3d>& vecPt);														//序号在三角形中

	bool isSamePt(osg::Vec3d& psrc, osg::Vec3d& pdst)//判断是否同一点
	{
		return (psrc.x() == pdst.x()) && (psrc.y() == pdst.y()) && (psrc.z() == pdst.z());
	}

	void NormalVector(osg::Vec3d &a, osg::Vec3d &b, osg::Vec3d &c, osg::Vec3d &nomal);											//法向量计算

	bool checkNomal(osg::Vec3d nom, osg::Vec3d normal, double m_normal, double err);											//检验法向量与给定法向量是否在允许偏差内

	bool checkNomalEx(osg::Vec3d nom, std::vector<osg::Vec3d>& vecNormal, double err);                                          //依次检验历史特征法向量

	bool PtInTriangle(osg::Vec3d pt, osg::Vec3d& t1, osg::Vec3d& t2, osg::Vec3d& t3);											//判断点在三角面片内		

	bool triangleDrawed(osg::Vec3d& a, osg::Vec3d& b, osg::Vec3d& c, std::vector<osg::Vec3d> &vec);								//三角是否重复搜索

	int FindMinMax(std::vector<osg::Vec3d>& vec, osg::Vec3d& ptMinXY, osg::Vec3d& ptMaxXY, double& MinZ, double& MaxZ, double& MinX, double& MaxX, double& MinY, double& MaxY);

	void getPointProject(osg::Vec3d& pt, osg::Vec3d& ptPrj, PLane& pl);

	double getDisFromPointToPlane(osg::Vec3d& pt, PLane& plane);

	void getCoordBy2Point(osg::Vec3d& pt1, osg::Vec3d& pt3, std::vector<osg::Vec3d>& coord, PLane& plane);

	void redrawWall(osg::Vec3d& pt, osg::Vec3d& pt1,std::vector<osg::Vec3d>&vecCoord, WallInfo& wallSelected, osg::Vec4d color, osg::Group* group, osg::Group* tempGroup,osg::Matrix matrix);

	bool drawIntersectOfPlane(osg::Group* mapperGroup, osg::Group* tempGroup, std::vector<WallInfo> &planeSelect, osg::Vec4d& color, osg::Matrix& worldMatrix, InterfaceSigEventCallBack* iSigEventCallBack);

	bool isLineInMapperGroup(osg::Vec3d& p1, osg::Vec3d& p2, osg::Group* MapperGroup);

	void clearGroupResult(osg::Group* group);

	bool testColor(osg::Vec4& color1, osg::Vec4& color2, double err);

	bool testColorEx(osg::Vec4& color1, std::vector<osg::Vec4>& vecColor, double err);

	void getTextureCoordBy3Points(osg::Vec2d& pt1, osg::Vec2d& pt2, osg::Vec2d& pt3, std::vector<osg::Vec2d> &vec);

	void RGB2HSV(osg::Vec4& rgb,osg::Vec4& hsv);
};