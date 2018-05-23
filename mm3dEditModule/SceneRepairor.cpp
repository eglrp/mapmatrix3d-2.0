#include "StdAfx.h"
#include "SceneRepairor.h"
#include "Geometry.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceMFCExcute.h"

CSceneRepairor::CSceneRepairor(void)
{
}


CSceneRepairor::~CSceneRepairor(void)
{
}

bool CSceneRepairor::ObtainGeoRect(osg::ref_ptr<osg::Node> node, osg::Matrix matrix, std::vector<osg::Vec3d> &minMaxGeode)    //获取osgb节点
{
	osg::ref_ptr<osg::Vec3Array> extracted_verts = new osg::Vec3Array;
	InterfaceOsgFinderFactory iFactory;
	InterfaceOsgFinder* IOsgFinder = iFactory.create();
	IOsgFinder->ExtractVerts(node, extracted_verts);
	int size_t = extracted_verts.get()->size() ; 

	if (size_t == 0)
	{
		return false;
	}

	std::vector <osg::Vec3>::iterator iter = extracted_verts.get()->begin();
	double x = iter->x();
	double y = iter->y();
	double z = iter->z();
	osg::Vec3 pt(x, y, z);
	pt = pt * matrix;
	double xMin = pt.x();
	double yMin = pt.y(); 
	double zMin = pt.z();
	double xMax = pt.x();
	double yMax = pt.y();
	double zMax = pt.z();

	for (int i = 0 ; i < size_t; i++)
	{
		double x = iter->x();
		double y = iter->y();
		double z = iter->z();
		osg::Vec3 pt(x, y, z);
		pt = pt * matrix;

		if (xMin > pt.x())
		{
			xMin = pt.x();
		}

		if (yMin > pt.y())
		{
			yMin = pt.y();
		}

		if (zMin > pt.z())
		{
			zMin = pt.z();
		}

		if (xMax < pt.x())
		{
			xMax = pt.x();
		}

		if (yMax < pt.y())
		{
			yMax = pt.y();
		}

		if (zMax < pt.z())
		{
			zMax = pt.z();
		}

		iter++ ; 
	}

	minMaxGeode.push_back(osg::Vec3d(xMin, yMin, zMin));
	minMaxGeode.push_back(osg::Vec3d(xMax, yMax, zMax));
	return true;
}

