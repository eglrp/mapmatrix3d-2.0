// osgExtractWall.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgExtractWall.h"
#include "InterfaceSqliteDatabase.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceMFCExcute.h"
#include "BaseUserEvent.h"
#include "SceneExtractWall.h"
#include "cvLine.h"
#include "Geometry.h"
#include <algorithm>

void COsgExtractWall::savePointDataToSqlite3(std::vector<std::vector<osg::Vec3d> >&vecTotalTriangle, InterfaceSqliteDatabase* iSqliteDatabase, char* tablename,bool add ,char* dbname)
{
	sqlite3* db = iSqliteDatabase->OpenDatabase(dbname);
	char deleteSql[99];
	char insertSql[99];
	if (strcmp(tablename, "Normal") == 0)
	{
		strcpy(deleteSql, "delete from Normal");
		strcpy(insertSql, "insert into Normal( x, y, z)values('%lf', '%lf', '%lf')");
	}
	if (strcmp(tablename, "Points") == 0)
	{
		strcpy(deleteSql, "delete from Points");
		strcpy(insertSql, "insert into Points( x, y, z)values('%lf', '%lf', '%lf')");
	}
	if (add == false)
	{
		
		iSqliteDatabase->ClearAllRecord(db, deleteSql);	
	}
	iSqliteDatabase->StartBegin(db);
	if (vecTotalTriangle.empty())
	{
		iSqliteDatabase->Commit(db);
		iSqliteDatabase->CloseDatabase(db);
		return;
	}
	for (int i = 0; i < vecTotalTriangle.size(); i++)
	{
		for (int j = 0; j < vecTotalTriangle[i].size(); ++j)
		{
			double x = vecTotalTriangle[i][j].x(); double y = vecTotalTriangle[i][j].y(); double z = vecTotalTriangle[i][j].z();
			char* InsertSql = sqlite3_mprintf(insertSql, x, y, z);
			iSqliteDatabase->InsertRecord(db, InsertSql);
		}
	}
	iSqliteDatabase->Commit(db);
	iSqliteDatabase->CloseDatabase(db);
}

void COsgExtractWall::clearPointDataInSqlite3(InterfaceSqliteDatabase* iSqliteDatabase, char* tablename, char* dbname)
{
	sqlite3* db = iSqliteDatabase->OpenDatabase(dbname);
	char* op = "delete from ";
	char *result = (char *)malloc(strlen(op) + strlen(tablename) + 1);
	if (result == NULL)
		return;
	strcpy(result, op);
	strcat(result, tablename);
	iSqliteDatabase->ClearAllRecord(db, result);
	iSqliteDatabase->Commit(db);
	iSqliteDatabase->CloseDatabase(db);
}

void COsgExtractWall::saveWallDataToSqlite3(std::vector<WallInfo>& planePara, InterfaceSqliteDatabase* iSqliteDatabase, char* dbname)
{
	sqlite3* db = iSqliteDatabase->OpenDatabase(dbname);
	iSqliteDatabase->ClearAllRecord(db, "delete from Wall");
	iSqliteDatabase->StartBegin(db);
	if (planePara.empty())
	{
		iSqliteDatabase->Commit(db);
		iSqliteDatabase->CloseDatabase(db);
		return;
	}
	for (int j = 0; j < planePara.size(); ++j)
	{
		double a = planePara[j].pl->A; double b = planePara[j].pl->B; double c = planePara[j].pl->C; double d = planePara[j].pl->D;
		//顶点1
		double x1 = (planePara[j].coord)[0].x();
		double y1 = (planePara[j].coord)[0].y();
		double z1 = (planePara[j].coord)[0].z();
		//顶点2
		double x2 = (planePara[j].coord)[1].x();
		double y2 = (planePara[j].coord)[1].y();
		double z2 = (planePara[j].coord)[1].z();
		//顶点3
		double x3 = (planePara[j].coord)[2].x();
		double y3 = (planePara[j].coord)[2].y();
		double z3 = (planePara[j].coord)[2].z();
		//顶点4
		double x4 = (planePara[j].coord)[3].x();
		double y4 = (planePara[j].coord)[3].y();
		double z4 = (planePara[j].coord)[3].z();
		float angle = planePara[j].angle;
		double err = planePara[j].err;
		std::string str(planePara[j].normal);
		char* insertSql = sqlite3_mprintf("insert into Wall(a, b, c, d, x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4,angle,err,normal)values('%lf', '%lf', '%lf', '%lf', '%lf', '%lf', '%lf', '%lf', '%lf', '%lf', '%lf', '%lf', '%lf', '%lf', '%lf', '%lf', '%f', '%lf', '%s')", a, b, c,d,x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4,angle,err,str.c_str());
		iSqliteDatabase->InsertRecord(db, insertSql);
	}
	iSqliteDatabase->Commit(db);
	iSqliteDatabase->CloseDatabase(db);
}

void COsgExtractWall::clearWallDataInSqlite3(InterfaceSqliteDatabase* iSqliteDatabase, char* dbname)
{
	sqlite3* db = iSqliteDatabase->OpenDatabase(dbname);
	iSqliteDatabase->ClearAllRecord(db, "delete from Wall");
	iSqliteDatabase->Commit(db);
	iSqliteDatabase->CloseDatabase(db);
}

