#pragma once
#include "OSG_Plus.h"
#include "InterfaceOsgFinder.h"

class COsgVertextExtract : public osg::NodeVisitor
{
public: //所有的顶点 
	osg::ref_ptr<osg::Vec3Array> extracted_verts; 
	osg::Matrix matrix;

	void ExtractVerts(osg::ref_ptr<osg::Node> node, osg::ref_ptr<osg::Vec3Array> &extracted_verts);

public:
	COsgVertextExtract();
	void apply(osg::Geode& geode);
	~COsgVertextExtract(void);
};

class COsgVertextReplace : public osg::NodeVisitor
{
public: 
	COsgVertextReplace(osg::ref_ptr<osgViewer::Viewer> viewer,osg::ref_ptr<osg::Node> scenceNode,  osg::Matrix matrix, float x, float y,
		float pixSize, float disErr) :osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
		_scenceNode = scenceNode;
		_viewer = viewer;
		_x = x;
		_y = y;
		_pixSize = pixSize;
		_disErr = disErr;
		_matrix = matrix;
	};
	
	osg::Matrix _matrix;

private:
	osg::Node* _scenceNode;
	osgViewer::Viewer* _viewer;
	float _x;
	float _y;
	float _pixSize;
	float _disErr;
public:
	
	void apply(osg::Geode& geode);
	void getNearestInsect(osg::ref_ptr<osg::Node> node, osg::Vec3d& worldPt, double radiu);
	float getAngle(osg::Vec3d p1,osg::Vec3d p2);
	~COsgVertextReplace(void) {};
};

