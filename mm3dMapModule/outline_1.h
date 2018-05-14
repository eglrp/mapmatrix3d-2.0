#include <osgViewer/Viewer>
#include <osg/Group>


#ifdef DLL_OUTLINE_APILIBDLL  
#define DLL_OUTLINE_API _declspec(dllexport)  
#else  
#define DLL_OUTLINE_API  _declspec(dllimport)  
#endif 


namespace outline
{
	osg::StateSet* makePtState(int size);

	osg::Geode* drawLine(osg::Vec3d p1,osg::Vec3d p2, double r, double g, double b, double a, double size);
	osg::Geode* drawPoint(osg::Vec3d p, double r, double g, double b, double a, double size);

	bool testIntersection(osg::Vec3d &p1,osg::Vec3d &p2,osg::Group *mRoot,osg::Group *mMeasureResultGroup);

	void MoveUP(osg::Vec3d &p,osg::Group *mRoot,int s);//下移
	void MoveForward(osg::Vec3d &p,osg::Group *root,int s);//前进
	void MoveBack(osg::Vec3d &p1,osg::Vec3d &p2,osg::Group *mRoot,int s);
	bool drawSection(osg::Vec3d &p1,osg::Vec3d &p2,osg::Group *mRoot,int s);//画线段交点

	inline double distance2(osg::Vec3d p1,osg::Vec3d p2)
	{
		return sqrt((p1.x()-p2.x())*(p1.x()-p2.x())+(p1.y()-p2.y())*(p1.y()-p2.y())+(p1.z()-p2.z())*(p1.z()-p2.z()));
	}

	bool isDown__;
	int crashCount;
	std::vector<osg::Vec3d> vecPoints;
	osg::Vec3d lastp;


	DLL_OUTLINE_API void ComputeOutline(osg::Vec3d pt,osg::Vec3d nm,osg::Group *mRoot,std::vector<osg::Vec3d>& vec);//计算并画出剖面线
}
