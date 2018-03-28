#pragma once
#include <vector>
#include <osg/Node>
#include <osg/Group>


#ifdef EXTRACTTEXTURELIBDLL  
#define EXTRACTTEXTUREAPI _declspec(dllexport)  
#else  
#define EXTRACTTEXTUREAPI  _declspec(dllimport)  
#endif  



class EXTRACTTEXTUREAPI InterfaceOsgExtractTexture
{
public:
	virtual osg::ref_ptr<osg::Node> ExtractTexture(std::vector<osg::Vec3d> vecWorldPt, osg::ref_ptr<osg::Node> sceneNode, double res) = 0;

	virtual osg::ref_ptr<osg::Image> ExtractTextureImage(std::vector<osg::Vec3d> vecWorldPt, osg::ref_ptr<osg::Node> sceneNode, double res) = 0;
};

class EXTRACTTEXTUREAPI InterfaceOsgExtractTextureFactory
{
public:
	InterfaceOsgExtractTexture* create();
};