void COsgExtractWall::readWallDataFromSqlite3(std::vector<WallInfo>& planePara, InterfaceSqliteDatabase* iSqliteDatabase, char* dbname)
{
	planePara.clear();
	sqlite3* db = iSqliteDatabase->OpenDatabase(dbname);
	char *errmsg = NULL;
	const char* strSql1 = "select * from Wall";
	char **dbResult = NULL;
	int nRow;
	int nCol;
	int result = sqlite3_get_table(db, strSql1, &dbResult, &nRow, &nCol, &errmsg);
	if (nRow < 1)
	{
		return;
	}
	
	for (int i = 0; i < nRow; i++)
	{
		WallInfo wall;
		wall.pl = new PLane();
		wall.pl->A = atof(dbResult[(i + 1) * nCol + 0]);
		wall.pl->B = atof(dbResult[(i + 1) * nCol + 1]);
		wall.pl->C = atof(dbResult[(i + 1) * nCol + 2]);
		wall.pl->D = atof(dbResult[(i + 1) * nCol + 3]);
		
		wall.coord.push_back(osg::Vec3d(atof(dbResult[(i + 1) * nCol + 4]), atof(dbResult[(i + 1) * nCol + 5]), atof(dbResult[(i + 1) * nCol + 6])));
		wall.coord.push_back(osg::Vec3d(atof(dbResult[(i + 1) * nCol + 7]), atof(dbResult[(i + 1) * nCol + 8]), atof(dbResult[(i + 1) * nCol + 9])));
		wall.coord.push_back(osg::Vec3d(atof(dbResult[(i + 1) * nCol + 10]), atof(dbResult[(i + 1) * nCol + 11]), atof(dbResult[(i + 1) * nCol + 12])));
		wall.coord.push_back(osg::Vec3d(atof(dbResult[(i + 1) * nCol + 13]), atof(dbResult[(i + 1) * nCol + 14]), atof(dbResult[(i + 1) * nCol + 15])));

		wall.angle = atof(dbResult[(i + 1) * nCol + 16]);
		wall.err = atof(dbResult[(i + 1) * nCol + 17]);
		std::string str(dbResult[(i + 1) * nCol + 18]);
		wall.normal = str;
		planePara.push_back(wall);
	}
}

void COsgExtractWall::readPointDataFromSqlite3(std::vector<osg::Vec3d>& vecTotalTriangle, InterfaceSqliteDatabase* iSqliteDatabase, char* tablename, char* dbname)
{
	vecTotalTriangle.clear();
	sqlite3* db = iSqliteDatabase->OpenDatabase(dbname);
	char *errmsg = NULL;
	char strSql1[99];
	if (strcmp(tablename, "Points") == 0)
	{
		strcpy(strSql1, "select * from Points");
	}
	if (strcmp(tablename, "Normal") == 0)
	{
		strcpy(strSql1, "select * from Normal");
	}

	char **dbResult = NULL;
	int nRow;
	int nCol;
	int result = sqlite3_get_table(db, strSql1, &dbResult, &nRow, &nCol, &errmsg);
	if (nRow < 1)
	{
		return;
	}
	double val[3];
	for (int i = 0; i < nRow; i++)
	{
		for (int j = 0; j < nCol; j++)
		{
			val[j] = atof(dbResult[(i + 1) * nCol + j]);
		}
		osg::Vec3d p(val[0], val[1], val[2]);
		vecTotalTriangle.push_back(p);
	}
}

bool COsgExtractWall::DrawTriangle(osg::Group* mRoot,osg::NodePath np, osg::Vec3d worldPt, osg::Matrix worldMatrix, std::vector<osg::Vec3d> &vecTriangle, osg::Vec3d normal,float ang,double colorErr)	//画三角形
{
	CSceneExtractWall sceneExtractWall;
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);

	//查找底层数据
	std::vector<std::string> vecChildFileName;

	for (int i = 0; i <= np.size() - 1; i++)
	{
		osg::ref_ptr<osg::PagedLOD> lod = dynamic_cast<osg::PagedLOD*> (np[i]);
		if (lod)
		{
			IFinder->FindLodBottomLevelFile(lod, vecChildFileName);
			break;
		}
	}
	if (vecChildFileName.size() == 0)
	{
		return false;
	}
	//获得鼠标点击的面片和起始搜索范围
	std::vector<osg::Vec3d> vecIndex;
	osg::Vec4 color;//保存起始点的颜色
	if (!sceneExtractWall.getTriangleSelect(vecChildFileName, color,worldMatrix, worldPt, vecIndex, vecTriangle))
	{
		return false;
	}

	for (int i = 0; i < vecChildFileName.size(); ++i)
	{
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(vecChildFileName[i]);

		if (!node)
		{
			continue;
		}

		InterfaceOsgFinderFactory iOsgFinderFactory;
		InterfaceOsgFinder* iOsgFinder = iOsgFinderFactory.create();
		std::vector<osg::Geode*> vecGeode = iOsgFinder->FindAndGetGeode(node);

		for (size_t i = 0; i < vecGeode.size(); ++i)
		{
			sceneExtractWall.getTrianglePoints(vecGeode[i], worldMatrix, vecTriangle, vecIndex,color,colorErr, normal, ang);
		}
	}

	return true;
}

bool COsgExtractWall::DrawTriangleEx(osg::Group* mRoot, osg::NodePath np, osg::Vec3d worldPt, osg::Matrix worldMatrix, std::vector<osg::Vec3d> &vecTriangle, std::vector<osg::Vec3d> & vecNormal, std::vector<osg::Vec4> & vecColor, float ang, double colorErr)	//画三角形
{
	CSceneExtractWall sceneExtractWall;
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);

	//查找底层数据
	std::vector<std::string> vecChildFileName;

	for (int i = 0; i <= np.size() - 1; i++)
	{
		osg::ref_ptr<osg::PagedLOD> lod = dynamic_cast<osg::PagedLOD*> (np[i]);
		if (lod)
		{
			IFinder->FindLodBottomLevelFile(lod, vecChildFileName);
			break;
		}
	}
	if (vecChildFileName.size() == 0)
	{
		vecNormal.pop_back();
		return false;
	}
	//获得鼠标点击的面片和起始搜索范围
	std::vector<osg::Vec3d> vecIndex;
	osg::Vec4 color;//保存起始点的颜色
	if (!sceneExtractWall.getTriangleSelect(vecChildFileName, color, worldMatrix, worldPt, vecIndex, vecTriangle))
	{
		vecNormal.pop_back();
		return false;
	}
	vecColor.push_back(color);
	for (int i = 0; i < vecChildFileName.size(); ++i)
	{
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(vecChildFileName[i]);

		if (!node)
		{
			continue;
		}

		InterfaceOsgFinderFactory iOsgFinderFactory;
		InterfaceOsgFinder* iOsgFinder = iOsgFinderFactory.create();
		std::vector<osg::Geode*> vecGeode = iOsgFinder->FindAndGetGeode(node);

		for (size_t i = 0; i < vecGeode.size(); ++i)
		{
			sceneExtractWall.getTrianglePointsEx(vecGeode[i], worldMatrix, vecTriangle, vecIndex, vecColor, colorErr, vecNormal, ang);
		}
	}

	return true;
}

