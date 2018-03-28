#include "StdAfx.h"
#include "qcomm.h"
#include <osg/LOD>
#include <osg/PagedLOD>
#include "my_matrix_compu.hpp"
#include "OsgVertexCompute.h"
#include "projection_matrix_transform.hpp"
#include "SevenParameter.h"

COsgVertexCompute::COsgVertexCompute(double *Rr, double *XYZ, double s) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
{
	for (int i = 0; i < 9; i++)
	{
		Rr_Save[i] = Rr[i];
	}

	for(int i = 0; i < 3; i++)
	{
		xyz[i] = XYZ[i];
	}

	scale = s;
}


COsgVertexCompute::~COsgVertexCompute(void)
{
}

void COsgVertexCompute::ComputeVertex(osg::ref_ptr<osg::Node> node, double *Rr, double *FWK, double s)
{
	COsgVertexCompute ive(Rr, FWK, s);
	node->accept(ive);
}

void COsgVertexCompute::apply(osg::PagedLOD& pagedLod)
{
	CSevenParameter sevenParamCompute;

	for (int i = 0; i < 9; i++)
	{
		sevenParamCompute.R_[i] = Rr_Save[i];
	}

	for (int i = 0; i < 3; i++)
	{
		sevenParamCompute.t_[i] = xyz[i];
	}

	sevenParamCompute.s_ = scale;
	osg::Vec3 center = pagedLod.getCenter();
	CoordDouble3D absCenter;
	sevenParamCompute.CalculateRelativeToAbsolute(CoordDouble3D(center.x(), center.y(), center.z()), absCenter);
	pagedLod.setCenter(osg::Vec3d(absCenter.x, absCenter.y, absCenter.z));
	center = pagedLod.getCenter();
	traverse(pagedLod);
}

void COsgVertexCompute::apply(osg::Geode& geode) 
{ 
	CSevenParameter sevenParamCompute;
	
	for (int i = 0; i < 9; i++)
	{
		sevenParamCompute.R_[i] = Rr_Save[i];
	}

	for (int i = 0; i < 3; i++)
	{
		sevenParamCompute.t_[i] = xyz[i];
	}

	sevenParamCompute.s_ = scale;
	//看看有多少可绘制结点 
	for(unsigned int i= 0; i < geode.getNumDrawables(); ++i) 
	{ 
		osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(i)); 

		if(!geom) 
			continue; //得到可绘制结点的顶点序列 
	
		osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray()); 

		if(!verts) 
			continue; //把顶点序列插入到顶点集中以便输出 

		//根据旋转矩阵计算
		for (int j = 0; j < verts->size(); j++)
		{
			double ptXYZ[3];
			double ptRotate[3];
			ptXYZ[0] = verts->at(j).x();
			ptXYZ[1] = verts->at(j).y();
			ptXYZ[2] = verts->at(j).z();
			CoordDouble3D relaXYZ(ptXYZ[0], ptXYZ[1], ptXYZ[2]);
			CoordDouble3D absXYZ;
			sevenParamCompute.CalculateRelativeToAbsolute(relaXYZ, absXYZ);

			verts->at(j).x() = absXYZ.x;
			verts->at(j).y() = absXYZ.y;
			verts->at(j).z() = absXYZ.z;
		}

	} 

}


