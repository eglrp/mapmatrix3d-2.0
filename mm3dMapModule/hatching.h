#include <osgViewer/Viewer>
#include <osg/Group>
#include <windows.h>
#include <math.h>

#ifdef DLL_HATCH_APILIBDLL  
#define DLL_HATCH_API _declspec(dllexport)  
#else  
#define DLL_HATCH_API  _declspec(dllimport)  
#endif 

int restart ;//记录循环打空次数

bool isDown;
double deltaX,deltaY;
osg::Matrix worldMatrix;

void MoveBack(osg::Vec3d &p1,osg::Vec3d &p2,osg::Group *mRoot,osg::Matrix transMatrix);
void MoveUp(osg::Vec3d &p,std::vector<osg::Vec3d>& vecLine,osg::Group *root);//上移
void MoveDown(osg::Vec3d &p,std::vector<osg::Vec3d>& vecLine,osg::Group *root,osg::Matrix transMatrix);//下移
void MoveForward(osg::Vec3d &p,double &dx,double &dy,std::vector<osg::Vec3d>& vecLine,osg::Group *root,osg::Matrix transMatrix);//前进
osg::Geode* drawLine(osg::Vec3d p1,osg::Vec3d p2, double r, double g, double b, double a, double size);
osg::Geode* drawPoint(osg::Vec3d p, double r, double g, double b, double a, double size);
osg::StateSet* makePtState(int size);


DLL_HATCH_API  void computeHatching(osg::Vec3d Pt1,osg::Vec3d Pt2,osg::Group *root,std::vector<osg::Vec3d>& vecLine, osg::Matrix transMatrix);//计算剖面线，输出点集