void CSceneRepairor::InterpolatePt(osg::Geode& geode, osg::Matrix matrix, std::vector<osg::Vec3d> &vecPt, GridDem gDem, bool isRepair)
{
	int childGeomNum = geode.getNumDrawables();

	for (int i = 0; i < childGeomNum; i++)
	{
		osg::ref_ptr<osg::Geometry> geom = dynamic_cast<osg::Geometry*> (geode.getDrawable(i));
		int iAverageSize = 0;

		if (geom)
		{
			osg::ref_ptr<osg::Vec3Array> verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray()); 
			//获取三角形序号
			osg::PrimitiveSet* priSet = geom->getPrimitiveSet(0);
			osg::ref_ptr<osg::DrawElements> drawElementTriangle = priSet->getDrawElements();
			int cnt = drawElementTriangle->getNumIndices();
			int triCnt = cnt / 3;

			for (int w = 0; w < triCnt; w++)
			{
				int index1 = drawElementTriangle->index(w * 3);
				int index2 = drawElementTriangle->index(w * 3 + 1);
				int index3 = drawElementTriangle->index(w * 3 + 2);

				//顶点坐标对应新影像坐标
				std::vector<CoordDouble3D> pt;
				double vertX1 = verts->at(index1).x();
				double vertY1 = verts->at(index1).y();
				double vertZ1 = verts->at(index1).z();
				osg::Vec3d tempPt1(vertX1, vertY1, vertZ1);
				tempPt1 = tempPt1 * matrix;
				pt.push_back(CoordDouble3D(tempPt1.x(), tempPt1.y(), tempPt1.z()));
				vertX1 = tempPt1.x();
				vertY1 = tempPt1.y();
				vertZ1 = tempPt1.z();

				double vertX2 = verts->at(index2).x();
				double vertY2 = verts->at(index2).y();
				double vertZ2 = verts->at(index2).z();
				osg::Vec3d tempPt2(vertX2, vertY2, vertZ2);
				tempPt2 = tempPt2 * matrix;
				pt.push_back(CoordDouble3D(tempPt2.x(), tempPt2.y(), tempPt2.z()));
				vertX2 = tempPt2.x();
				vertY2 = tempPt2.y();
				vertZ2 = tempPt2.z();

				double vertX3 = verts->at(index3).x();
				double vertY3 = verts->at(index3).y();
				double vertZ3 = verts->at(index3).z();
				osg::Vec3d tempPt3(vertX3, vertY3, vertZ3);
				tempPt3 = tempPt3 * matrix;
				pt.push_back(CoordDouble3D(tempPt3.x(), tempPt3.y(), tempPt3.z()));
				vertX3 = tempPt3.x();
				vertY3 = tempPt3.y();
				vertZ3 = tempPt3.z();

				double lfTmp=max(pt[0].x, max(pt[1].x, pt[2].x)) - gDem.x0;
				int iCol_max = ceil(lfTmp / gDem.dx);

				lfTmp = max(pt[0].y, max(pt[1].y, pt[2].y)) - gDem.y0;
				int iRow_max = ceil(lfTmp / gDem.dy);

				lfTmp = min(pt[0].x, min(pt[1].x, pt[2].x)) - gDem.x0;
				int iCol_min = floor(lfTmp / gDem.dx);

				lfTmp = min(pt[0].y, min(pt[1].y, pt[2].y)) - gDem.y0;
				int iRow_min = floor(lfTmp / gDem.dy);

				iRow_min--;
				iRow_max++;
				iCol_min--;
				iCol_max++;

				if(w == 0)
				{
					iAverageSize = max((iRow_max - iRow_min), (iCol_max - iCol_min)); 
				}
				else
				{
					if((iRow_max - iRow_min) > iAverageSize * 10||(iCol_max - iCol_min) > iAverageSize * 10)
					{
						continue; //非正常三角形。。。不处理
					}
				}

				std::vector<CL::Vec2d> pol;
				pol.push_back(CL::Vec2d(vertX1, vertY1));
				pol.push_back(CL::Vec2d(vertX2, vertY2));
				pol.push_back(CL::Vec2d(vertX3, vertY3));
				pol.push_back(CL::Vec2d(vertX1, vertY1));

				for(int k = iRow_min; k <= iRow_max; k++)
				{
					for(int s = iCol_min; s <= iCol_max; s++)
					{
						if(k < 0)
							k = 0;
						if(s < 0)
							s = 0;
						if(k > (gDem.rows - 1))
							continue;
						if(s > (gDem.cols - 1))
							continue;

						double x = gDem.x0 + gDem.dx * s;
						double y = gDem.y0 + gDem.dy * k;
						//利用反距离权重进行插值
						double d1 = sqrt((vertX1 - x) * (vertX1 - x) + (vertY1 - y) * (vertY1 - y));
						double d2 = sqrt((vertX2 - x) * (vertX2 - x) + (vertY2 - y) * (vertY2 - y));
						double d3 = sqrt((vertX3 - x) * (vertX3 - x) + (vertY3 - y) * (vertY3 - y));
						double z = (1 / d1) / (1 / d1 + 1 / d2 + 1 / d3) * vertZ1 + (1 / d2) / (1 / d1 + 1 / d2 + 1 / d3) * vertZ2 + (1 / d3) / (1 / d1 + 1 / d2 + 1 / d3) * vertZ3;

						if (Point_In_Polygon_2D(x, y, pol))
						{
							vecPt.push_back(osg::Vec3d(x, y, z));
						}

					}
				}
			}
		}
	}
}

osg::ref_ptr<osg::Node> CSceneRepairor::createInterpolateRepairPatch(std::vector<osg::Vec3d> vecWorldCoord, osg::Matrix worldMatrix, std::string imgFileName)
{
	//求出位移
	osg::Vec3d xyz = worldMatrix.getTrans();
	std::vector<osg::Vec3d> vecCoord;

	for (int i = 0; i < vecWorldCoord.size(); i++)
	{
		vecCoord.push_back(osg::Vec3d(vecWorldCoord[i].x() - xyz.x(), vecWorldCoord[i].y() - xyz.y(), vecWorldCoord[i].z() - xyz.z()));
	}

	osg::ref_ptr<osg::Node> repairNode = createRepairPatch(vecCoord, imgFileName);

	double* db = (double*) worldMatrix.ptr();

	osg::Matrix newMatrix;
	double idb[16] = {0};
	idb[0] = db[0]; idb[1] = db[1]; idb[2] =db[2];
	idb[4] = db[4]; idb[5] = db[5]; idb[6] =db[6];
	idb[8] = db[8]; idb[9] = db[9]; idb[10] =db[10];
	idb[15] = 1;
	newMatrix.set(idb);
	osg::Matrix iMatrix;
	iMatrix = iMatrix.inverse(newMatrix);

	osg::ref_ptr<osg::MatrixTransform> newTrans = new osg::MatrixTransform;
	newTrans->setMatrix(iMatrix);
	newTrans->setName("repairMatrix");
	newTrans->addChild(repairNode);

	return newTrans;
}

