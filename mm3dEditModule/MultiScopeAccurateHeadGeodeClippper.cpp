#include "stdafx.h"
#include "MultiScopeAccurateHeadGeodeClippper.h"
#include <osg/Geometry>

bool CMultiScopeAccurateHeadGeodeClippper::ApplyWithTriangle(osg::Geometry* geom)
{
	bool isEmpty = true;

	osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
	//查找纹理
	osg::ref_ptr<osg::Vec2Array> textureArray = FindTextureArray(geom);
	osg::ref_ptr<osg::Image> image = FindTextureImage(geom);

	if (image == NULL || textureArray == NULL)
	{
		return isEmpty;
	}

	isEmpty = ApplyWithGeometry(geom, textureArray, verts, image, vecScope, matrix, isInside);

	return isEmpty;
}

bool CMultiScopeAccurateHeadGeodeClippper::ApplyWithGeometry(osg::Geometry* geom, osg::ref_ptr<osg::Vec2Array> textureArray, osg::Vec3Array* verts, osg::ref_ptr<osg::Image> image, std::vector<CScope> vecScope, osg::Matrix matrix, bool isInside)
{
	bool isEmpty = true;
	int imageWidth = image->s();
	int imageHeight = image->t();

	for (int k = 0; k < geom->getNumPrimitiveSets(); k++)
	{
		osg::PrimitiveSet* priSet = geom->getPrimitiveSet(k);
		osg::ref_ptr<osg::DrawElements> drawElementTriangle = priSet->getDrawElements();
		osg::ref_ptr<osg::DrawElementsUInt> newDrawElementsTriangle = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);

		int cnt = drawElementTriangle->getNumIndices();
		int triCnt = cnt / 3;

		for (int w = 0; w < triCnt; w++)
		{
			int index1 = drawElementTriangle->index(w * 3);
			int index2 = drawElementTriangle->index(w * 3 + 1);
			int index3 = drawElementTriangle->index(w * 3 + 2);
			std::vector<osg::Vec3d> vecWorldPt;
			ObtainWorldPt(verts, index1, index2, index3, matrix, vecWorldPt);
			osg::Vec3d pt1 = vecWorldPt[0];
			osg::Vec3d pt2 = vecWorldPt[1];
			osg::Vec3d pt3 = vecWorldPt[2];

			//创建范围
			std::vector<CL::Vec2d> pol;
			createClipPolygon(vecScope, pol);
			double minH = vecScope[0].minH;
			double maxH = vecScope[0].maxH;

			std::vector<osg::Vec2d> vecIntersectPtTexture;
			std::vector<osg::Vec3d> vecIntersectPt;

			osg::Vec2d texture1, texture2, texture3;
			texture1.x() = textureArray->at(index1).x() * imageWidth;
			texture1.y() = textureArray->at(index1).y() * imageHeight;
			texture2.x() = textureArray->at(index2).x() * imageWidth;
			texture2.y() = textureArray->at(index2).y() * imageHeight;
			texture3.x() = textureArray->at(index3).x() * imageWidth;
			texture3.y() = textureArray->at(index3).y() * imageHeight;

			bool isCorner = GetTriangleIntersectPolygon(pt1, pt2, pt3, pol, minH, maxH, texture1, texture2, texture3, vecIntersectPt, vecIntersectPtTexture);

			if (vecIntersectPt.size() == 2 && isCorner == false)
			{
				vecIntersectPtTexture[0].x() = vecIntersectPtTexture[0].x() / imageWidth;
				vecIntersectPtTexture[0].y() = vecIntersectPtTexture[0].y() / imageHeight;

				vecIntersectPtTexture[1].x() = vecIntersectPtTexture[1].x() / imageWidth;
				vecIntersectPtTexture[1].y() = vecIntersectPtTexture[1].y() / imageHeight;

				ApplyWithEdgeTriangle(vecIntersectPtTexture, pt1, pt2, pt3, pol, matrix, vecIntersectPt, verts, index1, index2, index3, newDrawElementsTriangle, textureArray);
				isEmpty = false;

				vecInsideVerts.push_back(vecIntersectPt[0]);
				vecIntersectPt.push_back(vecIntersectPt[1]);
			}
			else if (vecIntersectPt.size() > 0 || isCorner == true)
			{
				//认为是边角位置的三角形
				newDrawElementsTriangle->push_back(index1);
				newDrawElementsTriangle->push_back(index2);
				newDrawElementsTriangle->push_back(index3);

				isEmpty = false;
			}
			else 
			{
				if(ApplyWithInnerTriangle(pt1, pt2, pt3, isInside))
				{
					newDrawElementsTriangle->push_back(index1);
					newDrawElementsTriangle->push_back(index2);
					newDrawElementsTriangle->push_back(index3);
					isEmpty = false;
				}
			}
			
		}

		geom->setPrimitiveSet(k, newDrawElementsTriangle);
	}

	return isEmpty;
}

