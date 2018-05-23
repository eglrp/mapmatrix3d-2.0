#include <osgViewer/Viewer>
#include <osg/Group>
#include "Geometry.h"

#ifdef DLL_OUTLINE3_APILIBDLL  
#define DLL_OUTLINE_API_3 _declspec(dllexport)  
#else  
#define DLL_OUTLINE_API_3  _declspec(dllimport)  
#endif 


namespace outline_3
{
	DLL_OUTLINE_API_3 void computeOutline(osg::Geode* geode,double H,osg::Matrix matrix,std::vector<osg::Vec3d>& vec);
	DLL_OUTLINE_API_3 void computeOutlineInArea(osg::Geode* geode, double H, osg::Matrix matrix, std::vector<osg::Vec3d>& vec, std::vector<osg::Vec3d>& area);
	DLL_OUTLINE_API_3 int computeOutlinebyPoint(std::vector<osg::Vec3d>& vecpt, double H, osg::Matrix matrix, std::vector<osg::Vec3d>& vec);

	DLL_OUTLINE_API_3 int computePoints(osg::Vec3d Pt1,osg::Vec3d Pt2,osg::Vec3d Pt3,double H,std::vector<osg::Vec3d>& vec);
	void computePointsInArea(osg::Vec3d Pt1, osg::Vec3d Pt2, osg::Vec3d Pt3, double H, std::vector<osg::Vec3d>& vec, std::vector<osg::Vec3d>& area);
	inline osg::Vec3d getInsection(osg::Vec3d& p1,osg::Vec3d& p2,double H)
	{
		return osg::Vec3d((H - p1.z())*(p2.x() - p1.x())/(p2.z() - p1.z()) + p1.x(),(H - p1.z())*(p2.y() - p1.y())/(p2.z() - p1.z()) + p1.y(),H);
	}
}