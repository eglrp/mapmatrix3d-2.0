#include "compass.h"
#include <osg/Texture2D>
#include <osg/Geode>
#include <osgDB/ReadFile>

Compass::Compass( const Compass& copy, osg::CopyOp copyop ):
	osg::Camera(copy, copyop),
	_plateTransform(copy._plateTransform),
	_needleTransform(copy._needleTransform),
	_mainCamera(copy._mainCamera)
	{

	}

void Compass::traverse( osg::NodeVisitor& nv )
{
	if ( _mainCamera.valid() &&
		nv.getVisitorType()==osg::NodeVisitor::CULL_VISITOR )
	{
		osg::Matrix matrix = _mainCamera->getViewMatrix();
		matrix.setTrans( osg::Vec3() );
		osg::Vec3 northVec = osg::Y_AXIS * matrix;
		northVec.z() = 0.0f;
		northVec.normalize();
		osg::Vec3 axis = osg::Y_AXIS ^ northVec;
		float angle = atan2(axis.length(), osg::Y_AXIS*northVec);
		axis.normalize();
		if ( _plateTransform.valid() )
			_plateTransform->setMatrix( osg::Matrix::rotate(
			angle, axis) );
	}
	_plateTransform->accept( nv );
	_needleTransform->accept( nv );
	osg::Camera::traverse( nv );
}


osg::MatrixTransform* createCompassPart( const std::string&
image, float radius, float height )
{
	osg::Vec3 center(-radius, -radius, height);
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(createTexturedQuadGeometry(center, osg::Vec3(radius * 2.0f, 0.0f, 0.0f), osg::Vec3(0.0f, radius * 2.0f, 0.0f)));
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setImage(osgDB::readImageFile(image));
	osg::ref_ptr<osg::MatrixTransform> part = new osg::MatrixTransform;
	part->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get());
	part->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	part->addChild(geode.get());
	return part.release();
}



DLL_API  void createCompass(osgViewer::Viewer *mViewer, osg::ref_ptr<osg::Group> mRoot,int x,int y,int height,int width)
{
	osg::ref_ptr<Compass> compass = new Compass;
	compass->setMainCamera(mViewer->getCamera());

	osg::ref_ptr<osg::Viewport> viewPort = mViewer->getCamera()->getViewport();

	compass->setViewport(viewPort.get());
	compass->setProjectionMatrix( osg::Matrixd::ortho(-100,5, -50, 5, -10.0, 10.0) );
	compass->setPlate( createCompassPart("c1.png", 1.8f, 0.0f) );
	compass->setNeedle( createCompassPart("c1.png", 0.0f, -1.0f) );

	compass->setRenderOrder(osg::Camera::POST_RENDER);
	compass->setClearMask(GL_DEPTH_BUFFER_BIT);
	compass->setAllowEventFocus( false );
	compass->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	compass->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	compass->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	mRoot->addChild(compass.get());	
}
