// BaseDrawer.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "BaseDrawer.h"
#include "qcomm.h"
#include "ThreadSafeUpdateCallback.h"

void CSceneDrawer::BaseDrawPtOperate(osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix)
{
	osg::Vec3d xyzTrans = worldMatrix.getTrans();
	osg::Vec3d drawPt(worldPt.x()- xyzTrans.x(), worldPt.y()- xyzTrans.y(), worldPt.z()- xyzTrans.z());
	osg::ref_ptr<osg::Geode> geode = DrawPoint(drawPt, color, size);

	osg::Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);
	AddNodeChildToGroup(geode, drawGroup, drawMatrix);
}

void CSceneDrawer::BaseDrawLineOperate(osg::Vec3d worldPt1, osg::Vec3d worldPt2, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix)
{
	osg::Vec3d xyzTrans = worldMatrix.getTrans();
	osg::Vec3d drawPt1(worldPt1.x()- xyzTrans.x(), worldPt1.y()- xyzTrans.y(), worldPt1.z()- xyzTrans.z());

	osg::Vec3d drawPt2(worldPt2.x()- xyzTrans.x(), worldPt2.y()- xyzTrans.y(), worldPt2.z()- xyzTrans.z());	
	osg::ref_ptr<osg::Geode> geode = DrawLine(drawPt1, drawPt2, color, size);

	osg::Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);
	AddNodeChildToGroup(geode, drawGroup, drawMatrix);
}

void CSceneDrawer::BaseDrawSolidOperate(std::vector<osg::Vec3d> worldCoord, double height, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix)
{
	osg::Vec3d xyzTrans = worldMatrix.getTrans();
	std::vector<osg::Vec3d> coord;

	for (int i = 0; i < worldCoord.size(); i++)
	{
		coord.push_back(osg::Vec3d(worldCoord[i].x() - xyzTrans.x(), worldCoord[i].y() - xyzTrans.y(), worldCoord[i].z() - xyzTrans.z()));
	}

	osg::ref_ptr<osg::Group> groupSolid = DrawSolid(coord, height, color, size);

	osg::Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);
	AddNodeChildToGroup(groupSolid, drawGroup, drawMatrix);
}

void CSceneDrawer::BaseDrawFlatPlaneOperate(osg::Vec3d newCenter, double radius, osg::Vec4d color, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix)
{
	osg::ref_ptr<osg::Geode> geoFlatPlane = DrawFlatPlane(newCenter, radius, color);
	osg::Vec3d xyzTrans = worldMatrix.getTrans();
	osg::Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);
	AddNodeChildToGroup(geoFlatPlane, drawGroup, drawMatrix);
}

void CSceneDrawer::BaseDrawPolygon(std::vector<osg::Vec3d> worldCoord, osg::Vec4d color, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix)
{
	osg::Vec3d xyzTrans = worldMatrix.getTrans();
	std::vector<osg::Vec3d> coord;

	for (int i = 0; i < worldCoord.size(); i++)
	{
		coord.push_back(osg::Vec3d(worldCoord[i].x() - xyzTrans.x(), worldCoord[i].y() - xyzTrans.y(), worldCoord[i].z() - xyzTrans.z()));
	}
	osg::ref_ptr<osg::Geode> geode = DrawPolygon(coord, color);
	osg::Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);
	AddNodeChildToGroup(geode, drawGroup, drawMatrix);
}

void CSceneDrawer::BaseDrawLineLoopOperate(std::vector<osg::Vec3d> vecWorldCoord, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix)
{
	osg::Vec3d xyzTrans = worldMatrix.getTrans();

	for (int i = 0; i < vecWorldCoord.size(); i++)
	{
		vecWorldCoord[i] -= xyzTrans;
	}

	osg::ref_ptr<osg::Geode> geodeLineLoop = DrawLineLoop(vecWorldCoord, color, size);
	osg::Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);
	AddNodeChildToGroup(geodeLineLoop, drawGroup, drawMatrix);
}