bool COsgExtractWall::fittingWallSpace(vector<osg::Vec3d> &vecPt,std::vector<WallInfo>& planePara,osg::Vec3d& normal, osg::Matrix& worldMatrix)
{
	CSceneExtractWall sceneExtractWall;
	if (vecPt.empty())
	{
		//iSigEventCallBack->EmitSigShowOpsHint("找不到采样信息");
		return false;
	}
	
	//提取真实平面
	PLane* plane = new PLane();
	

	CVPlane(vecPt, plane->A, plane->B, plane->C, plane->D);

	double minz = 0, maxz = 0,minx = 0,maxx = 0,miny = 0,maxy = 0;
	osg::Vec3d minxy, maxxy;
	osg::Vec3d p0, p1, p2, p3, p4;
	osg::Vec3d ptMaxxy, ptMinxy;
	std::vector<osg::Vec3d> coord;
	coord.clear();
	if (sceneExtractWall.FindMinMax(vecPt, ptMinxy, ptMaxxy, minz, maxz,minx,maxx,miny,maxy))
	{
		ptMinxy.z() = minz;
		ptMaxxy.z() = maxz;
		sceneExtractWall.getPointProject(ptMinxy, minxy, *plane);
		sceneExtractWall.getPointProject(ptMaxxy, maxxy, *plane);
		sceneExtractWall.getCoordBy2Point(minxy, maxxy, coord, *plane);
	}
	else
	{
		return false;
	}

	double err = 0;
	for (size_t i = 0; i < vecPt.size(); ++i)
	{
		err += pow(sceneExtractWall.getDisFromPointToPlane(vecPt[i], *plane), 2);
	}
	err /= vecPt.size();//墙面拟合误差
	err = sqrt(err);
	float angle = CL::getAngleForVector(osg::Vec3d(plane->A, plane->B, plane->C), osg::Vec3d(plane->A, plane->B, 0));
	
	//保存墙面信息
	double a = plane->A, b = plane->B, c = plane->C;
	if (plane->A * normal.x() < 0)
	{
		a = -plane->A;
	}
	if (plane->B * normal.y() < 0)
	{
		b = -plane->B;
	}
	if (plane->C * normal.z() < 0)
	{
		c = -plane->C;
	}
	char strMsg[99];
	sprintf(strMsg, "%f:%f:%f", a, b, c);
	string str(strMsg);
	
	WallInfo wall;
	wall.pl = plane;
	wall.coord.clear();
	wall.coord.assign(coord.begin(), coord.end());
	wall.angle = angle;
	wall.err = err;
	wall.normal = str;
	wall.minH = minz;
	wall.maxH = maxz;
	wall.minx = minx;
	wall.maxx = maxx;
	wall.miny = miny;
	wall.maxy = maxy;
	(planePara).push_back(wall);

	//vecPt.clear();
	return true;
}

void COsgExtractWall::addDraggerToScene(osg::Group* mRoot, osg::Group* slotGroup, osg::Group* editWallGroup)
{
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	bool hasScope = false;
	int num = mRoot->getNumChildren();
	osg::ref_ptr<osg::Node> childNode;
	for (int i = 0; i < num; i++)
	{
		childNode = mRoot->getChild(i);
		if (childNode->getName() == "ScopeGroup")
		{
			//hasScope = true;
			mRoot->removeChild(i);
			break;
		}
	}
	if (!hasScope)
	{
		IDrawer->AddDraggerToScene(mRoot, slotGroup, editWallGroup); //添加拖动事件		
	}
}

void COsgExtractWall::getWorldMatrix(osg::Group* group, osg::Matrix& worldMatrix)
{
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	std::vector<osg::MatrixTransform*> vecTrans;

	if (group->getNumChildren() > 0)
	{
		IFinder->FindTransNode(group, vecTrans);
		worldMatrix = vecTrans[0]->getMatrix();
	}
}


bool COsgExtractWall::draggerEvent(bool& atWall, const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osgViewer::Viewer* mViewer, CDraggerPoint* _activeDragger, osg::Group* editWallGroup, osg::Group* tempGroup, osg::Vec3d& partnerPoint, WallInfo& wallSelected, osgManipulator::PointerInfo& _pointer, osg::Vec3d& endPoint, std::vector<WallInfo>& planePara, osg::Matrix& worldMatrix, osg::Vec4d& color)
{
	CSceneExtractWall sceneExtrctWall;
	if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE)//释放鼠标
	{
		if (atWall == true)
		{
			//关闭拖动事件
			osg::Vec3d p0;
			sceneExtrctWall.getPointProject(_activeDragger->getPoint(), p0, *(wallSelected.pl));
			std::vector<osg::Vec3d> vecCoord;
			sceneExtrctWall.redrawWall(p0, partnerPoint, vecCoord,wallSelected,color, editWallGroup,tempGroup, worldMatrix);
			wallSelected.coord = vecCoord;
			(planePara).push_back(wallSelected);
			atWall = false;
			_activeDragger = 0;

			//清除tempGroup
			sceneExtrctWall.clearGroupResult(tempGroup);
			return true;
		}
	}
	if (ea.getEventType() == osgGA::GUIEventAdapter::DRAG)//拖动鼠标
	{
		if (_activeDragger && atWall)
		{
			_pointer._hitIter = _pointer._hitList.begin();
			_pointer.setCamera(mViewer->getCamera());
			_pointer.setMousePosition(ea.getX(), ea.getY());
			_activeDragger->handle(_pointer, ea, aa);
			endPoint = _activeDragger->getPoint();
			osg::Vec3d p0;
			std::vector<osg::Vec3d> vecCoord;
			sceneExtrctWall.getPointProject(endPoint, p0, *(wallSelected.pl));//endPoint非墙面上的点，做投影
			sceneExtrctWall.redrawWall(p0, partnerPoint, vecCoord,wallSelected,color, tempGroup,tempGroup, worldMatrix);
		}
	}
	return false;

}

