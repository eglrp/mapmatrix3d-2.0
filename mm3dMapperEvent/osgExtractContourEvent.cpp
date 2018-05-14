#include "stdafx.h"
#include "osgExtractContourEvent.h"
#include "ThreadSafeUpdateCallback.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceOsgFinder.h"
#include "OsgPagedLodBottomLevelFinder.h"
#include "PGControl.h"
#include "outline_3.h"
void COsgExtractContourEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_EXTRACT_CONTOUR);

	if (isActivated == false && flag == true)													 //不激活
	{
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		
	}
}

bool COsgExtractContourEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{
				if (mMapperGroup->getNumChildren() > 0)
				{
					mMapperGroup->removeChild(mMapperGroup->getNumChildren() - 1);
				}
				return false;
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				ClearGroupResult(mMapperGroup);
				return false;
			}
		}
	}
	return false;
}

void COsgExtractContourEvent::extractContour(double zs, double ze, double zspan)
{
	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	//查找底层数据
	
	std::vector<std::string> vecChildFileName;
	IFinder->FindLodBottomLevelFile(sceneNode, vecChildFileName);

	bool isSingle = false;
	if (vecChildFileName.size() == 0)
	{
		vecChildFileName.push_back("");
		isSingle = true;
		//return;
	}
	std::vector<osg::MatrixTransform*> vecTrans;
	IFinder->FindTransNode(sceneNode, vecTrans);
	if (vecTrans.size() > 1)
	{
		//return;
	}

	osg::Matrix worldMatrix;
	if (vecTrans.size() == 1)
	{
		worldMatrix  = vecTrans[0]->getMatrix();
	}
	if (vecTrans.size() == 0)
	{
		worldMatrix.setTrans(osg::Vec3d(0,0,0));
	}
	iSigEventCallBack->EmitSigShowOpsHint("正在提取等高线...");
	std::vector<CTri3D> vecTri;
	for (int i = 0; i < vecChildFileName.size(); ++i)
	{
		osg::ref_ptr<osg::Node> node;
		if (isSingle)
		{
			node = sceneNode;
		}
		else
			node = osgDB::readNodeFile(vecChildFileName[i]);

		if (!node)
		{
			continue;
		}

		InterfaceOsgFinderFactory iOsgFinderFactory;
		InterfaceOsgFinder* iOsgFinder = iOsgFinderFactory.create();
		std::vector<osg::Geode*> vecPtr = iOsgFinder->FindAndGetGeode(node);

		for (size_t i = 0; i < vecPtr.size(); ++i)
		{
			osg::Geode* geode = vecPtr[i];
			if (!geode)
			{
				continue;
			}
			int num = geode->getNumDrawables();
			for (int j = num - 1; j >= 0; --j)
			{
				osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode->getDrawable(j));

				if (!geom)
					continue; //得到可绘制结点的顶点序列 

				osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

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
						vecTri.push_back(CTri3D(point3D(pt1.x(),pt1.y(),pt1.z()),
							point3D(pt2.x(), pt2.y(), pt2.z()),
							point3D(pt3.x(), pt3.y(), pt3.z())));
					}
				}
			}
		}
	}
	std::vector<osg::Vec3d> vec;
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	osg::ref_ptr<osg::Group> outlineGroup = new osg::Group;
	std::vector<CLine> vecLineTemp;
	for (double z = zs;z<=ze;z+=zspan)
	{
		vecLineTemp.clear();
		for (auto tri : vecTri)
		{
			vec.clear();
			outline_3::computePoints(osg::Vec3d(tri.p1.x, tri.p1.y, tri.p1.z),
				osg::Vec3d(tri.p2.x, tri.p2.y, tri.p2.z),
				osg::Vec3d(tri.p3.x, tri.p3.y, tri.p3.z),z,vec);
			if (vec.size() == 2)
			{
				//IDrawer->BaseDrawCurLineOperate(vec, osg::Vec4(1, 1, 0, 1), 2.5, outlineGroup, worldMatrix);
				vecLineTemp.push_back(CLine(vec[0], vec[1]));
			}
		}
		while (vecLineTemp.size() > 0)
		{
			std::list<osg::Vec3d> lineRes;
			CLine line;
			line.p1 = vecLineTemp.front().p1;
			line.p2 = vecLineTemp.front().p2;
			vecLineTemp.erase(vecLineTemp.begin());
			lineRes.push_back(line.p1);
			lineRes.push_back(line.p2);
			for (std::vector<CLine>::iterator ite = vecLineTemp.begin(); ite != vecLineTemp.end();)
			{
				if (((*ite).p1 - lineRes.front()).length() < 0.001)
				{
					lineRes.push_front((*ite).p2);
				}
				else if (((*ite).p1 - lineRes.back()).length() < 0.001)
				{
					lineRes.push_back((*ite).p2);
				}
				else if (((*ite).p2 - lineRes.front()).length() < 0.001)
				{
					lineRes.push_front((*ite).p1);
				}
				else if (((*ite).p2 - lineRes.back()).length() < 0.001)
				{
					lineRes.push_back((*ite).p1);
				}
				else
				{
					++ite;
					continue;
				}
				vecLineTemp.erase(ite);
				ite = vecLineTemp.begin();
			}
			//当曲线点数大于2并且首尾不相连时，绘制
			if (lineRes.size() > 1)
			{
				std::vector<osg::Vec3d> vec0;
				for (auto pt:lineRes)
				{
					vec0.push_back(pt);
				}
				IDrawer->BaseDrawCurLineOperate(vec0, osg::Vec4(1, 1, 0, 1), 2.5, outlineGroup, worldMatrix);
			}
		}
	}
	mMapperGroup->addChild(outlineGroup);
	iSigEventCallBack->EmitSigShowOpsHint("等高线提取完毕");
}