void CSceneDrawer::BaseDrawCircleLoopOperate(osg::Vec3d newCenter, double radius, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix)
{
	osg::Vec3d xyzTrans = worldMatrix.getTrans();
	newCenter -= xyzTrans;

	osg::ref_ptr<osg::Geode> geodeLineLoop = DrawCircleLoop(newCenter, radius, color, size);
	osg::Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);
	AddNodeChildToGroup(geodeLineLoop, drawGroup, drawMatrix);
}

void CSceneDrawer::BaseDrawCurLineOperate(std::vector<osg::Vec3d> vecWorldCoord, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix)
{
	osg::Vec3d xyzTrans = worldMatrix.getTrans();

	for (int i = 0; i < vecWorldCoord.size(); i++)
	{
		vecWorldCoord[i] -= xyzTrans;
	}

	osg::ref_ptr<osg::Geode> geodeCurLine = DrawCurLine(vecWorldCoord, color, size);
	osg::Matrix drawMatrix;
	drawMatrix.setTrans(xyzTrans);
	AddNodeChildToGroup(geodeCurLine, drawGroup, drawMatrix);
}



osg::ref_ptr<osg::Geode> CSceneDrawer::DrawPoint(osg::Vec3d pt, osg::Vec4d color, double size)
{
	//绘制点
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	osg::ref_ptr<osg::Geometry> geomPt = new osg::Geometry();
	//设定点
	osg::ref_ptr<osg::Vec3Array> ptArray = new osg::Vec3Array();
	ptArray->push_back(pt);
	geomPt->setVertexArray(ptArray);
	//设定颜色
	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array();
	colorArray->push_back(color);
	geomPt->setColorArray(colorArray);
	geomPt->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	//设定点样式
	geomPt->setStateSet(MakePtState(size));
	//添加几何节点
	geomPt->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, ptArray->size()));
	geode->addDrawable(geomPt);

	//设置类型
	geode->setName("point");
	return geode;
}

osg::ref_ptr<osg::Geode> CSceneDrawer::DrawLine(osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec4d color, double size)				  
{
	//绘制线
	osg::ref_ptr<osg::Geode> geoLine = new osg::Geode();
	osg::ref_ptr<osg::Geometry> geomLine = new osg::Geometry();
	osg::ref_ptr<osg::Vec4Array> lineColor = new osg::Vec4Array();
	osg::ref_ptr<osg::Vec3Array> lineArray = new osg::Vec3Array();

	lineArray->push_back(pt1);
	lineArray->push_back(pt2);
	lineColor->push_back(color);
	geomLine->setVertexArray(lineArray);
	geomLine->setColorArray(lineColor);
	geomLine->setColorBinding(osg::Geometry::BIND_OVERALL);
	geomLine->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));
	geoLine->addDrawable(geomLine);

	//设置线样式
	geoLine->setStateSet(MakeLineState());

	//设置线宽
	osg::ref_ptr <osg::LineWidth> LineSize = new osg::LineWidth; 
	LineSize->setWidth(size);
	geomLine->getOrCreateStateSet()->setAttributeAndModes(LineSize.get(), osg::StateAttribute::ON);
	return geoLine;
}

