#include <osgViewer/Viewer>
#include <osg/Group>

#ifdef DLL_OUTLINE2_APILIBDLL  
#define DLL_OUTLINE_API_2 _declspec(dllexport)  
#else  
#define DLL_OUTLINE_API_2  _declspec(dllimport)  
#endif 


namespace outline_2
{
	inline double dist2(osg::Vec3d p1,osg::Vec3d p2)
	{
		return sqrt((p1.x()-p2.x())*(p1.x()-p2.x())+(p1.y()-p2.y())*(p1.y()-p2.y())+(p1.z()-p2.z())*(p1.z()-p2.z()));
	}
	double deltaX,deltaY;
	void cacuDeltaXY(osg::Vec3d pt1,osg::Vec3d pt2,double &deltaX,double &deltaY);
	DLL_OUTLINE_API_2 void ComputeOutline(std::vector<osg::Vec3d> Area,osg::Vec3d Pt,osg::Group *mRoot,std::vector<osg::Vec3d>& vec);
}