bool COsgExtractWall::wallInsect(osg::Group* mapperGroup, osg::Group* tempGroup,osg::Vec3d &worldPt, std::vector<WallInfo>& planePara, std::vector<WallInfo>& planeSelect, osg::Vec3d& ptInsect,std::vector<int>& vecWallIndex ,osg::Vec4d& color,osg::Matrix& worldMatrix, InterfaceSigEventCallBack* iSigEventCallBack)
{
	if ((planePara).empty())
	{
		return false;
	}
	osg::Vec3d partnerPoint;
	for (size_t i = 0; i < (planePara).size(); ++i)
	{
		if (CL::Point_In_Rectangle_3D(worldPt, partnerPoint, (planePara)[i].coord))
		{

			planeSelect.push_back((planePara)[i]);
			InterfaceBaseDrawerFactory IDrawerFactory;
			InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
			IDrawer->BaseDrawPolygon(planePara[i].coord, color, tempGroup, worldMatrix);
			char strMsg[99];
			sprintf(strMsg, "墙面信息：倾斜角度：%f °，墙面拟合误差：%f ", (planePara)[i].angle, (planePara)[i].err);
			iSigEventCallBack->EmitSigShowOpsHint(strMsg);
			
			if (planeSelect.size() == 3)
			{
				if ( (planeSelect[0].coord == planeSelect[1].coord ) ||
					 (planeSelect[0].coord == planeSelect[2].coord ) ||
					 (planeSelect[2].coord == planeSelect[1].coord ) )
				{
					planeSelect.clear();
					return true;
				}
				GetInsectof3Plane(planeSelect[0],planeSelect[1],planeSelect[2],ptInsect);
				IDrawer->BaseDrawPtOperate(ptInsect, osg::Vec4(1, 0, 0, 1), 4.5, mapperGroup, worldMatrix);
				for (int j = 0;j<planePara.size();++j)
				{
					if (planePara[j].coord == planeSelect[0].coord ||
						planePara[j].coord == planeSelect[1].coord ||
						planePara[j].coord == planeSelect[2].coord)
					{
						vecWallIndex.push_back(j);
					}
				}
				planeSelect.clear();
				return true;
			}
			
		}
	}
	return false;
}

void COsgExtractWall::lineInsectOprate(std::vector<LineInfo>& vecLineforInsect, std::vector<osg::Vec3d>& vecCoord, osg::Vec3d& worldPt, osg::Vec3d& PointInLine1, osg::Vec3d& PointInLine2, char* transName1,char *transName2, osg::ref_ptr<osg::MatrixTransform>& selectTrans1, osg::ref_ptr<osg::MatrixTransform>& selectTrans2, osg::Group* mapperGroup,osg::Group* tempGroup, osg::Matrix worldMatrix, InterfaceSigEventCallBack* iSigEventCallBack)
{
	if (vecCoord.size() > 1)
	{
		return;
	}
	vecCoord.push_back(worldPt);

	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	std::vector<osg::MatrixTransform*> vecTrans;
	IFinder->FindTransNode(mapperGroup, vecTrans);
	osg::Vec3d pt_redraw1, pt_redraw2;

	double dis = 0.1;
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

		double k;
		if ((k = CL::getDisFromPointToLine(worldPt, pt1, pt2)) < dis)
		{
			dis = k;
			pt_redraw1 = pt1;
			pt_redraw2 = pt2;
			string s1;
			if (vecCoord.size() == 1)
			{
				selectTrans1 = mTrans;
				s1 = mTrans->getParent(0)->getName();
				transName1 = new char[99];
				strcpy(transName1, s1.c_str());

			}
			else
			{
				selectTrans2 = mTrans;
				s1 = mTrans->getParent(0)->getName();
				transName2 = new char[99];
				strcpy(transName2, s1.c_str());
			}
		}
	}
	if (dis == 0.1)
	{
		vecCoord.pop_back();
		return;
	}
	if (distance2(worldPt, pt_redraw1) < distance2(worldPt, pt_redraw2))
	{
		IDrawer->BaseDrawPtOperate(pt_redraw1, osg::Vec4d(0,0,1,1), 4.5, tempGroup, worldMatrix);
		if (vecCoord.size() == 1)
		{
			PointInLine1 = pt_redraw1;
		}
		else
		{
			PointInLine2 = pt_redraw1;
		}
	}
	else
	{
		IDrawer->BaseDrawPtOperate(pt_redraw2, osg::Vec4d(0, 0, 1, 1), 4.5, tempGroup, worldMatrix);
		if (vecCoord.size() == 1)
		{
			PointInLine1 = pt_redraw2;
		}
		else
		{
			PointInLine2 = pt_redraw2;
		}
	}
	if (vecCoord.size() == 1)//选择第一条线时清空vecline
	{
		vecLineforInsect.clear();
	}
	vecLineforInsect.push_back(LineInfo(pt_redraw1, pt_redraw2));
	if (vecLineforInsect.size() == 2)
	{
		iSigEventCallBack->EmitSigShowOpsHint("按Tab键切换端点，按Enter键求交，按Backspace键撤销");
	}
}

