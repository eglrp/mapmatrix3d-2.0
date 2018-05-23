#pragma once
#include "MultiScopeHeadGeodeClipper.h"

class CMultiScopeAccurateHeadGeodeClippper : public CMultiScopeHeadGeodeClipper
{
public:
	CMultiScopeAccurateHeadGeodeClippper(std::vector<CScope> VecScope, osg::Matrix MATRIX, bool IsInside) : CMultiScopeHeadGeodeClipper(VecScope, MATRIX, IsInside)
	{
		vecScope = VecScope;
		matrix = MATRIX;
		isInside = IsInside;
	}

public:
	std::vector<osg::Vec3d> vecInsideVerts;

protected:
	virtual bool ApplyWithTriangle(osg::Geometry* geom);

	virtual bool ApplyWithGeometry(osg::Geometry* geom, osg::ref_ptr<osg::Vec2Array> textureArray, osg::Vec3Array* verts, osg::ref_ptr<osg::Image> image, std::vector<CScope> vecScope, osg::Matrix matrix, bool isInside);

	virtual void ApplyWithEdgeTriangle(std::vector<osg::Vec2d> vecIntersectPtTexture, osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec3d pt3, std::vector<CL::Vec2d> pol, osg::Matrix matrix, std::vector<osg::Vec3d> vecIntersectPt, osg::Vec3Array* verts, int index1, int index2, int index3, osg::ref_ptr<osg::DrawElementsUInt> newDrawElementsTriangle, osg::ref_ptr<osg::Vec2Array> textureArray);

	virtual bool ApplyWithInnerTriangle(osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec3d pt3, bool isInside);

	virtual bool GetTriangleIntersectPolygon(osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec3d pt3, std::vector<CL::Vec2d> pol, double minH, double maxH, osg::Vec2d texture1, osg::Vec2d texture2, osg::Vec2d texture3, std::vector<osg::Vec3d> &vecIntersectPt, std::vector<osg::Vec2d> &vecIntersectPtTexture);
	
	virtual double CalculateIntersectPtHeight(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double  y3);

	virtual void CalculateIntersectPtTextureXY(osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec3d intersectPt, osg::Vec2d texture1, osg::Vec2d texture2, osg::Vec2d &interTexture);

	virtual osg::ref_ptr<osg::Image> FindTextureImage(osg::ref_ptr<osg::Geometry> geom);

	virtual osg::ref_ptr<osg::Vec2Array> FindTextureArray(osg::ref_ptr<osg::Geometry> geom);

	virtual void createClipPolygon(std::vector<CScope> vecScope, std::vector<CL::Vec2d> &pol);

	virtual void ObtainWorldPt(osg::Vec3Array* verts, int index1, int index2, int index3, osg::Matrix matrix, std::vector<osg::Vec3d> &vecPt);

private:
	std::vector<CScope> vecScope;
	osg::Matrix matrix;
	bool isInside;
};

class CMultiScopeNotAccurateHeadGeodeClippper : public CMultiScopeAccurateHeadGeodeClippper
{
public:
	CMultiScopeNotAccurateHeadGeodeClippper(std::vector<CScope> VecScope, osg::Matrix MATRIX, bool IsInside) : CMultiScopeAccurateHeadGeodeClippper(VecScope, MATRIX, IsInside)
	{
		vecScope = VecScope;
		matrix = MATRIX;
		isInside = IsInside;
	}

public:
	std::vector<osg::Vec3d> vecInsideVerts;
	virtual void ApplyWithEdgeTriangle(std::vector<osg::Vec2d> vecIntersectPtTexture, osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec3d pt3, std::vector<CL::Vec2d> pol, osg::Matrix matrix, std::vector<osg::Vec3d> vecIntersectPt, osg::Vec3Array* verts, int index1, int index2, int index3, osg::ref_ptr<osg::DrawElementsUInt> newDrawElementsTriangle, osg::ref_ptr<osg::Vec2Array> textureArray)
	{
		return;
	}
private:
	std::vector<CScope> vecScope;
	osg::Matrix matrix;
	bool isInside;
};