osg::ref_ptr<osg::Geode> CSceneDrawer::DrawLineLoop(std::vector<osg::Vec3d> coord, osg::Vec4d color, double size)
{
	//绘制线
	osg::ref_ptr<osg::Geode> geoLineLoop = new osg::Geode();
	osg::ref_ptr<osg::Geometry> geomLineLoop = new osg::Geometry();
	osg::ref_ptr<osg::Vec4Array> lineLoopColor = new osg::Vec4Array();
	osg::ref_ptr<osg::Vec3Array> lineLoopArray = new osg::Vec3Array();

	for (int i = 0; i < coord.size(); i++)
	{
		lineLoopArray->push_back(coord[i]);
	}

	lineLoopColor->push_back(color);
	geomLineLoop->setVertexArray(lineLoopArray);
	geomLineLoop->setColorArray(lineLoopColor);
	geomLineLoop->setColorBinding(osg::Geometry::BIND_OVERALL);
	geomLineLoop->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, lineLoopArray->size()));
	geoLineLoop->addDrawable(geomLineLoop);

	//设置线样式
	geoLineLoop->setStateSet(MakeLineState());

	//设置线宽
	osg::ref_ptr <osg::LineWidth> LineSize = new osg::LineWidth; 
	LineSize->setWidth(size);
	geomLineLoop->getOrCreateStateSet()->setAttributeAndModes(LineSize.get(), osg::StateAttribute::ON);
	return geoLineLoop;
}

osg::ref_ptr<osg::Geode> CSceneDrawer::DrawCurLine(std::vector<osg::Vec3d> coord, osg::Vec4d color, double size)
{
	//绘制线
	osg::ref_ptr<osg::Geode> geoCurLine = new osg::Geode();
	osg::ref_ptr<osg::Geometry> geomCurLine = new osg::Geometry();
	osg::ref_ptr<osg::Vec4Array> curLineColor = new osg::Vec4Array();
	osg::ref_ptr<osg::Vec3Array> curLineArray = new osg::Vec3Array();

	for (int i = 0; i < coord.size(); i++)
	{
		curLineArray->push_back(coord[i]);
	}

	curLineColor->push_back(color);
	geomCurLine->setVertexArray(curLineArray);
	geomCurLine->setColorArray(curLineColor);
	geomCurLine->setColorBinding(osg::Geometry::BIND_OVERALL);
	geomCurLine->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, curLineArray->size()));
	geoCurLine->addDrawable(geomCurLine);

	//设置线样式
	geoCurLine->setStateSet(MakeLineState());

	//设置线宽
	osg::ref_ptr <osg::LineWidth> LineSize = new osg::LineWidth; 
	LineSize->setWidth(size);
	geomCurLine->getOrCreateStateSet()->setAttributeAndModes(LineSize.get(), osg::StateAttribute::ON);
	return geoCurLine;
}

osg::ref_ptr<osg::Geode> CSceneDrawer::DrawPolygon(std::vector<osg::Vec3d> coord, osg::Vec4d color)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
	osg::ref_ptr<osg::Vec4Array> plygonColor = new osg::Vec4Array();
	osg::ref_ptr<osg::Vec3Array> plygonArray = new osg::Vec3Array();

	for (int i = 0; i < coord.size(); i++)
	{
		plygonArray->push_back(coord[i]);
	}

	plygonColor->push_back(color);
	geom->setVertexArray(plygonArray);
	geom->setColorArray(plygonColor);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::DrawElementsUInt* geomBase = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 1);

	for (int i = 0; i < coord.size(); i++)
	{
		geomBase->push_back(i);
	}

	geom->addPrimitiveSet(geomBase);
	geode->addDrawable(geom);

	osg::ref_ptr<osg::StateSet> stateset=geode->getOrCreateStateSet();  
	stateset->setMode(GL_BLEND,osg::StateAttribute::ON);  
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);  
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	geode->setStateSet(stateset);

	return geode;
}