void COsgExtractWall::calcIntersectOfLine(vector<LineInfo>& vecLineforInsect, std::vector<osg::Vec3d>& vecCoord, osg::Group* mapperGroup,osg::Group* tempGroup, osg::Vec3d& PointInLine1, osg::Vec3d& PointInLine2, char* transName1, char *transName2, osg::ref_ptr<osg::MatrixTransform>& selectTrans1, osg::ref_ptr<osg::MatrixTransform>& selectTrans2,osg::Matrix worldMatrix)
{
	if (vecLineforInsect.size() != 2)
	{
		return;
	}
	CSceneExtractWall sceneExtractWall;
	sceneExtractWall.clearGroupResult(tempGroup);
	osg::ref_ptr<osg::Group> parent1 = selectTrans1->getParent(0);
	osg::ref_ptr<osg::Group> parent2 = selectTrans2->getParent(0);
	if (parent1)
	{
		mapperGroup->removeChild(parent1);
	}
	if (parent2)
	{
		mapperGroup->removeChild(parent2);
	}

	double k1 = (vecLineforInsect[0].p2.x() - vecLineforInsect[0].p1.x()) / (vecLineforInsect[0].p2.y() - vecLineforInsect[0].p1.y());
	double b1 = -k1*vecLineforInsect[0].p1.y() + vecLineforInsect[0].p1.x();

	double k2 = (vecLineforInsect[1].p2.x() - vecLineforInsect[1].p1.x()) / (vecLineforInsect[1].p2.y() - vecLineforInsect[1].p1.y());
	double b2 = -k2*vecLineforInsect[1].p1.y() + vecLineforInsect[1].p1.x();

	double y = (b2 - b1) / (k1 - k2);
	double x = k1*y + b1;

	k1 = (vecLineforInsect[0].p2.x() - vecLineforInsect[0].p1.x()) / (vecLineforInsect[0].p2.z() - vecLineforInsect[0].p1.z());
	b1 = -k1*vecLineforInsect[0].p1.z() + vecLineforInsect[0].p1.x();

	k2 = (vecLineforInsect[1].p2.x() - vecLineforInsect[1].p1.x()) / (vecLineforInsect[1].p2.z() - vecLineforInsect[1].p1.z());
	b2 = -k2*vecLineforInsect[1].p1.z() + vecLineforInsect[1].p1.x();
	double z = (b2 - b1) / (k1 - k2);

	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	osg::ref_ptr<osg::Group> group1 = new osg::Group;
	if (vecLineforInsect[0].p1 == PointInLine1)
	{
		IDrawer->BaseDrawLineOperate(vecLineforInsect[0].p2, osg::Vec3d(x, y, z), osg::Vec4d(1,0,0,1), 3.5, group1, worldMatrix);
		group1->setName(transName1);
		mapperGroup->addChild(group1);
	}
	if (vecLineforInsect[0].p2 == PointInLine1)
	{
		IDrawer->BaseDrawLineOperate(vecLineforInsect[0].p1, osg::Vec3d(x, y, z), osg::Vec4d(1,0,0,1), 3.5, group1, worldMatrix);
		group1->setName(transName1);
		mapperGroup->addChild(group1);
	}
	osg::ref_ptr<osg::Group> group2 = new osg::Group;
	if (vecLineforInsect[1].p1 == PointInLine2)
	{
		IDrawer->BaseDrawLineOperate(vecLineforInsect[1].p2, osg::Vec3d(x, y, z), osg::Vec4d(1, 0, 0, 1), 3.5, group2, worldMatrix);
		group2->setName(transName2);
		mapperGroup->addChild(group2);
	}
	if (vecLineforInsect[1].p2 == PointInLine2)
	{
		IDrawer->BaseDrawLineOperate(vecLineforInsect[1].p1, osg::Vec3d(x, y, z), osg::Vec4d(1, 0, 0, 1), 3.5, group2, worldMatrix);
		group2->setName(transName2);
		mapperGroup->addChild(group2);
	}
	vecCoord.clear();
}

void COsgExtractWall::combineGroup(int& n, vector<osg::Vec3d>& vecCoord, std::vector<LineForCombine>& vecLFC, osg::Group* mapperGroup, osg::Vec4d& color,osg::Matrix& worldMatrix, InterfaceSigEventCallBack* iSigEventCallBack)
{
	InterfaceMFCExcuteFactory IExcuteFactory;
	InterfaceMFCExcute* IExcute = IExcuteFactory.create();
	//按所在墙面组合
	if (vecCoord.size() < 3)
	{
		IExcute->PopUpMessage("组合范围错误");
		return;
	}

	int totalNum = vecCoord.size();
	std::vector<CL::Vec2d> pol;

	for (int i = 0; i < totalNum; i++)
	{
		pol.push_back(CL::Vec2d(vecCoord[i].x(), vecCoord[i].y()));
	}

	pol.push_back(CL::Vec2d(vecCoord[0].x(), vecCoord[0].y()));

	if (pol.size() == 0)
	{
		IExcute->PopUpMessage("组合范围错误");
		return;
	}
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	std::vector<osg::MatrixTransform*> vecTrans;
	IFinder->FindTransNode(mapperGroup, vecTrans);

	std::vector<string> vecWall;


	//第一步找到所有在范围内的直线

	vecLFC.clear();
	for (int i = 0; i < vecTrans.size(); i++)
	{
		osg::ref_ptr<osg::MatrixTransform> mTrans = vecTrans[i];
		bool isInPolygon = false;

		if (mTrans)
		{
			osg::ref_ptr<osg::Vec3Array> extracted_verts = new osg::Vec3Array;
			IFinder->ExtractVerts(mTrans, extracted_verts);
			osg::Matrix matrix = mTrans->getMatrix();

			if (extracted_verts->size() < 2)
			{
				continue;
			}

			for (int j = 0; j < extracted_verts->size(); j++)
			{
				osg::Vec3d pt(extracted_verts->at(j).x(), extracted_verts->at(j).y(), extracted_verts->at(j).z());
				pt = pt * matrix;

				if (Point_In_Polygon_2D(pt.x(), pt.y(), pol))
				{
					int k = extracted_verts->size() - 1;
					string name = mTrans->getParent(0)->getName();
					LineForCombine lc;
					lc.name = name;
					lc.p1 = osg::Vec3d(extracted_verts->at(0).x(), extracted_verts->at(0).y(), extracted_verts->at(0).z())*matrix;
					lc.p2 = osg::Vec3d(extracted_verts->at(k).x(), extracted_verts->at(k).y(), extracted_verts->at(k).z())*matrix;
					vecLFC.push_back(lc);
					//删除线
					osg::ref_ptr<osg::Group> parent = mTrans->getParent(0);
					if (parent)
					{
						mapperGroup->removeChild(parent);
					}
					break;
				}
			}
		}
	}
	//第二步，查找所有涉及的面
	getAllWall(vecLFC, vecWall);
	//第三步，遍历面，找到面上的线组合
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	n = 0;
	for (size_t i = 0; i < vecWall.size(); ++i)
	{
		osg::ref_ptr<osg::Group> wallGroup = new osg::Group;
		for (std::vector<LineForCombine>::iterator it = vecLFC.begin(); it != vecLFC.end(); ++it)
		{
			char name[99];
			strcpy(name, (it->name).c_str());
			if (lineInWall(vecWall[i], name))
			{
				IDrawer->BaseDrawLineOperate(it->p1, it->p2, color, 3.5, wallGroup, worldMatrix);
			}
		}
		if (wallGroup->getNumChildren() > 0)
		{
			char name[99];
			strcpy(name, vecWall[i].c_str());
			wallGroup->setName(name);
			mapperGroup->addChild(wallGroup);
			n++;
		}
	}
	vecCoord.clear();
	iSigEventCallBack->EmitSigShowOpsHint("墙面组合：已组合,按Z键可撤销组合结果");
}

