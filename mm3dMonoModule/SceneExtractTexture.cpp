#include "StdAfx.h"
#include "SceneExtractTexture.h"
#include "coord.hpp"
#include "InterfaceOsgFinder.h"
#include "InterfaceOsgEditor.h"
#include "InterfaceOsgClip.h"
#include "InterfaceOsgDomGenerator.h"
#include "InterfaceOsgAlgorithm.h"
#include "qcomm.h"

osg::ref_ptr<osg::Node> CSceneExtractTexture::extractTexture(std::vector<osg::Vec3d> VecWorldPt, osg::ref_ptr<osg::Node> sceneNode, double Res)
{
	res = Res;
	vecWorldPt = VecWorldPt;

	//计算需要把模型旋转的角度
	//osg::Matrix rotationMatrix1 = StartCaculateRotationMatrix(vecWorldPt);
	osg::ref_ptr<osg::Node> node = ProjectTextureInRect(sceneNode);
	return node.get();
}

osg::ref_ptr<osg::Image> CSceneExtractTexture::ExtractTextureImage(std::vector<osg::Vec3d> VecWorldPt, osg::ref_ptr<osg::Node> sceneNode, double Res)
{
	res = Res;
	vecWorldPt = VecWorldPt;

	std::string outImgFileName = "1.jpg";
	osg::Matrix rotationMatrix1 = StartCaculateRotationMatrix(vecWorldPt);
	osg::ref_ptr<osg::Image> image = ExtractTextureInRect(sceneNode, outImgFileName);
	return image.get();
}

osg::Matrix CSceneExtractTexture::StartCaculateRotationMatrix(std::vector<osg::Vec3d> vecWorldPt)
{
	double a = 0;
	double b = 0;
	double c = 0;
	double d = 0;
	CVPlane(vecWorldPt, a, b, c, d);

	osg::Vec3d beforeNormal(a, b, c);
	beforeNormal.normalize();
	osg::Vec3d afterNormal(0, 0, 1);

	osg::Vec3d rotationAxis = beforeNormal ^ afterNormal;
	double axisAngle = acos(beforeNormal * afterNormal);

	rotationAxis.normalize();
	osg::Quat quat1(axisAngle, rotationAxis);
	osg::Matrix rotationMatrix1;
	rotationMatrix1.setRotate(quat1);

	return rotationMatrix1;
}


osg::ref_ptr<osg::Node> CSceneExtractTexture::ProjectTextureInRect(osg::ref_ptr<osg::Node> sceneNode)
{
	std::string outImgFileName = "1.jpg";
	ExtractTextureInRect(sceneNode, outImgFileName);
	//实时画出
	osg::ref_ptr<osg::Node> node = createOsgbTextureModel("1.txt", outImgFileName);
	return node.get();
}

osg::ref_ptr<osg::Image> CSceneExtractTexture::ExtractTextureInRect(osg::ref_ptr<osg::Node> sceneNode, std::string outImgFileName)
{
	//计算出转换后的范围， 把需要的tile筛选出来

	//for (int i = 0; i < vecWorldPt.size(); i++)
	//{
	//	osg::Vec3d pt = vecWorldPt[i] * rotationMatrix;
	//	vecCoord.push_back(CoordDouble3D(pt.x(), pt.y(), pt.z()));
	//	vecConvertTransCoord.push_back(pt);
	//}

	//查找变换节点
	std::vector<osg::MatrixTransform*> vecMatrixTrans;
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IOsgFinder = IFinderFactory.create();
	IOsgFinder->FindTransNode(sceneNode, vecMatrixTrans);

	if (vecMatrixTrans.size() == 0)
	{
		return NULL;
	}

	osg::ref_ptr<osg::Group> mGroup = vecMatrixTrans[0]->getChild(0)->asGroup();
	osg::Matrix worldMatrix = vecMatrixTrans[0]->getMatrix();
	CScope scope;
	scope.vecCoord = vecWorldPt;

	InterfaceOsgAlgorithmFactory iOsgAlgorithmFactory;
	InterfaceOsgAlgorithm* iOsgAlgorithm = iOsgAlgorithmFactory.create();
	double minHeight = 0; double maxHeight = 0;
	iOsgAlgorithm->FindMinMaxHeight(vecWorldPt, minHeight, maxHeight);

	scope.minH = minHeight - 1;						//不设置最小上限
	scope.maxH = maxHeight + 1;

	std::vector<CScope> vecScope;
	vecScope.push_back(scope);

	//计算在范围内的底层osgb数据
	osg::ref_ptr<osg::Group> group = new osg::Group;
	FindBottomLevelOsgbModelInRect(vecScope, mGroup, worldMatrix, group);

	////生成真正射影像
	//osg::ref_ptr<osg::MatrixTransform> mTrans = new osg::MatrixTransform;
	//mTrans->setMatrix(worldMatrix * rotationMatrix);
	//mTrans->addChild(group);

	////进行裁切类似悬空物
	//InterfaceOsgClipFactory IClipFactory;
	//InterfaceOsgClip* IClip = IClipFactory.create(BY_SUSPEND_SCOPE);
	//IClip->ClipOsgHeader(mTrans, vecScope, true);

	/*osg::ref_ptr<osgDB::Options> spOptions = new osgDB::Options;
	spOptions->setPluginStringData("WriteImageHint", "IncludeFile");
	osgDB::writeNodeFile(*mTrans, "E:\\1.osgb", spOptions.get());*/

	InterfaceOsgDomGeneratorFactory IOsgDomGeneFactory;
	InterfaceOsgDomGenerator* IDomGene = IOsgDomGeneFactory.create();
	IDomGene->produceNodeDOM(group, outImgFileName, worldMatrix, res, res);

	osg::ref_ptr<osg::Image> image = osgDB::readImageFile(outImgFileName);
	return image.get();
}