bool CMultiScopeAccurateHeadGeodeClippper::ApplyWithInnerTriangle(osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec3d pt3, bool isInside)
{
	bool isInPolygon = false;
	CVertsTriangle vertsTri;
	vertsTri.pt1 = pt1;
	vertsTri.pt2 = pt2;
	vertsTri.pt3 = pt3;

	if (isInAreaJudger.isTriangleInScope(vertsTri))
	{
		isInPolygon = true;
	}

	if (isInPolygon ^ isInside)
	{
		return false;
	}
	else
	{
		return true;
	}
	
}

void CMultiScopeAccurateHeadGeodeClippper::ApplyWithEdgeTriangle(std::vector<osg::Vec2d> vecIntersectPtTexture, osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec3d pt3, 
	std::vector<CL::Vec2d> pol, osg::Matrix matrix, std::vector<osg::Vec3d> vecIntersectPt, osg::Vec3Array* verts, int index1, int index2, int index3, 
	osg::ref_ptr<osg::DrawElementsUInt> newDrawElementsTriangle, osg::ref_ptr<osg::Vec2Array> textureArray)
{
	osg::Matrix iMatrix = matrix.inverse(matrix);
	osg::Vec3d ori1 = vecIntersectPt[0] * iMatrix;
	osg::Vec3d ori2 = vecIntersectPt[1] * iMatrix;

	//插入坐标
	verts->push_back(ori1);
	verts->push_back(ori2);

	textureArray->push_back(vecIntersectPtTexture[0]);
	textureArray->push_back(vecIntersectPtTexture[1]);

	std::vector<int> vecInsidePtIndex;
	int numIndex = verts->size();
	bool isIndex1 = false;
	bool isIndex2 = false;
	bool isIndex3 = false;

	if (!(CL::Point_In_Polygon_2D(pt1.x(), pt1.y(), pol) ^ isInside))
	{
		vecInsidePtIndex.push_back(index1);
		isIndex1 = true;
	}

	if (!(CL::Point_In_Polygon_2D(pt2.x(), pt2.y(), pol) ^ isInside))
	{
		vecInsidePtIndex.push_back(index2);
		isIndex2 = true;
	}

	if (!(CL::Point_In_Polygon_2D(pt3.x(), pt3.y(), pol) ^ isInside))
	{
		vecInsidePtIndex.push_back(index3);
		isIndex3 = true;
	}

	if (vecInsidePtIndex.size() == 1)
	{
		newDrawElementsTriangle->push_back(vecInsidePtIndex[0]);
		newDrawElementsTriangle->push_back(numIndex - 2);
		newDrawElementsTriangle->push_back(numIndex - 1);
	}
	else if (vecInsidePtIndex.size() == 2)											//两个顶点在里面
	{
		if (isIndex1 == false)
		{
			newDrawElementsTriangle->push_back(vecInsidePtIndex[0]);
			newDrawElementsTriangle->push_back(vecInsidePtIndex[1]);
			newDrawElementsTriangle->push_back(numIndex - 1);

			newDrawElementsTriangle->push_back(vecInsidePtIndex[0]);
			newDrawElementsTriangle->push_back(numIndex - 2);
			newDrawElementsTriangle->push_back(numIndex - 1);
		}
		else if (isIndex2 == false)
		{
			newDrawElementsTriangle->push_back(vecInsidePtIndex[0]);
			newDrawElementsTriangle->push_back(vecInsidePtIndex[1]);
			newDrawElementsTriangle->push_back(numIndex - 1);

			newDrawElementsTriangle->push_back(vecInsidePtIndex[0]);
			newDrawElementsTriangle->push_back(numIndex - 2);
			newDrawElementsTriangle->push_back(numIndex - 1);
		}
		else if (isIndex3 == false)
		{
			newDrawElementsTriangle->push_back(vecInsidePtIndex[0]);
			newDrawElementsTriangle->push_back(vecInsidePtIndex[1]);
			newDrawElementsTriangle->push_back(numIndex - 1);

			newDrawElementsTriangle->push_back(vecInsidePtIndex[1]);
			newDrawElementsTriangle->push_back(numIndex - 2);
			newDrawElementsTriangle->push_back(numIndex - 1);
		}
	}
}