void COsgExtractWall::drawHorizLineOprate(bool &islineStart,double& horiz_high,osg::Vec3d& worldPt, std::vector<LineInfo>& vecLine, std::vector<char*>& vecName, osg::Matrix worldMatrix, vector<osg::Vec3d>& vecCoord, osg::Group* tempGroup, osg::Group* mapperGroup, InterfaceSigEventCallBack* iSigEventCallBack)
{
	int n = mapperGroup->getNumChildren();
	if (n == 0)
	{
		return;
	}
	double dis = 0.1;
	//没有距离小于0.1的线
	if (!islineStart)
	{
		worldPt.z() = horiz_high;
	}
	if (!getEndPoints(islineStart,worldPt,vecLine,vecName,vecCoord, mapperGroup, worldMatrix, dis))
	{
		return;
	}
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	if (islineStart)
	{
		vecCoord.clear();
		//清空vecline
		LineInfo line(vecLine.back().p1, vecLine.back().p2);
		vecLine.clear();
		vecLine.push_back(line);
		horiz_high = worldPt.z();
		IDrawer->BaseDrawPtOperate(worldPt, osg::Vec4d(0,0,1,1), 4.5, tempGroup, worldMatrix);
		islineStart = false;
		vecCoord.push_back(worldPt);
		iSigEventCallBack->EmitSigShowOpsHint("绘制水平线：选择线段绘制水平线，按Backspace键撤销");
		return;
	}
	CSceneExtractWall sceneExtractWall;
	sceneExtractWall.clearGroupResult(tempGroup);
	
	osg::ref_ptr<osg::Group> group = new osg::Group;
	IDrawer->BaseDrawLineOperate(worldPt, vecCoord.back(), osg::Vec4d(1,0,0,1), 3.5, group, worldMatrix);
	//找出共有面
	string s1, s2, s3, s4;
	analysisName(vecName.back(), s1, s2);
	analysisName(vecName[vecName.size() - 2], s3, s4);
	char name[99];
	if (s1 == s3 || s1 == s4)
	{
		strcpy(name, s1.c_str());
		group->setName(name);
	}
	else
	{
		strcpy(name, s2.c_str());
		group->setName(name);
	}
	mapperGroup->addChild(group);

	vecCoord.push_back(worldPt);
	if (distance2(worldPt, vecCoord.front()) <= 0.00001)
	{
		iSigEventCallBack->EmitSigShowOpsHint("绘制水平线：水平线已闭合，此次绘制结束");
		// 可继续下一次绘制
		islineStart = true;
	}
	return;
}

void COsgExtractWall::deleteHorizLine(std::vector<char*>& vecName,osg::Group* mapperGroup, osg::Group* tempGroup, std::vector<LineInfo>& vecLine, vector<osg::Vec3d>& vecCoord, bool& islineStart, osg::Matrix worldMatrix, InterfaceSigEventCallBack* iSigEventCallBack)
{
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	if (vecLine.size() == 0)
	{
		int num = mapperGroup->getNumChildren();
		if (num > 0)
		{
			mapperGroup->removeChild(num - 1);
		}
		return;
	}
	if (vecLine.size() == 1)//只绘制了一个起始点
	{
		int num = mapperGroup->getNumChildren();
		mapperGroup->removeChild(num - 1);
		osg::ref_ptr<osg::Group> group = new osg::Group;
		IDrawer->BaseDrawLineOperate(vecLine[0].p1, vecLine[0].p2, osg::Vec4d(1,0,0,1), 3.5, group, worldMatrix);
		group->setName(vecName[0]);
		mapperGroup->addChild(group);
		vecLine.clear();
		CSceneExtractWall sceneExtractWall;
		sceneExtractWall.clearGroupResult(tempGroup);
	
		islineStart = true;
		vecCoord.clear();
		iSigEventCallBack->EmitSigShowOpsHint("绘制水平线：请选定水平线绘制起点");
	}
	if (vecLine.size() > 1)
	{
		if (distance2(vecCoord.front(), vecCoord.back()) <= 0.00001)//已闭合
		{
			for (size_t i = 1; i < vecCoord.size(); ++i)
			{
				int num = mapperGroup->getNumChildren();
				mapperGroup->removeChild(num - 1);
				mapperGroup->removeChild(num - 2);
			}

			for (size_t i = 0; i < vecLine.size() - 1; i++)
			{
				osg::ref_ptr<osg::Group> group = new osg::Group;
				IDrawer->BaseDrawLineOperate(vecLine[i].p1, vecLine[i].p2, osg::Vec4d(1, 0, 0, 1), 3.5, group, worldMatrix);
				group->setName(vecName[i]);
				mapperGroup->addChild(group);
			}
		}
		else
		{
			for (size_t i = 1; i < vecCoord.size(); ++i)
			{
				int num = mapperGroup->getNumChildren();
				mapperGroup->removeChild(num - 1);
				mapperGroup->removeChild(num - 2);
			}
			int num = mapperGroup->getNumChildren();
			mapperGroup->removeChild(num - 1);
			for (size_t i = 0; i < vecLine.size(); i++)
			{
				osg::ref_ptr<osg::Group> group = new osg::Group;
				IDrawer->BaseDrawLineOperate(vecLine[i].p1, vecLine[i].p2, osg::Vec4d(1, 0, 0, 1), 3.5, group, worldMatrix);
				group->setName(vecName[i]);
				mapperGroup->addChild(group);
			}
		}
		vecLine.clear();
		islineStart = true;
		vecCoord.clear();
		iSigEventCallBack->EmitSigShowOpsHint("绘制水平线：请选定水平线绘制起点");
	}
}