osg::ref_ptr<osg::Group> CSceneDrawer::DrawSolid(std::vector<osg::Vec3d> coord, double height, osg::Vec4d color, double size)	  
{
	//画第一个面
	osg::ref_ptr<osg::Group> group = new osg::Group;
	osg::ref_ptr<osg::Geode> geode1 = DrawPolygon(coord, color);
	group->addChild(geode1);
	std::vector<osg::Vec3d> coordUp;

	for(int i = 0; i < coord.size(); i++)
	{
		coordUp.push_back(osg::Vec3d(coord[i].x(), coord[i].y(), coord[i].z() + height));
	}

	//画第二个面
	osg::ref_ptr<osg::Geode> geode2 = DrawPolygon(coordUp, color);
	group->addChild(geode2);

	//画第三个面 到最后
	for (int i = 0; i < coord.size(); i++)
	{
		int index1 = i;
		int index2 = i + 1;

		if (index1 == coord.size() - 1)
		{
			index2 = 0;
		}

		std::vector<osg::Vec3d> coordSideFace;
		coordSideFace.push_back(coord[index1]);
		coordSideFace.push_back(coord[index2]);
		coordSideFace.push_back(coordUp[index2]);
		coordSideFace.push_back(coordUp[index1]);
		osg::ref_ptr<osg::Geode> geode3 = DrawPolygon(coordSideFace, color);
		group->addChild(geode3);
	}

	return group;
}

osg::ref_ptr<osg::Geode> CSceneDrawer::DrawFlatPlane(osg::Vec3d center, double radius, osg::Vec4d color)
{
	double angle = 0;
	double PI = 3.14159f;
	std::vector<osg::Vec3d> coordCircle;

	for(int i = 0; i < 72; i++)
	{
		double circle[72][2] = {0};
		circle[i][0] = radius * cos(angle) + center.x();
		circle[i][1] = radius * sin(angle) + center.y();
		angle += 2.0 * PI / 72.0f;
		coordCircle.push_back(osg::Vec3d(circle[i][0], circle[i][1], center.z()));
	}

	osg::ref_ptr<osg::Geode> geoPolygon = DrawPolygon(coordCircle, color);
	//设置半透明
	osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();  // blend func    
	blendFunc->setSource(osg::BlendFunc::SRC_ALPHA);       
	blendFunc->setDestination(osg::BlendFunc::ONE_MINUS_SRC_ALPHA);  
	osg::ref_ptr<osg::StateSet> stateset = geoPolygon->getOrCreateStateSet();
	stateset->setAttributeAndModes(blendFunc);
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);  
	return geoPolygon;
}

osg::ref_ptr<osg::Geode> CSceneDrawer::DrawCircleLoop(osg::Vec3d center, double radius, osg::Vec4d color, double size)
{
	double angle = 0;
	double PI = 3.14159f;
	std::vector<osg::Vec3d> coordCircle;

	for(int i = 0; i < 72; i++)
	{
		double circle[72][2] = {0};
		circle[i][0] = radius * cos(angle) + center.x();
		circle[i][1] = radius * sin(angle) + center.y();
		angle += 2.0 * PI / 72.0f;
		coordCircle.push_back(osg::Vec3d(circle[i][0], circle[i][1], center.z()));
	}

	osg::ref_ptr<osg::Geode> geoCircleLoop = DrawLineLoop(coordCircle, color, size);
	//设置半透明
	osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();  // blend func    
	blendFunc->setSource(osg::BlendFunc::SRC_ALPHA);       
	blendFunc->setDestination(osg::BlendFunc::ONE_MINUS_SRC_ALPHA);  
	osg::ref_ptr<osg::StateSet> stateset = geoCircleLoop->getOrCreateStateSet();
	stateset->setAttributeAndModes(blendFunc);
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);  
	return geoCircleLoop;
}

void CSceneDrawer::AddNodeChildToGroup(osg::ref_ptr<osg::Node> nodeChild, osg::ref_ptr<osg::Group> parent, osg::Matrix matrix)
{
	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
	trans->setMatrix(matrix);
	trans->addChild(nodeChild);

	parent->addChild(trans);
}

osg::ref_ptr<osg::StateSet> CSceneDrawer::MakePtState(int size)
{
	//设置样式
	osg::ref_ptr<osg::StateSet> set = new osg::StateSet();
	osg::ref_ptr<osg::Point> point = new osg::Point();
	point->setSize(size);
	set->setAttribute(point);
	set->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	set->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	return set;
}

