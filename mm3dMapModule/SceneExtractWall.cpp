#include "StdAfx.h"
#include "SceneExtractWall.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceBaseDrawer.h"
#include "Geometry.h"

bool CSceneExtractWall::getTriangleSelect(std::vector<std::string>& vecChildFileName, osg::Vec4& color,osg::Matrix worldMatrix, osg::Vec3d worldPt, std::vector<osg::Vec3d> &vecIndex, std::vector<osg::Vec3d> &vecTriangle)//获取选择三角形
{
	for (int i = 0; i < vecChildFileName.size(); ++i)
	{
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(vecChildFileName[i]);

		if (!node)
		{
			continue;
		}

		InterfaceOsgFinderFactory IOsgFinderFactory;
		InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();
		std::vector<osg::Geode*> vecGeode = IOsgFinder->FindAndGetGeode(node);

		for (size_t i = 0; i < vecGeode.size(); ++i)
		{
			if (!vecGeode[i])
			{
				continue;
			}

			int num = vecGeode[i]->getNumDrawables();

			for (int j = num - 1; j >= 0; --j)
			{
				osg::Geometry* geom = dynamic_cast<osg::Geometry*>(vecGeode[i]->getDrawable(j));

				if (!geom)
					continue; //得到可绘制结点的顶点序列 

				osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

				for (size_t k = 0; k < geom->getNumPrimitiveSets(); k++)
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

						double x1 = verts->at(index1).x();
						double y1 = verts->at(index1).y();
						double z1 = verts->at(index1).z();
						osg::Vec3d pt1(x1, y1, z1);
						pt1 = pt1 * worldMatrix;

						double x2 = verts->at(index2).x();
						double y2 = verts->at(index2).y();
						double z2 = verts->at(index2).z();
						osg::Vec3d pt2(x2, y2, z2);
						pt2 = pt2 * worldMatrix;

						double x3 = verts->at(index3).x();
						double y3 = verts->at(index3).y();
						double z3 = verts->at(index3).z();
						osg::Vec3d pt3(x3, y3, z3);
						pt3 = pt3 * worldMatrix;

						if (PtInTriangle(worldPt, pt1, pt2, pt3))//找到鼠标点击的面片
						{
							vecIndex.clear();
							vecIndex.push_back(pt1);
							vecIndex.push_back(pt2);
							vecIndex.push_back(pt3);
							vecTriangle.clear();
							vecTriangle.push_back(pt1);
							vecTriangle.push_back(pt2);
							vecTriangle.push_back(pt3);
							//保存颜色
							osg::ref_ptr<osg::Texture2D> texture = dynamic_cast<osg::Texture2D*> (geom->getOrCreateStateSet()->getTextureAttribute(0,osg::StateAttribute::TEXTURE));
							osg::ref_ptr<osg::Vec2Array> textureArray = dynamic_cast<osg::Vec2Array*> (geom->getTexCoordArray(0));
							osg::ref_ptr<osg::Image> image = texture->getImage(0);
							int imageWidth = image->s();
							int imageHeight = image->t();
							
							osg::Vec2d texture1,texture2,texture3;
							texture1.x() = textureArray->at(index1).x() * imageWidth;
							texture1.y() = textureArray->at(index1).y() * imageHeight;
							texture2.x() = textureArray->at(index2).x() * imageWidth;
							texture2.y() = textureArray->at(index2).y() * imageHeight;
							texture3.x() = textureArray->at(index3).x() * imageWidth;
							texture3.y() = textureArray->at(index3).y() * imageHeight;
							std::vector<osg::Vec2d> vecCoord;
							getTextureCoordBy3Points(texture1,texture2,texture3,vecCoord);
							unsigned char* data = (unsigned char*)(image->getDataPointer());
							int R = 0, G = 0, B = 0;
							for (size_t i = 0;i<vecCoord.size();++i)
							{
								R += (int)data[((int)vecCoord[i].y()) * imageWidth * 3 + ((int)vecCoord[i].x()) * 3 + 0];
								G += (int)data[((int)vecCoord[i].y()) * imageWidth * 3 + ((int)vecCoord[i].x()) * 3 + 1];
								B += (int)data[((int)vecCoord[i].y()) * imageWidth * 3 + ((int)vecCoord[i].x()) * 3 + 2];
							}
							/*int r = data[((int)texture1.y()) * imageWidth * 3 + ((int)texture1.x()) * 3 + 0];
							int g = data[((int)texture1.y()) * imageWidth * 3 + ((int)texture1.x()) * 3 + 1];
							int b = data[((int)texture1.y()) * imageWidth * 3 + ((int)texture1.x()) * 3 + 2];*/
							color.r() = (R / 1.0) / vecCoord.size() / 255.0;
							color.g() = (G / 1.0) / vecCoord.size() / 255.0;
							color.b() = (B / 1.0) / vecCoord.size() / 255.0;
							color.a() = 1.0;
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}
bool CSceneExtractWall::getTrianglePoints(osg::Geode* geode, osg::Matrix matrix, std::vector<osg::Vec3d> &vec, std::vector<osg::Vec3d> &vecIndex, osg::Vec4& color,double colorErr,osg::Vec3d normal,float ANGLE)
{
	if (!geode)
	{
		return false;
	}
	int num = geode->getNumDrawables();
	bool breakout = false;
	bool finish = false;

	while (!finish)
	{
		finish = true;
		for (int j = num - 1; j >= 0; --j)
		{
			osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode->getDrawable(j));
			if (!geom)
				continue; //得到可绘制结点的顶点序列 
			osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
			for (size_t k = 0; k < geom->getNumPrimitiveSets(); k++)//k只有1个
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

					int a = indexInTriangle(pt1, vecIndex);
					int b = indexInTriangle(pt2, vecIndex);
					int c = indexInTriangle(pt3, vecIndex);

					if (a != -1 || b != -1 || c != -1)
					{
						osg::Vec3d nm1;
						NormalVector(pt1, pt2, pt3, nm1);
						double m_normal = sqrt(normal.x()*normal.x() + normal.y()*normal.y() + normal.z()*normal.z());

						if (checkNomal(nm1, normal, m_normal, ANGLE) && !triangleDrawed(pt1, pt2, pt3, vecIndex))//如果相邻面片法线相似，更新vecIndex
						{
							osg::ref_ptr<osg::Texture2D> texture = dynamic_cast<osg::Texture2D*> (geom->getOrCreateStateSet()->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
							osg::ref_ptr<osg::Vec2Array> textureArray = dynamic_cast<osg::Vec2Array*> (geom->getTexCoordArray(0));
							if (!texture)
							{
								continue;
							}
							osg::ref_ptr<osg::Image> image = texture->getImage(0);
							int imageWidth = image->s();
							int imageHeight = image->t();
							//获取三个顶点的颜色
							osg::Vec2d texture1, texture2, texture3;
							texture1.x() = textureArray->at(index1).x() * imageWidth;
							texture1.y() = textureArray->at(index1).y() * imageHeight;
							texture2.x() = textureArray->at(index2).x() * imageWidth;
							texture2.y() = textureArray->at(index2).y() * imageHeight;
							texture3.x() = textureArray->at(index3).x() * imageWidth;
							texture3.y() = textureArray->at(index3).y() * imageHeight;
							unsigned char* data = (unsigned char*)(image->getDataPointer());
							
							osg::Vec4 color1;
							std::vector<osg::Vec2d> vecCoord;
							getTextureCoordBy3Points(texture1, texture2, texture3, vecCoord);
							int R = 0, G = 0, B = 0;
							for (size_t i = 0; i<vecCoord.size(); ++i)
							{
								R += (int)data[((int)vecCoord[i].y()) * imageWidth * 3 + ((int)vecCoord[i].x()) * 3 + 0];
								G += (int)data[((int)vecCoord[i].y()) * imageWidth * 3 + ((int)vecCoord[i].x()) * 3 + 1];
								B += (int)data[((int)vecCoord[i].y()) * imageWidth * 3 + ((int)vecCoord[i].x()) * 3 + 2];
							}
							/*int r = data[((int)texture1.y()) * imageWidth * 3 + ((int)texture1.x()) * 3 + 0];
							int g = data[((int)texture1.y()) * imageWidth * 3 + ((int)texture1.x()) * 3 + 1];
							int b = data[((int)texture1.y()) * imageWidth * 3 + ((int)texture1.x()) * 3 + 2];*/
							color1.r() = (R / 1.0) / vecCoord.size() / 255.0;
							color1.g() = (G / 1.0) / vecCoord.size() / 255.0;
							color1.b() = (B / 1.0) / vecCoord.size() / 255.0;
							color1.a() = 1;
							//finish = false;
							if (a == -1 && testColor(color, color1, colorErr))
							{
								vecIndex.push_back(pt1);
								finish = false;
							}
							if (b == -1 && testColor(color, color1, colorErr))
							{
								vecIndex.push_back(pt2);
								finish = false;
							}
							if (c == -1 && testColor(color, color1, colorErr))
							{
								vecIndex.push_back(pt3);
								finish = false;
							}
							if (indexInTriangle(pt1, vec) == -1 && testColor(color, color1, colorErr))
							{
								vec.push_back(pt1);
							}	
							if (indexInTriangle(pt2, vec) == -1 && testColor(color, color1, colorErr))
							{
								vec.push_back(pt2);
							}	
							if (indexInTriangle(pt3, vec) == -1 && testColor(color, color1, colorErr))
							{
								vec.push_back(pt3);
							}	
						}
					}
				}
			}
		}
	}
	return false;
}

bool CSceneExtractWall::getTrianglePointsEx(osg::Geode* geode, osg::Matrix matrix, std::vector<osg::Vec3d> &vec, std::vector<osg::Vec3d> &vecIndex, std::vector<osg::Vec4>& vecColor, double colorErr, std::vector<osg::Vec3d> & vecNormal, float ANGLE)
{
	if (!geode)
	{
		return false;
	}
	int num = geode->getNumDrawables();
	bool breakout = false;
	bool finish = false;

	while (!finish)
	{
		finish = true;
		for (int j = num - 1; j >= 0; --j)
		{
			osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode->getDrawable(j));
			if (!geom)
				continue; //得到可绘制结点的顶点序列 
			osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
			for (size_t k = 0; k < geom->getNumPrimitiveSets(); k++)//k只有1个
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

					int a = indexInTriangle(pt1, vecIndex);
					int b = indexInTriangle(pt2, vecIndex);
					int c = indexInTriangle(pt3, vecIndex);

					if (a != -1 || b != -1 || c != -1)
					{
						osg::Vec3d nm1;
						NormalVector(pt1, pt2, pt3, nm1);
						

						if (checkNomalEx(nm1,vecNormal, ANGLE) && !triangleDrawed(pt1, pt2, pt3, vecIndex))//如果相邻面片法线相似，更新vecIndex
						{
							osg::ref_ptr<osg::Texture2D> texture = dynamic_cast<osg::Texture2D*> (geom->getOrCreateStateSet()->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
							osg::ref_ptr<osg::Vec2Array> textureArray = dynamic_cast<osg::Vec2Array*> (geom->getTexCoordArray(0));
							if (!texture)
							{
								continue;
							}
							osg::ref_ptr<osg::Image> image = texture->getImage(0);
							int imageWidth = image->s();
							int imageHeight = image->t();
							//获取三个顶点的颜色
							osg::Vec2d texture1, texture2, texture3;
							texture1.x() = textureArray->at(index1).x() * imageWidth;
							texture1.y() = textureArray->at(index1).y() * imageHeight;
							texture2.x() = textureArray->at(index2).x() * imageWidth;
							texture2.y() = textureArray->at(index2).y() * imageHeight;
							texture3.x() = textureArray->at(index3).x() * imageWidth;
							texture3.y() = textureArray->at(index3).y() * imageHeight;
							unsigned char* data = (unsigned char*)(image->getDataPointer());

							osg::Vec4 color1;
							std::vector<osg::Vec2d> vecCoord;
							getTextureCoordBy3Points(texture1, texture2, texture3, vecCoord);
							int R = 0, G = 0, B = 0;
							for (size_t i = 0; i<vecCoord.size(); ++i)
							{
								R += (int)data[((int)vecCoord[i].y()) * imageWidth * 3 + ((int)vecCoord[i].x()) * 3 + 0];
								G += (int)data[((int)vecCoord[i].y()) * imageWidth * 3 + ((int)vecCoord[i].x()) * 3 + 1];
								B += (int)data[((int)vecCoord[i].y()) * imageWidth * 3 + ((int)vecCoord[i].x()) * 3 + 2];
							}
							
							color1.r() = (R / 1.0) / vecCoord.size() / 255.0;
							color1.g() = (G / 1.0) / vecCoord.size() / 255.0;
							color1.b() = (B / 1.0) / vecCoord.size() / 255.0;
							color1.a() = 1;
							
							if (a == -1 && testColorEx(color1, vecColor, colorErr))
							{
								vecIndex.push_back(pt1);
								finish = false;
							}
							if (b == -1 && testColorEx(color1, vecColor, colorErr))
							{
								vecIndex.push_back(pt2);
								finish = false;
							}
							if (c == -1 && testColorEx(color1, vecColor, colorErr))
							{
								vecIndex.push_back(pt3);
								finish = false;
							}
							if (indexInTriangle(pt1, vec) == -1 && testColorEx(color1, vecColor, colorErr))
							{
								vec.push_back(pt1);
							}
							if (indexInTriangle(pt2, vec) == -1 && testColorEx(color1, vecColor, colorErr))
							{
								vec.push_back(pt2);
							}
							if (indexInTriangle(pt3, vec) == -1 && testColorEx(color1, vecColor, colorErr))
							{
								vec.push_back(pt3);
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool CSceneExtractWall::testColorEx(osg::Vec4& color1, std::vector<osg::Vec4>& vecColor, double err)
{
	for (size_t i = 0;i<vecColor.size();++i)
	{
		if (testColor(color1,vecColor[i],err))
		{
			return true;
		}
	}
	return false;
}

//检查颜色是否在差异内
bool CSceneExtractWall::testColor(osg::Vec4& color1, osg::Vec4& color2, double err)
{
	double absR = color1.r() >= color2.r() ? color1.r() - color2.r() : color2.r() - color1.r();
	double absG = color1.g() >= color2.g() ? color1.g() - color2.g() : color2.g() - color1.g();
	double absB = color1.b() >= color2.b() ? color1.b() - color2.b() : color2.b() - color1.b();
	if (sqrt(absB*absB + absG*absG + absR*absR) < (1-err)*sqrt(3.0))
	{
		return true;
	}
	return false;
	/*double r1 = color1.r() / 255;
	double g1 = color1.g() / 255;
	double b1 = color1.b() / 255;
	double r2 = color2.r() / 255;
	double g2 = color2.g() / 255;
	double b2 = color2.b() / 255;

	double max1 = r1 > g1 ? r1 : g1;
	max1 = max1 > b1 ? max1 : b1;
	double max2 = r2 > g2 ? r2 : g2;
	max2 = max2 > b2 ? max2 : b2;

	double min1 = r1 < g1 ? r1 : g1;
	min1 = min1 < b1 ? min1 : b1;
	double min2 = r2 < g2 ? r2 : g2;
	min2 = min2 < b2 ? min2 : b2;
	double h1, s1, v1, h2, s2, v2;
	if (max1 == min1)
	{
		h1 = 0;
	}
	else if (max1 == r1 && g1>=b1)
	{
		h1 = 60 * (g1 - b1) / (max1 - min1);
	}
	else if (max1 == r1 && g1<b1)
	{
		h1 = 60 * (g1 - b1) / (max1 - min1) + 360;
	}
	else if (max1 == g1)
	{
		h1 = 60 * (b1 - r1) / (max1 - min1) + 120;
	}
	else if (max1 == b1)
	{
		h1 = 60 * (r1 - g1) / (max1 - min1) + 240;
	}
	if (max1 == 0)
	{
		s1 = 0;
	}
	else
		s1 = 1 - min1 / max1;
	v1 = max1;


	if (max2 == min2)
	{
		h2 = 0;
	}
	else if (max2 == r2 && g2 >= b2)
	{
		h2 = 60 * (g2 - b2) / (max2 - min2);
	}
	else if (max2 == r2 && g2 < b2)
	{
		h2 = 60 * (g2 - b2) / (max2 - min2) + 360;
	}
	else if (max2 == g2)
	{
		h2 = 60 * (b2 - r2) / (max2 - min2) + 120;
	}
	else if (max2 == b2)
	{
		h2 = 60 * (r2 - g2) / (max2 - min2) + 240;
	}
	if (max2 == 0)
	{
		s2 = 0;
	}
	else
		s2 = 1 - min2 / max2;
	v2 = max2;

	if (h1>h2)
	{
		if (h2/h1>err)
		{
			return true;
		}
		return false;
	}
	else
	{
		if (h1/h2>err)
		{
			return true;
		}
		return false;
	}*/
}

void CSceneExtractWall::getTextureCoordBy3Points(osg::Vec2d& pt1, osg::Vec2d& pt2, osg::Vec2d& pt3, std::vector<osg::Vec2d> &vec)
{
	vec.clear();
	std::vector<CL::Vec2d> pol;
	pol.push_back(CL::Vec2d(pt1.x(),pt1.y()));
	pol.push_back(CL::Vec2d(pt2.x(), pt2.y()));
	pol.push_back(CL::Vec2d(pt3.x(), pt3.y()));
	pol.push_back(CL::Vec2d(pt1.x(), pt1.y()));
	//区域范围
	double maxX = pt1.x() > pt2.x() ? pt1.x() : pt2.x();
	maxX = maxX > pt3.x() ? maxX : pt3.x();
	double maxY = pt1.y() > pt2.y() ? pt1.y() : pt2.y();
	maxY = maxY > pt3.y() ? maxY : pt3.y();
	double minX = pt1.x() > pt2.x() ? pt2.x() : pt1.x();
	minX = minX > pt3.x() ? pt3.x() : minX;
	double minY = pt1.y() > pt2.y() ? pt2.y() : pt1.y();
	minY = minY > pt3.y() ? pt3.y() : minY;
	for (int x = (int)minX;x<(int)maxX;x++)
	{
		for (int y = (int)minY; y<(int)maxY; y++)
		{
			if (Point_In_Polygon_2D(x, y, pol))
			{
				vec.push_back(osg::Vec2d(x,y));
			}
		}
	}
}

//三角是否重复搜索
bool CSceneExtractWall::triangleDrawed(osg::Vec3d& a, osg::Vec3d& b, osg::Vec3d& c, std::vector<osg::Vec3d> &vec)
{
	int x = indexInTriangle(a, vec);
	int y = indexInTriangle(b, vec);
	int z = indexInTriangle(c, vec);
	if (x != -1 && y != -1 && z != -1)
	{
		{
			return true;
		}
	}
	return false;
}

int CSceneExtractWall::indexInTriangle(osg::Vec3d& pt, std::vector<osg::Vec3d>& vecPt)
{
	for (size_t i = 0; i != vecPt.size(); ++i)
	{
		if (isSamePt(pt, vecPt[i]))
		{
			return i;
		}
	}
	return -1;
}

//计算三角面片的法向量
void CSceneExtractWall::NormalVector(osg::Vec3d& a, osg::Vec3d& b, osg::Vec3d& c, osg::Vec3d& nomal)
{
	osg::Vec3d ab = b - a;
	osg::Vec3d bc = c - b;

	nomal.x() = (-ab.z()*bc.y() + ab.y()*bc.z()) / (ab.x()*bc.y() - bc.x()*ab.y());
	nomal.y() = (-bc.x()*ab.z() + ab.x()*bc.z()) / (bc.x()*ab.y() - ab.x()*bc.y());
	nomal.z() = 1;
}


//检验法向量与给定法向量是否在允许偏差内
bool CSceneExtractWall::checkNomal(osg::Vec3d nom, osg::Vec3d normal, double m_normal, double err)
{
	double mo = sqrt(nom.x()*nom.x() + nom.y()*nom.y() + nom.z()*nom.z());
	double ji = nom.x() * normal.x() + nom.y() * normal.y() + nom.z() * normal.z();
	double angle = acos(ji / (mo * m_normal));
	angle = angle * 180 / osg::PI;

	if (angle < err || angle>180 - err)
	{
		return true;
	}
	return false;
}

bool CSceneExtractWall::checkNomalEx(osg::Vec3d nom, std::vector<osg::Vec3d>& vecNormal, double err)
{
	for (size_t i = 0;i<vecNormal.size();++i)
	{
		double mo = sqrt(vecNormal[i].x()*vecNormal[i].x() + vecNormal[i].y()*vecNormal[i].y() + vecNormal[i].z()*vecNormal[i].z());
		if (checkNomal(nom,vecNormal[i],mo,err))
		{
			return true;
		}
	}
	return false;
}

//判断点在三角面片内
bool CSceneExtractWall::PtInTriangle(osg::Vec3d pt, osg::Vec3d& t1, osg::Vec3d& t2, osg::Vec3d& t3)
{
	osg::Vec3d vec1 = t1 - pt;
	osg::Vec3d vec2 = t2 - pt;
	osg::Vec3d vec3 = t3 - pt;

	double ang1 = CL::getAngleForVector(vec1, vec2);
	double ang2 = CL::getAngleForVector(vec1, vec3);
	double ang3 = CL::getAngleForVector(vec3, vec2);

	if (ang1 + ang2 + ang3 <= 360 + 5 && ang1 + ang2 + ang3 >= 360 - 5)
	{
		return true;
	}
	return false;
}

int CSceneExtractWall::FindMinMax(std::vector<osg::Vec3d>& vecPt, osg::Vec3d& ptMinXY, osg::Vec3d& ptMaxXY, double& MinZ, double& MaxZ,double& MinX, double& MaxX, double& MinY, double& MaxY )
{
	if (vecPt.empty())
	{
		return 0;
	}
	osg::Vec3d ptMinX, ptMaxX, ptMinY, ptMaxY, ptMinZ, ptMaxZ;
	ptMinY = vecPt[0];
	ptMaxY = vecPt[0];
	ptMinX = vecPt[0];
	ptMaxX = vecPt[0];
	ptMinZ = vecPt[0];
	ptMaxZ = vecPt[0];

	for (size_t i = 1; i < vecPt.size(); ++i)
	{
		if (vecPt[i].y() < ptMinY.y())
		{
			ptMinY = vecPt[i];
		}
		else if (vecPt[i].y() > ptMaxY.y())
		{
			ptMaxY = vecPt[i];
		}

		if (vecPt[i].x() < ptMinX.x())
		{
			ptMinX = vecPt[i];
		}
		else if (vecPt[i].x() > ptMaxX.x())
		{
			ptMaxX = vecPt[i];
		}

		if (vecPt[i].z() < ptMinZ.z())
		{
			ptMinZ = vecPt[i];
		}
		else if (vecPt[i].z() > ptMaxZ.z())
		{
			ptMaxZ = vecPt[i];
		}
	}
	MinX = ptMinX.x();
	MaxX = ptMaxX.x();
	MinY = ptMinY.y();
	MaxY = ptMaxY.y();
	MinZ = ptMinZ.z();
	MaxZ = ptMaxZ.z();

	if (ptMaxX.x() - ptMinX.x() > ptMaxY.y() - ptMinY.y())
	{
		ptMinXY = ptMinX;
		ptMaxXY = ptMaxX;
	}
	else
	{
		ptMinXY = ptMinY;
		ptMaxXY = ptMaxY;
	}
	return 1;
}

void CSceneExtractWall::getPointProject(osg::Vec3d& pt, osg::Vec3d& ptPrj, PLane& pl)//计算投影点
{
	double k = -getDisFromPointToPlane(pt, pl);
	ptPrj.x() = pt.x() + k*pl.A;
	ptPrj.y() = pt.y() + k*pl.B;
	ptPrj.z() = pt.z() + k*pl.C;
}

double CSceneExtractWall::getDisFromPointToPlane(osg::Vec3d& pt, PLane& pl)
{
	return (pl.A * pt.x() + pl.B * pt.y() + pl.C * pt.z() + pl.D) / sqrt(pl.A * pl.A + pl.B*pl.B + pl.C * pl.C);
}

//由对角点计算矩形的四个顶点
void CSceneExtractWall::getCoordBy2Point(osg::Vec3d& pt1, osg::Vec3d& pt3, std::vector<osg::Vec3d>& coord, PLane& plane)
{
	osg::Vec3d ptmin, ptmax, pt2, pt4;
	if (pt1.z() <= pt3.z())
	{
		ptmin = pt1;
		ptmax = pt3;
	}
	else
	{
		ptmin = pt3;
		ptmax = pt1;
	}
	double a = plane.A;
	double b = plane.B;
	double c = plane.C;
	double d = plane.D;

	double k1 = -a / b;
	double b1 = -(d + c*ptmax.z()) / b;

	double k2 = -1 / k1;
	double b2 = ptmin.y() - ptmin.x()*k2;

	pt2.x() = (b2 - b1) / (k1 - k2);
	pt2.y() = k1*pt2.x() + b1;
	pt2.z() = ptmax.z();

	b1 = -(d + c*ptmin.z()) / b;
	b2 = ptmax.y() - ptmax.x()*k2;

	pt4.x() = (b2 - b1) / (k1 - k2);
	pt4.y() = k1*pt4.x() + b1;
	pt4.z() = ptmin.z();
	coord.clear();
	coord.push_back(ptmin);
	coord.push_back(pt2);
	coord.push_back(ptmax);
	coord.push_back(pt4);
}

void CSceneExtractWall::redrawWall(osg::Vec3d& pt, osg::Vec3d& pt1, std::vector<osg::Vec3d>&vecCoord, WallInfo& wallSelected,osg::Vec4d color, osg::Group* group, osg::Group* tempGroup, osg::Matrix matrix)
{
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	osg::Vec3d p1, p2, p3, p4;

	vecCoord.clear();
	getCoordBy2Point(pt, pt1, vecCoord, *(wallSelected.pl));
	//清除tempGroup
	while (tempGroup->getNumChildren() > 0)
	{
		osg::ref_ptr<osg::Group> childGroup = tempGroup->getChild(0)->asGroup();
		//如果是Group，先删除其子节点，再删除自身
		if (childGroup)
		{
			while (childGroup->getNumChildren() > 0)
				childGroup->removeChildren(0, 1);
		}
		tempGroup->removeChildren(0, 1);
	}
	IDrawer->BaseDrawPolygon(vecCoord, color, group, matrix);
	return;
}

bool CSceneExtractWall::drawIntersectOfPlane(osg::Group* mapperGroup,osg::Group* tempGroup,std::vector<WallInfo> &planeSelect,osg::Vec4d& color, osg::Matrix& worldMatrix, InterfaceSigEventCallBack* iSigEventCallBack)
{
	if (planeSelect[0].angle == planeSelect[1].angle && planeSelect[0].err == planeSelect[1].err)//同一墙面
	{
		return false;
	}
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	double a1 = planeSelect[0].pl->A;
	double b1 = planeSelect[0].pl->B;
	double c1 = planeSelect[0].pl->C;
	double d1 = planeSelect[0].pl->D;

	double a2 = planeSelect[1].pl->A;
	double b2 = planeSelect[1].pl->B;
	double c2 = planeSelect[1].pl->C;
	double d2 = planeSelect[1].pl->D;

	double x1, x2, y1, y2, z1, z2;
	if (planeSelect[0].angle < 10 && planeSelect[1].angle < 10)//假设是墙壁相交
	{
		double minz1 = planeSelect[0].coord[0].z();
		double maxz1 = planeSelect[0].coord[0].z();
		double minz2 = planeSelect[1].coord[0].z();
		double maxz2 = planeSelect[1].coord[0].z();
		for (int i = 1; i < planeSelect[0].coord.size(); ++i)
		{
			if (planeSelect[0].coord[i].z() < minz1)
			{
				minz1 = planeSelect[0].coord[i].z();
			}
			else if (planeSelect[0].coord[i].z() > maxz1)
			{
				maxz1 = planeSelect[0].coord[i].z();
			}
		}
		for (int i = 1; i < planeSelect[1].coord.size(); ++i)
		{
			if (planeSelect[1].coord[i].z() < minz2)
			{
				minz2 = planeSelect[1].coord[i].z();
			}
			else if (planeSelect[1].coord[i].z() > maxz2)
			{
				maxz2 = planeSelect[1].coord[i].z();
			}
		}
		z1 = minz1 > minz2 ? minz1 : minz2;
		x1 = ((-d1 - c1*z1)*b2 - (-d2 - c2*z1)*b1) / (b2*a1 - b1*a2);
		y1 = ((-d1 - c1*z1)*a2 - (-d2 - c2*z1)*a1) / (a2*b1 - a1*b2);

		z2 = maxz1 > maxz2 ? maxz2 : maxz1;
		x2 = ((-d1 - c1*z2)*b2 - (-d2 - c2*z2)*b1) / (b2*a1 - b1*a2);
		y2 = ((-d1 - c1*z2)*a2 - (-d2 - c2*z2)*a1) / (a2*b1 - a1*b2);
	}
	else//假设是屋顶相交
	{
		double minx1 = abs(planeSelect[0].coord[0].x() - planeSelect[0].coord[2].x());
		double minx2 = abs(planeSelect[1].coord[0].x() - planeSelect[1].coord[2].x());
		double minx = minx1 > minx2 ? minx2 : minx1;

		double miny1 = abs(planeSelect[0].coord[0].y() - planeSelect[0].coord[2].y());
		double miny2 = abs(planeSelect[1].coord[0].y() - planeSelect[1].coord[2].y());
		double miny = miny1 > miny2 ? miny2 : miny1;

		if (minx > miny)//x方向
		{
			if (minx1 < minx2)
			{
				x1 = planeSelect[0].coord[0].x();
				y1 = ((-d1 - a1*x1)*c2 - (-d2 - a2*x1)*c1) / (b1*c2 - b2*c1);
				z1 = ((-d1 - a1*x1)*b2 - (-d2 - a2*x1)*b1) / (b2*c1 - b1*c2);

				x2 = planeSelect[0].coord[2].x();
				y2 = ((-d1 - a1*x2)*c2 - (-d2 - a2*x2)*c1) / (b1*c2 - b2*c1);
				z2 = ((-d1 - a1*x2)*b2 - (-d2 - a2*x2)*b1) / (b2*c1 - b1*c2);
			}
			else
			{
				x1 = planeSelect[1].coord[0].x();
				y1 = ((-d1 - a1*x1)*c2 - (-d2 - a2*x1)*c1) / (b1*c2 - b2*c1);
				z1 = ((-d1 - a1*x1)*b2 - (-d2 - a2*x1)*b1) / (b2*c1 - b1*c2);

				x2 = planeSelect[1].coord[2].x();
				y2 = ((-d1 - a1*x2)*c2 - (-d2 - a2*x2)*c1) / (b1*c2 - b2*c1);
				z2 = ((-d1 - a1*x2)*b2 - (-d2 - a2*x2)*b1) / (b2*c1 - b1*c2);
			}
		}
		else//y方向
		{
			if (miny1 < miny2)
			{
				y1 = planeSelect[0].coord[0].y();
				x1 = ((-d1 - b1*y1)*c2 - (-d2 - b2*y1)*c1) / (a1*c2 - a2*c1);
				z1 = ((-d1 - b1*y1)*a2 - (-d2 - b2*y1)*a1) / (c1*a2 - a1*c2);

				y2 = planeSelect[0].coord[2].y();
				x2 = ((-d1 - b1*y2)*c2 - (-d2 - b2*y2)*c1) / (a1*c2 - a2*c1);
				z2 = ((-d1 - b1*y2)*a2 - (-d2 - b2*y2)*a1) / (c1*a2 - a1*c2);
			}
			else
			{
				y1 = planeSelect[1].coord[0].y();
				x1 = ((-d1 - b1*y1)*c2 - (-d2 - b2*y1)*c1) / (a1*c2 - a2*c1);
				z1 = ((-d1 - b1*y1)*a2 - (-d2 - b2*y1)*a1) / (c1*a2 - a1*c2);

				y2 = planeSelect[1].coord[2].y();
				x2 = ((-d1 - b1*y2)*c2 - (-d2 - b2*y2)*c1) / (a1*c2 - a2*c1);
				z2 = ((-d1 - b1*y2)*a2 - (-d2 - b2*y2)*a1) / (c1*a2 - a1*c2);
			}
		}
	}
	//清除tempGroup
	
	clearGroupResult(tempGroup);
	if (!isLineInMapperGroup(osg::Vec3d(x1, y1, z1), osg::Vec3d(x2, y2, z2), mapperGroup))
	{
		osg::ref_ptr<osg::Group> group = new osg::Group;
		IDrawer->BaseDrawLineOperate(osg::Vec3d(x1, y1, z1), osg::Vec3d(x2, y2, z2), color, 3.5, group, worldMatrix);
		//线段以相交墙面的倾斜角命名
		string str = planeSelect[0].normal + "," + planeSelect[1].normal;
		char name[99];
		strcpy(name, str.c_str());
		group->setName(name);
		mapperGroup->addChild(group);
	}
	char strMsg[99];
	float angle = 90 - CL::getAngleForVector(osg::Vec3d(x2 - x1, y2 - y1, z2 - z1), osg::Vec3d(x2 - x1, y2 - y1, 0));
	sprintf(strMsg, "交线信息：倾斜角度：%f °   按Backspace键可撤销当前交线 ", angle);
	iSigEventCallBack->EmitSigShowOpsHint(strMsg);
	return true;
}

bool CSceneExtractWall::isLineInMapperGroup(osg::Vec3d& p1, osg::Vec3d& p2, osg::Group* MapperGroup)
{
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	std::vector<osg::MatrixTransform*> vecTrans;
	IFinder->FindTransNode(MapperGroup, vecTrans);
	for (size_t i = 0; i < vecTrans.size(); i++)
	{
		osg::ref_ptr<osg::MatrixTransform> mTrans = vecTrans[i];
		if (!mTrans)
		{
			continue;
		}
		osg::ref_ptr<osg::Vec3Array> extracted_verts = new osg::Vec3Array;
		IFinder->ExtractVerts(mTrans, extracted_verts);
		osg::Matrix matrix = mTrans->getMatrix();
		if (extracted_verts->size() == 0)
		{
			continue;
		}
		size_t num = extracted_verts->size();
		osg::Vec3d pt1(extracted_verts->at(0).x(), extracted_verts->at(0).y(), extracted_verts->at(0).z());
		osg::Vec3d pt2(extracted_verts->at(num - 1).x(), extracted_verts->at(num - 1).y(), extracted_verts->at(num - 1).z());
		pt1 = pt1 * matrix;
		pt2 = pt2 * matrix;

		if ((distance2(p1, pt1) < 0.00001 && distance2(p2, pt2) < 0.00001) || (distance2(p1, pt2) < 0.00001 && distance2(p2, pt1) < 0.00001))
		{
			return true;
		}
	}
	return false;
}

void CSceneExtractWall::clearGroupResult(osg::Group* tempGroup)
{
	while (tempGroup->getNumChildren() > 0)
	{
		osg::ref_ptr<osg::Group> childGroup = tempGroup->getChild(0)->asGroup();
		//如果是Group，先删除其子节点，再删除自身
		if (childGroup)
		{
			while (childGroup->getNumChildren() > 0)
				childGroup->removeChildren(0, 1);
		}
		tempGroup->removeChildren(0, 1);
	}
}

void CSceneExtractWall::RGB2HSV(osg::Vec4& rgb, osg::Vec4& hsv)
{
	double r1 = rgb.r();
	double g1 = rgb.g();
	double b1 = rgb.b();
	

	double max1 = r1 > g1 ? r1 : g1;
	max1 = max1 > b1 ? max1 : b1;
	

	double min1 = r1 < g1 ? r1 : g1;
	min1 = min1 < b1 ? min1 : b1;
	
	double h1, s1, v1;
	if (max1 == min1)
	{
	h1 = 0;
	}
	else if (max1 == r1 && g1>=b1)
	{
	h1 = 60 * (g1 - b1) / (max1 - min1);
	}
	else if (max1 == r1 && g1<b1)
	{
	h1 = 60 * (g1 - b1) / (max1 - min1) + 360;
	}
	else if (max1 == g1)
	{
	h1 = 60 * (b1 - r1) / (max1 - min1) + 120;
	}
	else if (max1 == b1)
	{
	h1 = 60 * (r1 - g1) / (max1 - min1) + 240;
	}
	if (max1 == 0)
	{
	s1 = 0;
	}
	else
	s1 = 1 - min1 / max1;
	v1 = max1;

	hsv.r() = h1;
	hsv.g() = s1;
	hsv.b() = v1;
	hsv.a() = 1.0;
}