void COsgExtractWall::getAllWall(vector<LineForCombine>& vecLFC, vector<string> &vecWall)
{
	for (size_t i = 0; i < vecLFC.size(); ++i)
	{
		if (vecLFC[i].name == "")
		{
			continue;
		}
		std::string s1, s2;
		char *data;
		int len = (vecLFC[i].name).length();
		data = (char *)malloc((len + 1) * sizeof(char));
		(vecLFC[i].name).copy(data, len, 0);
		analysisName(data, s1, s2);
		if (s1 != " ")
		{
			vecWall.push_back(s1);
		}
		if (s2 != " ")
		{
			vecWall.push_back(s2);
		}
	}
	sort(vecWall.begin(), vecWall.end());
	std::vector<string>::iterator pos;
	pos = unique(vecWall.begin(), vecWall.end());
	vecWall.erase(pos, vecWall.end());
}

void COsgExtractWall::analysisName(char* name, string& str1, string& str2)
{
	std::string str(name);
	int i = str.find(",");
	if (i == std::string::npos)
	{
		int j = str.find(":");
		if (j == std::string::npos)
		{
			str1 = " ";
			str2 = " ";
		}
		else
		{
			str1 = str;
			str2 = " ";
		}
	}
	else
	{
		str1 = str.substr(0, i);
		str2 = str.substr(i + 1);
	}
}

bool COsgExtractWall::lineInWall(string& d, char* s)
{
	string d1, d2;
	analysisName(s, d1, d2);

	if (d == d1 || d == d2)
	{
		return true;
	}
	return false;
}

bool COsgExtractWall::getEndPoints(bool &islineStart, osg::Vec3d& pt, std::vector<LineInfo>& vecLine, std::vector<char*>& vecName, vector<osg::Vec3d>& vecCoord,osg::Group* MapperGroup, osg::Matrix worldMatrix, double  dis)
{
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	std::vector<osg::MatrixTransform*> vecTrans;
	IFinder->FindTransNode(MapperGroup, vecTrans);
	bool flag = false;
	osg::Vec3d pOut, pt_redraw1, pt_redraw2;
	osg::ref_ptr<osg::MatrixTransform> selectTrans = NULL;
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
		if (pt1.z() == pt2.z())
		{
			continue;
		}
		double k;
		if ((k =CL::getDisFromPointToLine(pt, pt1, pt2)) < dis)
		{
			getHorizPoint(pt, pOut, pt1, pt2);
			dis = k;
			flag = true;
			selectTrans = mTrans;
			pt_redraw1 = pt1;
			pt_redraw2 = pt2;
		}
	}
	if (flag)
	{
		if (islineStart)
		{
			vecName.clear();
		}
		if (!vecCoord.empty())//重复选点  退出
		{
			if (pOut == vecCoord.back())
			{
				return false;
			}
		}
		LineInfo line(pt_redraw1, pt_redraw2);
		vecLine.push_back(line);
		pt = pOut;
		//所选线段名字
		char* buf = new char[99];
		osg::ref_ptr<osg::Group> parent = selectTrans->getParent(0);
		strcpy(buf, (parent->getName()).c_str());
		if (parent)
		{
			MapperGroup->removeChild(parent);
		}
		InterfaceBaseDrawerFactory IDrawerFactory;
		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
		osg::ref_ptr<osg::Group> group = new osg::Group;
		if (distance2(pt_redraw1, pOut) < distance2(pt_redraw2, pOut))
		{
			IDrawer->BaseDrawLineOperate(pOut, pt_redraw2, osg::Vec4d(1,0,0,1), 3.5, group, worldMatrix);
			group->setName(buf);
			MapperGroup->addChild(group);
		}
		else
		{
			IDrawer->BaseDrawLineOperate(pOut, pt_redraw1, osg::Vec4d(1, 0, 0, 1), 3.5, group, worldMatrix);
			group->setName(buf);
			MapperGroup->addChild(group);
		}
		vecName.push_back(buf);
		return true;
	}
	return false;
}

void COsgExtractWall::getHorizPoint(osg::Vec3d& ptIn, osg::Vec3d& ptOut, osg::Vec3d& p1, osg::Vec3d& p2)
{
	double t = (ptIn.z() - p1.z()) / (p2.z() - p1.z());
	ptOut.x() = p1.x() + t*(p2.x() - p1.x());
	ptOut.y() = p1.y() + t*(p2.y() - p1.y());
	ptOut.z() = ptIn.z();
}