void CMultiScopeAccurateHeadGeodeClippper::ObtainWorldPt(osg::Vec3Array* verts, int index1, int index2, int index3, osg::Matrix matrix, std::vector<osg::Vec3d> &vecPt)
{
	double x1 = verts->at(index1).x();
	double y1 = verts->at(index1).y();
	double z1 = verts->at(index1).z();
	osg::Vec3d pt1(x1, y1, z1);
	pt1 = pt1 * matrix;

	double x2 = verts->at(index2).x();
	double y2 = verts->at(index2).y();
	double z2 = verts->at(index2).z();
	osg::Vec3d pt2(x2, y2, z2);
	pt2 = pt2 * matrix;

	double x3 = verts->at(index3).x();
	double y3 = verts->at(index3).y();
	double z3 = verts->at(index3).z();
	osg::Vec3d pt3(x3, y3, z3);
	pt3 = pt3 * matrix;

	vecPt.push_back(pt1);
	vecPt.push_back(pt2);
	vecPt.push_back(pt3);
}

void CMultiScopeAccurateHeadGeodeClippper::createClipPolygon(std::vector<CScope> vecScope, std::vector<CL::Vec2d> &pol)
{
	for (int t = 0; t < vecScope[0].vecCoord.size(); t++)
	{
		double x = vecScope[0].vecCoord[t].x();
		double y = vecScope[0].vecCoord[t].y();
		double z = vecScope[0].vecCoord[t].z();
		pol.push_back(CL::Vec2d(x, y));
	}

	double x = vecScope[0].vecCoord[0].x();
	double y = vecScope[0].vecCoord[0].y();
	double z = vecScope[0].vecCoord[0].z();
	pol.push_back(CL::Vec2d(x, y));
}

osg::ref_ptr<osg::Image> CMultiScopeAccurateHeadGeodeClippper::FindTextureImage(osg::ref_ptr<osg::Geometry> geom)
{
	osg::ref_ptr<osg::Texture2D> texture = dynamic_cast<osg::Texture2D*> (geom->getOrCreateStateSet()->getTextureAttribute(0, osg::StateAttribute::TEXTURE));

	if (!texture)
	{
		return NULL;
	}

	osg::ref_ptr<osg::Image> image = texture->getImage(0);
	return image.get();
}

osg::ref_ptr<osg::Vec2Array> CMultiScopeAccurateHeadGeodeClippper::FindTextureArray(osg::ref_ptr<osg::Geometry> geom)
{
	osg::ref_ptr<osg::Vec2Array> textureArray = dynamic_cast<osg::Vec2Array*> (geom->getTexCoordArray(0));

	if (!textureArray)
	{
		return NULL;
	}

	return textureArray.get();
}