osg::ref_ptr<osg::Node> CSceneRepairor::createRepairPatch(std::vector<osg::Vec3d> vecCoord, std::string imageFileName)
{
	//实时画出
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> verts = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> convertVerts = new osg::Vec3Array;

	for (int i = 0; i < vecCoord.size(); i++)
	{
		verts->push_back(osg::Vec3(vecCoord[i].x(), vecCoord[i].y(), vecCoord[i].z()));
		convertVerts->push_back(verts->at(i));
	}

	double xMin = convertVerts->at(0).x();
	double xMax = convertVerts->at(0).x();
	double yMin = convertVerts->at(0).y();
	double yMax = convertVerts->at(0).y();

	for (int i = 0; i < convertVerts->size(); i++)
	{
		if (xMin > convertVerts->at(i).x())
		{
			xMin = convertVerts->at(i).x();
		}

		if (yMin > convertVerts->at(i).y())
		{
			yMin = convertVerts->at(i).y();
		}

		if (xMax < convertVerts->at(i).x())
		{
			xMax = convertVerts->at(i).x();
		}

		if (yMax < convertVerts->at(i).y())
		{
			yMax = convertVerts->at(i).y();
		}
	}

	double scaleX = xMax - xMin;
	double scaleY = yMax - yMin;
	double scale;

	if (scaleX > scaleY)
	{
		scale = scaleX;
	}
	else
	{
		scale = scaleY;
	}

	geom->setVertexArray(convertVerts);
	//创建Delaunay三角网对象
	osg::ref_ptr<osgUtil::DelaunayTriangulator> dt = new osgUtil::DelaunayTriangulator(convertVerts.get());
	dt->setInputPointArray(convertVerts);
	//生成三角网
	dt->triangulate();
	geom->addPrimitiveSet(dt->getTriangles());

	//补纹理
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile(imageFileName);

	if (!image)
	{
		InterfaceMFCExcuteFactory iMfcExcuteFactory;
		InterfaceMFCExcute* iMfcExcute = iMfcExcuteFactory.create();
		iMfcExcute->PopUpMessage("无法读取影像");
		return NULL;
	}

	int posStart = imageFileName.rfind('\\') + 1;
	int posEnd = imageFileName.length() - 1;
	std::string imageFileTitle = imageFileName.substr(posStart, posEnd);

	image->setFileName(imageFileTitle);
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
	texture->setTextureSize(512, 512);
	texture->setInternalFormat(GL_RGBA);
	texture->setImage(image.get());

	//设置自动生成纹理坐标
	osg::ref_ptr<osg::TexGen> texgen = new osg::TexGen();
	texgen->setMode(osg::TexGen::OBJECT_LINEAR);
	osg::Matrix matrix;
	double db[16] = {0};
	db[0] = 1;  db[1] = 0;	db[2] =  0;  db[3] = 0; 
	db[4] = 0;  db[5] = 1;	db[6] =  0;  db[7] = 0; 
	db[8] = 0;  db[9] = 0;	db[10] = 1;  db[11] = 0; 
	db[12] = -xMin; db[13] = -yMin;	db[14] = 0;  db[15] = scale; 
	matrix.set(db);
	texgen->setPlanesFromMatrix(matrix);

	//样式设置
	osg::ref_ptr<osg::StateSet> state = new osg::StateSet;
	state->setTextureAttributeAndModes(1, texture.get(), osg::StateAttribute::ON);
	state->setTextureAttributeAndModes(1, texgen.get(), osg::StateAttribute::ON);
	osg::PolygonStipple* polygonStripple = new osg::PolygonStipple;
	state->setAttributeAndModes(polygonStripple, osg::StateAttribute::OVERRIDE);
	state->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	state->setTextureMode(0, GL_TEXTURE_GEN_S, osg::StateAttribute::OFF);
	state->setTextureMode(0, GL_TEXTURE_GEN_T, osg::StateAttribute::OFF);
	state->setTextureMode(0, GL_TEXTURE_GEN_R, osg::StateAttribute::OFF);
	state->setTextureMode(0, GL_TEXTURE_GEN_Q, osg::StateAttribute::OFF);
	geom->setStateSet(state.get());

	//设置材质
	osg::ref_ptr<osg::Material> mat = new osg::Material;
	mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 0.0, 0.0, 1));
	mat->setShininess(osg::Material::FRONT_AND_BACK, 0.0);
	osg::Material::Face face = osg::Material::FRONT_AND_BACK;
	mat->setAmbient(face, osg::Vec4f(1, 1, 1, 1));
	mat->setEmission(face, osg::Vec4f(0, 0, 0, 1));
	state->setAttribute(mat.get());

	geode->addDrawable(geom.get());
	geode->setName("repair");
	return geode;
}