bool COsgExtractWall::FindRelationInVector(CTriangle& triangle, std::vector<CTriangle>& neighbor, std::vector<CTriangleRelation>& vecTriangleRelation)
{
	std::vector<CTriangleRelation>::iterator ite = find_if(vecTriangleRelation.begin(), vecTriangleRelation.end(), findTriangle(triangle));
	if (ite == vecTriangleRelation.end())
	{
		return false;
	}
	neighbor.clear();
	neighbor.assign((*ite).neighbor.begin(), (*ite).neighbor.end());
	return true;
}

bool COsgExtractWall::findPointInVector(osg::Vec3d& pt, std::vector<osg::Vec3d>& vecPt)
{
	std::vector<osg::Vec3d>::iterator ite = find(vecPt.begin(), vecPt.end(), pt);
	if (ite != vecPt.end())
	{
		return true;
	}
	return false;
}





void COsgExtractWall::FindTriangleRelation(osg::ref_ptr<osg::Node> node, std::vector<CTriangleRelation>& vecTriangleRelation, osg::Matrix worldMatrix)
{
	InterfaceOsgFinderFactory IOsgFinderFactory;
	InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();
	std::vector<osg::Geode*> vecGeode = IOsgFinder->FindAndGetGeode(node);
	//保存三角
	std::vector<CTriangle> vecTriangle;
	vecTriangle.clear();

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
			//保存颜色
			osg::ref_ptr<osg::Texture2D> texture = dynamic_cast<osg::Texture2D*> (geom->getOrCreateStateSet()->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
			osg::ref_ptr<osg::Vec2Array> textureArray = dynamic_cast<osg::Vec2Array*> (geom->getTexCoordArray(0));

			if (!texture)
			{
				continue;
			}

			osg::ref_ptr<osg::Image> image = texture->getImage(0);
			int imageWidth = image->s();
			int imageHeight = image->t();
			osg::Vec2d texture1, texture2, texture3;//三个顶点的纹理坐标
			
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

					texture1.x() = textureArray->at(index1).x() * imageWidth;
					texture1.y() = textureArray->at(index1).y() * imageHeight;
					texture2.x() = textureArray->at(index2).x() * imageWidth;
					texture2.y() = textureArray->at(index2).y() * imageHeight;
					texture3.x() = textureArray->at(index3).x() * imageWidth;
					texture3.y() = textureArray->at(index3).y() * imageHeight;
					std::vector<osg::Vec2d> vecCoord;
					CSceneExtractWall sceneExtractWall;
					sceneExtractWall.getTextureCoordBy3Points(texture1, texture2, texture3, vecCoord);
					unsigned char* data = (unsigned char*)(image->getDataPointer());
					int R = 0, G = 0, B = 0;
					for (size_t i = 0; i < vecCoord.size(); ++i)
					{
						R += (int)data[((int)vecCoord[i].y()) * imageWidth * 3 + ((int)vecCoord[i].x()) * 3 + 0];
						G += (int)data[((int)vecCoord[i].y()) * imageWidth * 3 + ((int)vecCoord[i].x()) * 3 + 1];
						B += (int)data[((int)vecCoord[i].y()) * imageWidth * 3 + ((int)vecCoord[i].x()) * 3 + 2];
					}
					CTriangle triangle;
					triangle.pt1 = pt1;
					triangle.pt2 = pt2;
					triangle.pt3 = pt3;
					sceneExtractWall.NormalVector(pt1,pt2,pt3,triangle.normal);
					//osg::Vec4 color;
					//sceneExtractWall.RGB2HSV(osg::Vec4(R / 1.0 / vecCoord.size() / 255.0, G / 1.0 / vecCoord.size() / 255.0, B / 1.0 / vecCoord.size() / 255.0, 1.0), color);
					triangle.color.r() = R / 1.0 / vecCoord.size();
					triangle.color.g() = G / 1.0 / vecCoord.size();
					triangle.color.b() = B / 1.0 / vecCoord.size();
					triangle.color.a() = 1.0;
					vecTriangle.push_back(triangle);
					
				}
			}
		}
	}
	//将点集构造为三角形
	

	
	//为每个三角确定相邻关系
	vecTriangleRelation.clear();

	for (size_t i = 0; i < vecTriangle.size(); ++i)
	{
		CTriangleRelation triangleRelation;
		triangleRelation.self = vecTriangle[i];
		for (size_t j = 0; j < vecTriangle.size(); ++j)
		{
			if (i != j)
			{
				if ((vecTriangle[i]).neighborWith(vecTriangle[j]))
				{
					triangleRelation.neighbor.push_back(vecTriangle[j]);
				}
			}
		}
		vecTriangleRelation.push_back(triangleRelation);
	}
}

bool COsgExtractWall::GetInsectof3Plane(WallInfo& pl1, WallInfo& pl2, WallInfo& pl3, osg::Vec3d& ptInsect)
{
	double a1 = pl1.pl->A;
	double b1 = pl1.pl->B;
	double c1 = pl1.pl->C;
	double d1 = pl1.pl->D;

	double a2 = pl2.pl->A;
	double b2 = pl2.pl->B;
	double c2 = pl2.pl->C;
	double d2 = pl2.pl->D;

	double a3 = pl3.pl->A;
	double b3 = pl3.pl->B;
	double c3 = pl3.pl->C;
	double d3 = pl3.pl->D;

	double M1 = a3 * b2 - a2*b3;
	double M2 = a1*b2 - a2*b1;
	double N1 = a3*c2 - a2*c3;
	double N2 = a1*c2 - a2*c1;
	double P1 = a3*d2 - a2*d3;
	double P2 = a1*d2 - a2*d1;

	ptInsect.z() = (M2*P1 - M1*P2) / (M1*N2 - M2*N1);
	ptInsect.y() = (-M2*P1 - M2*N1*ptInsect.z()) / (M1*M2);
	ptInsect.x() = (-d1 - c1*ptInsect.z() - b1*ptInsect.y()) / a1;

	return true;
}