osg::ref_ptr<osg::StateSet> CSceneDrawer::MakeLineState()
{
	//设置样式
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
	osg::ref_ptr<osg::LineStipple> linestipple = new osg::LineStipple;
	linestipple->setFactor(1);
	stateset->setAttributeAndModes(linestipple,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	return stateset;
}

osg::ref_ptr<osg::StateSet> CSceneDrawer::MakeLineStateNT()
{
	//设置样式
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
	osg::ref_ptr<osg::LineStipple> linestipple = new osg::LineStipple;
	linestipple->setFactor(1);
	stateset->setAttributeAndModes(linestipple,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	return stateset;
}

void CSceneDrawer::ClearGroupChild(osg::ref_ptr<osg::Group> group)
{
	while(group->getNumChildren()>0)
	{
		osg::ref_ptr<osg::Group> childGroup = group->getChild(0)->asGroup();
		//如果是Group，先删除其子节点，再删除自身
		if(childGroup)
		{
			while( childGroup->getNumChildren() >0)
				childGroup->removeChildren(0,1);
		}

		group->removeChildren(0,1);
	}
}

void CSceneDrawer::DeleteVecChild(int childIndex, std::vector<osg::Vec3d> &vecGroup)
{
	int childNum = vecGroup.size();

	if (childNum == 0)
	{
		return;
	}

	std::vector<osg::Vec3d>::iterator iter = vecGroup.begin() + childNum - 1;
	vecGroup.erase(iter);
}

osg::ref_ptr<osg::Geode> CSceneDrawer::DrawPolygonNT(std::vector<osg::Vec3d> coord, osg::Vec4d color)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
	osg::ref_ptr<osg::Vec4Array> plygonColor = new osg::Vec4Array();
	osg::ref_ptr<osg::Vec3Array> plygonArray = new osg::Vec3Array();

	for (int i = 0; i < coord.size(); i++)
	{
		plygonArray->push_back(coord[i]);
	}

	plygonColor->push_back(color);
	geom->setVertexArray(plygonArray);
	geom->setColorArray(plygonColor);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::DrawElementsUInt* geomBase = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 1);

	for (int i = 0; i < coord.size(); i++)
	{
		geomBase->push_back(i);
	}

	geom->addPrimitiveSet(geomBase);
	geode->addDrawable(geom);
	geode->setStateSet(MakeLineStateNT());

	return geode;
}

void CSceneDrawer::AddDraggerToScene(osg::ref_ptr<osg::Group> mRoot,osg::ref_ptr<osg::Group> tempGroup,osg::ref_ptr<osg::Group> scopeGroup)
{
	osg::ref_ptr<osg::Geode> pGeoSphereDragger= new osg::Geode();
	osg::ref_ptr<osgManipulator::CommandManager> cmdMgr = new osgManipulator::CommandManager;
	osg::Node* scene = pGeoSphereDragger.get();
	scene->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	osgManipulator::Selection* selection = new osgManipulator::Selection;
	selection->addChild(scene);
	std::string name =" ";
	osgManipulator::Dragger* dragger = createDragger(scopeGroup);
	dragger->setName( name);
	osg::Group* root = new osg::Group;
	root->addChild(dragger);
	root->addChild(selection);
	cmdMgr->connect(*dragger, *selection);
	osg::ref_ptr<CThreadSafeUpdateCallback> spCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
	// Add the model to the scene
	if (spCallback != NULL)
	{
		spCallback->AddChild(tempGroup,root);
		spCallback->AddChild(mRoot,tempGroup);
	}
}

osgManipulator::Dragger* CSceneDrawer::createDragger(osg::Group* group)
{
	osgManipulator::Dragger* dragger = 0;
	CDraggerPoint* d = new CDraggerPoint(group);
	d->setScence();
	dragger = d;
	return dragger;
}