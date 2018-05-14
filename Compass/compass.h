#include <osg/MatrixTransform>  
#include <osg/Projection>  
#include <osg/Vec4>  
#include <osg/Camera>  
#include <osg/NodeVisitor>  
#include <osg/ClipPlane>  
#include <osg/Viewport>  
#include <osgViewer/Viewer>
 
#ifndef DLL_API
	#define DLL_API __declspec(dllexport)
#endif

class Compass : public osg::Camera
{
public:
	Compass(){};
	Compass( const Compass& copy, osg::CopyOp
	copyop=osg::CopyOp::SHALLOW_COPY );
	META_Node( osg, Compass );
	void setPlate( osg::MatrixTransform* plate ) {
					_plateTransform = plate; }
	void setNeedle( osg::MatrixTransform* needle ) {
					_needleTransform = needle; }
	void setMainCamera( osg::Camera* camera ) {
					_mainCamera = camera; }
	virtual void traverse( osg::NodeVisitor& nv );
protected:
	virtual ~Compass(){};
	osg::ref_ptr<osg::MatrixTransform> _plateTransform;
	osg::ref_ptr<osg::MatrixTransform> _needleTransform;
	osg::observer_ptr<osg::Camera> _mainCamera;
};


osg::MatrixTransform* createCompassPart( const std::string&
image, float radius, float height );

DLL_API void createCompass(osgViewer::Viewer *mViewer, osg::ref_ptr<osg::Group> mRoot,int x,int y,int height,int width);