bool CMultiScopeAccurateHeadGeodeClippper::GetTriangleIntersectPolygon(osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec3d pt3, std::vector<CL::Vec2d> pol, double minH, double maxH, osg::Vec2d texture1, osg::Vec2d texture2, osg::Vec2d texture3, std::vector<osg::Vec3d> &vecIntersectPt, std::vector<osg::Vec2d> &vecIntersectPtTexture)
{
	int intersectPolNum = 0;

	osg::Matrix iMatrix = matrix.inverse(matrix);

	for (int i = 0; i < pol.size() - 1; i++)
	{
		bool isIntersect = false;

		CL::Vec2d pt2D_1, pt2D_2, pt2D_3;
		pt2D_1.x = pt1.x();
		pt2D_1.y = pt1.y();

		pt2D_2.x = pt2.x();
		pt2D_2.y = pt2.y();

		pt2D_3.x = pt3.x();
		pt2D_3.y = pt3.y();

		if (CL::IsIntersect(pt2D_1.x, pt2D_1.y, pt2D_2.x, pt2D_2.y, pol[i].x, pol[i].y, pol[i + 1].x, pol[i + 1].y))
		{
			if ((minH == maxH) || (pt1.z() >= minH && pt1.z() <= maxH && pt2.z() >= minH && pt2.z() <= maxH))
			{
				CL::Vec2d intersetPt = CL::GetLineSegmentIntersection(pt2D_1, pt2D_2, pol[i], pol[i + 1]);

				double z = CalculateIntersectPtHeight(pt1.x(), pt1.y(), pt1.z(), pt2.x(), pt2.y(), pt2.z(), intersetPt.x, intersetPt.y);
				osg::Vec3d intersectPt3D(intersetPt.x, intersetPt.y, z);
				vecIntersectPt.push_back(intersectPt3D);

				//计算原始的坐标
				osg::Vec3d oriPt1 = pt1 * iMatrix;
				osg::Vec3d oriPt2 = pt2 * iMatrix;
				osg::Vec3d oriIntersectPt3D = intersectPt3D * iMatrix;

				//插值纹理坐标
				osg::Vec2d intersectTexture;
				CalculateIntersectPtTextureXY(oriPt1, oriPt2, oriIntersectPt3D, texture1, texture2, intersectTexture);
				vecIntersectPtTexture.push_back(intersectTexture);

				isIntersect = true;
			}
		}

		if (CL::IsIntersect(pt2D_2.x, pt2D_2.y, pt2D_3.x, pt2D_3.y, pol[i].x, pol[i].y, pol[i + 1].x, pol[i + 1].y))
		{
			if ((minH == maxH) || (pt2.z() >= minH && pt2.z() <= maxH && pt3.z() >= minH && pt3.z() <= maxH))
			{
				CL::Vec2d intersetPt = CL::GetLineSegmentIntersection(pt2D_2, pt2D_3, pol[i], pol[i + 1]);

				double z = CalculateIntersectPtHeight(pt2.x(), pt2.y(), pt2.z(), pt3.x(), pt3.y(), pt3.z(), intersetPt.x, intersetPt.y);
				osg::Vec3d intersectPt3D(intersetPt.x, intersetPt.y, z);
				vecIntersectPt.push_back(intersectPt3D);

				//计算原始的坐标
				osg::Vec3d oriPt1 = pt1 * iMatrix;
				osg::Vec3d oriPt2 = pt2 * iMatrix;
				osg::Vec3d oriIntersectPt3D = intersectPt3D * iMatrix;

				//插值纹理坐标
				osg::Vec2d intersectTexture;
				CalculateIntersectPtTextureXY(pt2, pt3, intersectPt3D, texture2, texture3, intersectTexture);
				vecIntersectPtTexture.push_back(intersectTexture);

				isIntersect = true;
			}
		}

		if (CL::IsIntersect(pt2D_1.x, pt2D_1.y, pt2D_3.x, pt2D_3.y, pol[i].x, pol[i].y, pol[i + 1].x, pol[i + 1].y))
		{
			if ((minH == maxH) || (pt1.z() >= minH && pt1.z() <= maxH && pt3.z() >= minH && pt3.z() <= maxH))
			{
				CL::Vec2d intersetPt = CL::GetLineSegmentIntersection(pt2D_1, pt2D_3, pol[i], pol[i + 1]);

				double z = CalculateIntersectPtHeight(pt1.x(), pt1.y(), pt1.z(), pt3.x(), pt3.y(), pt3.z(), intersetPt.x, intersetPt.y);
				osg::Vec3d intersectPt3D(intersetPt.x, intersetPt.y, z);
				vecIntersectPt.push_back(intersectPt3D);

				//计算原始的坐标
				osg::Vec3d oriPt1 = pt1 * iMatrix;
				osg::Vec3d oriPt2 = pt2 * iMatrix;
				osg::Vec3d oriIntersectPt3D = intersectPt3D * iMatrix;

				//插值纹理坐标
				osg::Vec2d intersectTexture;
				CalculateIntersectPtTextureXY(pt1, pt3, intersectPt3D, texture1, texture3, intersectTexture);
				vecIntersectPtTexture.push_back(intersectTexture);

				isIntersect = true;
			}
		}

		if (isIntersect)
		{
			intersectPolNum++;
		}

	}

	if (intersectPolNum >= 2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

double CMultiScopeAccurateHeadGeodeClippper::CalculateIntersectPtHeight(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double  y3)
{
	return (x3 - x1) / (x2 - x1) * (z2 - z1) + z1;
}


void CMultiScopeAccurateHeadGeodeClippper::CalculateIntersectPtTextureXY(osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec3d intersectPt, osg::Vec2d texture1, osg::Vec2d texture2, osg::Vec2d &interTexture)
{
	interTexture.x() = texture1.x() - (texture1.x() - texture2.x()) * (pt1.x() - intersectPt.x()) / (pt1.x() - pt2.x());
	interTexture.y() = texture1.y() - (texture1.y() - texture2.y()) * (pt1.y() - intersectPt.y()) / (pt1.y() - pt2.y());
}