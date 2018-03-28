#pragma once
#include "OSG_Plus.h"

class COsgVertexCompute : public osg::NodeVisitor
{
public:
	COsgVertexCompute(double *Rr, double *FWK, double s);
	~COsgVertexCompute(void);

	void ComputeVertex(osg::ref_ptr<osg::Node> node, double *Rr, double *FWK, double s);
	
protected:
	void apply(osg::Geode& geode);
	void apply(osg::PagedLOD& pagedLod);
public: 
	double Rr_Save[9];
	double xyz[3];
	double scale;

};

