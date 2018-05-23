#pragma once
#include "OSG_Plus.h"
#include <map>

#ifdef ALPHALIBDLL  
#define ALPHAAPI _declspec(dllexport)  
#else  
#define ALPHAAPI  _declspec(dllimport)  
#endif   


#define EPS 0.00001
typedef std::map<int ,std::vector<osg::Vec3d> >   MY_MAP;

class ALPHAAPI CAlphaShapes
{
public:
	bool doCheck(osg::Vec3d& p1,osg::Vec3d& p2,double alpha,std::vector<osg::Vec3d>& vecPoints);//Alpha Shapes检测 

	void dataOrganization(std::vector<osg::Vec3d> vecPointsIn, std::vector<osg::Vec3d>& vecPointsOut,double a);//将点云数据进行格网组织删减

	void extractPtCloudEdge(std::vector<osg::Vec3d>&vecWorldCoord, std::vector<osg::Vec3d>&vecEdge, double alpha);
protected:
	double distance_2Point_2D(double x1,double y1,double x2,double y2);

	inline double distance2(osg::Vec3d& p1, osg::Vec3d& p2)
	{
		return sqrt((p1.x() - p2.x())*(p1.x() - p2.x()) + (p1.y() - p2.y())*(p1.y() - p2.y()) + (p1.z() - p2.z())*(p1.z() - p2.z()));
	}
};