bool CSceneExtractTexture::FindBottomLevelOsgbModelInRect(std::vector<CScope> vecScope, osg::ref_ptr<osg::Group> mGroup, osg::Matrix worldMatrix, 
	osg::ref_ptr<osg::Group> group)
{
	std::vector<std::string> vecHalfInPolygonFileName;
	std::vector<std::string> vecTotalInPolygonFileName;
	InterfaceOsgClipFactory IClipFactory;
	InterfaceOsgClip* IClip = IClipFactory.create(BY_SUSPEND_SCOPE);
	IClip->obtainSelTileNamebyScope(vecScope, mGroup, worldMatrix, vecHalfInPolygonFileName, vecTotalInPolygonFileName);

	InterfaceOsgFinderFactory IOsgFinderFactory;
	InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();

	std::vector<std::string> vecBottomLevelFileName;

	for (int i = 0; i < mGroup->getNumChildren(); i++)
	{
		osg::ref_ptr<osg::PagedLOD> pagedLod = dynamic_cast<osg::PagedLOD*> (mGroup->getChild(i));

		if (pagedLod)
		{
			std::vector<std::string> vecTileBottomLevelFileName;
			std::string tileName = pagedLod->getFileName(1);

			if (std::find(vecHalfInPolygonFileName.begin(), vecHalfInPolygonFileName.end(), tileName) != vecHalfInPolygonFileName.end())
			{
				IOsgFinder->FindLodBottomLevelFile(pagedLod, vecTileBottomLevelFileName);
			}
			else if (std::find(vecTotalInPolygonFileName.begin(), vecTotalInPolygonFileName.end(), tileName) != vecTotalInPolygonFileName.end())
			{
				IOsgFinder->FindLodBottomLevelFile(pagedLod, vecTileBottomLevelFileName);
			}
			else
			{
				continue;
			}

			vecBottomLevelFileName.insert(vecBottomLevelFileName.end(), vecTileBottomLevelFileName.begin(), vecTileBottomLevelFileName.end());
		}
	}

	for (int i = 0; i < vecBottomLevelFileName.size(); i++)
	{
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(vecBottomLevelFileName[i]);
		group->addChild(node);
	}
};

osg::ref_ptr<osg::Node> CSceneExtractTexture::createOsgbTextureModel(std::string coordTxtFileName, std::string imageFileName)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> convertVerts = new osg::Vec3Array;

	convertVerts->insert(convertVerts->end(), vecWorldPt.begin(), vecWorldPt.end());
	geom->setVertexArray(convertVerts);															//设置顶点

	int triCnt = 0;
	osg::ref_ptr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt(osg::DrawElementsUInt::TRIANGLES, 0);

	for (int i = 0; i <  vecWorldPt.size() - 2; i++)
	{
		drawElements->push_back(0);
		drawElements->push_back(i + 1);
		drawElements->push_back(i + 2);
	}

	geom->addPrimitiveSet(drawElements.get());

	//读取左上角坐标
	double rectLeft, rectTop, xRes, yRes, rectRight, rectBottom;
	FILE* fpCoord = fopen(coordTxtFileName.c_str(), "r");
	fscanf(fpCoord, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", &rectLeft, &rectTop, &xRes, &yRes, &rectRight, &rectBottom);
	fclose(fpCoord);

	//设置材质
	osg::ref_ptr<osg::Material> material = new osg::Material;
	material->setColorMode(osg::Material::OFF);
	material->setAmbient(osg::Material::FRONT, osg::Vec4(1.0, 1.0, 1.0, 1.0));
	material->setDiffuse(osg::Material::FRONT, osg::Vec4(1.0, 1.0, 1.0, 1.0));
	material->setSpecular(osg::Material::FRONT, osg::Vec4(0.0, 0.0, 0.0, 1.0));
	material->setEmission(osg::Material::FRONT, osg::Vec4(0.0, 0.0, 0.0, 1.0));
	material->setShininess(osg::Material::FRONT, 0);

	//设置纹理
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile(imageFileName);
	int posStart = imageFileName.rfind('\\') + 1;
	int posEnd = imageFileName.length() - 1;
	std::string imageFileTitle = imageFileName.substr(posStart, posEnd);
	image->setFileName(imageFileTitle);
	osg::StateSet* stateset = geom->getOrCreateStateSet();  
	stateset->setAttribute(material.get());

	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
	texture->setInternalFormat(GL_RGBA);
	texture->setImage(image.get());
	stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);  

	int imageWidth = image->s();
	int imageHeight = image->t();
	//设置纹理映射坐标
	osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;

	for (int i = 0; i < convertVerts->size(); i++)
	{
		osg::Vec3d convertPt = convertVerts->at(i);
		double xTextureCoord = (convertPt.x() - rectLeft) / (rectRight - rectLeft);
		double yTextureCoord = (convertPt.y() - rectBottom) / (rectTop - rectBottom);

		int xPixel = xTextureCoord * imageWidth;
		int yPixel = yTextureCoord * imageHeight;

		texcoords->push_back(osg::Vec2(xTextureCoord, yTextureCoord));
	}

	geom->setTexCoordArray(0, texcoords);
	geode->addDrawable(geom);

	return geode.get();
}