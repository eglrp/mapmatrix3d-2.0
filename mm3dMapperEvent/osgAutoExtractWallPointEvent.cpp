// osgAutoExtractWallPointEvent.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "osgAutoExtractWallPointEvent.h"
#include "ThreadSafeUpdateCallback.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceOsgExtractWall.h"
#include <osgViewer/Renderer>
#include "osgUtil/RayIntersector"
#include "osgToolFunc.h"

#define WALLCOLOR osg::Vec4d(0,1,1,0.4)      //墙面颜色
#define PTCOLOR osg::Vec4d(0, 0, 1, 1)																   //点颜色
const float ANGLE = 20;																					   //面片角度偏差阈值
std::vector<WallInfo> gVecWall;
std::vector<CPointWall> gVecPW;

void COsgAutoExtractWallPointEvent::RegisterJScripts()
{
	//获得exe路径
	char szapipath[MAX_PATH];
	memset(szapipath, 0, MAX_PATH);
	GetModuleFileNameA(NULL, szapipath, MAX_PATH);
	mEXEPath = string(szapipath);
	int pos = mEXEPath.rfind('\\');
	mEXEPath = mEXEPath.substr(0, pos);

	//初始化temp_found 
	std::ifstream in0(mEXEPath + "\\scripts\\initializeTempFound.sql");
	string s0((istreambuf_iterator<char>(in0)), istreambuf_iterator<char>());
	PQexec(conn, s0.c_str());

	//检查三角面片是否搜索过  未搜索过可选择是否插入
	std::ifstream in1(mEXEPath + "\\scripts\\checkTheTriFound.sql");
	string s1((istreambuf_iterator<char>(in1)), istreambuf_iterator<char>());
	PQexec(conn, s1.c_str());//加载脚本

	std::ifstream in2(mEXEPath + "\\scripts\\baseFunction.txt");
	string s2((istreambuf_iterator<char>(in2)), istreambuf_iterator<char>());
	PQexec(conn, s2.c_str());//加载脚本

}

void COsgAutoExtractWallPointEvent::UnRegisterJScripts()
{
	std::ifstream in0(mEXEPath + "\\scripts\\UninstallPlaneScripts.sql");
	string s0((istreambuf_iterator<char>(in0)), istreambuf_iterator<char>());
	PQexec(conn, s0.c_str());
	PQexec(conn, "begin;select UninstallPlaneScripts();drop function UninstallPlaneScripts();commit;");
}

void COsgAutoExtractWallPointEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_SELECT_WALLSPACE_AUTO);

	if (isActivated == false && flag == true)													 //不激活
	{
		if (conn)
		{
			UnRegisterJScripts();
			PQfinish(conn);
			conn = NULL;
		}
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		vecTotalTriangle.clear();
		
		InterfaceOsgExtractWallFactory IWallFactory;
		InterfaceOsgExtractWall* IWall = IWallFactory.create();
		InterfaceBaseDrawerFactory IDrawerFactory;
		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
		IWall->addDraggerToScene(mRoot, slotGroup, mEditGroup);
		
		iSigEventCallBack->EmitSigShowOpsHint("自动采样：按Ctrl + 鼠标左键选择采样墙面");
		
		CPGController pgControl;
		conn = pgControl.connectToDB();
		RegisterJScripts();
	}
}


bool COsgAutoExtractWallPointEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		char msg[299];
		InterfaceOsgExtractWallFactory IWallFactory;
		InterfaceOsgExtractWall* IWall = IWallFactory.create();
		InterfaceBaseDrawerFactory IDrawerFactory;
		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
		
        if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Vec3d normal = pickResult.worldNm;
			osg::NodePath np = pickResult.nodePath;
			worldMatrix = pickResult.matrix;
			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}
			time_t te, ts;
			ts = time(NULL);
			
			std::vector<point3D> vecLineRes;
			doSearchPointEvent(ea.getX(), ea.getY(), np, worldPt, normal, vecLineRes);
			if (vecLineRes.size()>0)
			{	
				std::vector<osg::Vec3d> vecPt;
				for (auto pt:vecLineRes)
				{
					vecPt.push_back(osg::Vec3d(pt.x, pt.y, pt.z));
				}
				m_vecPointOfLastWall.assign(vecPt.begin(), vecPt.end());
				
				
				IWall->fittingWallSpace(vecPt, gVecWall, normal, worldMatrix);
				IDrawer->BaseDrawPolygon(gVecWall.back().coord, osg::Vec4d(0, 1, 1, 0.4), mEditGroup, worldMatrix);	
				te = time(NULL);
				sprintf(msg, "自动提取墙面：墙面倾斜角：%f ° 墙面拟合误差：%lf,耗时：%d 秒", gVecWall.back().angle, gVecWall.back().err,te - ts);
				iSigEventCallBack->EmitSigShowOpsHint(msg);
				if (gVecWall.size() >= 3)
				{
					std::vector<WallInfo> vecwall;
					vecwall.assign(gVecWall.begin(), gVecWall.end() - 1);
					WallInfo wall = gVecWall.back();

					for (int i = 0; i < vecwall.size() - 1; ++i)
					{
						for (int j = i + 1; j < vecwall.size(); ++j)
						{
							osg::Vec3d ptInsect;
							if (IWall->GetInsectof3Plane(vecwall[i], vecwall[j], wall, ptInsect))
							{
								CPointWall pw;
								pw.pt = ptInsect;
								pw.vecWallIndex.push_back(i);
								pw.vecWallIndex.push_back(j);
								pw.vecWallIndex.push_back(gVecWall.size() - 1);
								gVecPW.push_back(pw);
							}
						}
					}
				}
			}
		}
		else if (IsLeftClick(ea) && GetKeyState(VK_SHIFT) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Vec3d normal = pickResult.worldNm;
			osg::NodePath np = pickResult.nodePath;
			worldMatrix = pickResult.matrix;
			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}
			time_t te, ts;
			ts = time(NULL);
		
			std::vector<point3D> vecLineRes;
			doSearchPointEvent(ea.getX(), ea.getY(), np, worldPt, normal, vecLineRes);
			if (vecLineRes.size() > 0)
			{
				std::vector<osg::Vec3d> vecPt;
				for (auto pt : vecLineRes)
				{
					vecPt.push_back(osg::Vec3d(pt.x, pt.y, pt.z));
				}
				m_vecPointOfLastWall.insert(m_vecPointOfLastWall.end(), vecPt.begin(), vecPt.end());
				doBackSpaceEvent();
				IWall->fittingWallSpace(m_vecPointOfLastWall, gVecWall, normal, worldMatrix);
				IDrawer->BaseDrawPolygon(gVecWall.back().coord, osg::Vec4d(0, 1, 1, 0.4), mEditGroup, worldMatrix);
				te = time(NULL);
				sprintf(msg, "自动提取墙面：墙面倾斜角：%f ° 墙面拟合误差：%lf,耗时：%d 秒", gVecWall.back().angle, gVecWall.back().err, te - ts);
				iSigEventCallBack->EmitSigShowOpsHint(msg);
				if (gVecWall.size() >= 3)
				{
					std::vector<WallInfo> vecwall;
					vecwall.assign(gVecWall.begin(), gVecWall.end() - 1);
					WallInfo wall = gVecWall.back();

					for (int i = 0; i < vecwall.size() - 1; ++i)
					{
						for (int j = i + 1; j < vecwall.size(); ++j)
						{
							osg::Vec3d ptInsect;
							if (IWall->GetInsectof3Plane(vecwall[i], vecwall[j], wall, ptInsect))
							{
								CPointWall pw;
								pw.pt = ptInsect;
								pw.vecWallIndex.push_back(i);
								pw.vecWallIndex.push_back(j);
								pw.vecWallIndex.push_back(gVecWall.size() - 1);
								gVecPW.push_back(pw);
							}
						}
					}
				}
			}
		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{
				doBackSpaceEvent();
				m_vecPointOfLastWall.clear();		
				return false;
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				ClearGroupResult(mEditGroup);
				gVecWall.clear();
				gVecPW.clear();
				iSigEventCallBack->EmitSigShowOpsHint("");

				m_vecPlane.clear();
				m_vecPlanePoint.clear();
				m_vecContourPoint.clear();
				m_vecPointOfLastWall.clear();
				return false;
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_C)
			{
				if (m_vecContourPoint.size()>2)
				{
					ClearGroupResult(mEditGroup);
					IDrawer->BaseDrawLineLoopOperate(m_vecContourPoint, osg::Vec4(0, 1, 0, 1), 3.5, mEditGroup, worldMatrix);
				}
				return true;
				if (m_vecPlane.size()>0)
				{
					for (int i = 1;i<m_vecPlane.size();++i)
					{
						float ang = CL::getAngleForVector(osg::Vec3d(m_vecPlane[i].A, m_vecPlane[i].B, m_vecPlane[i].C), osg::Vec3d(m_vecPlane[0].A, m_vecPlane[0].B, m_vecPlane[0].C));
						if (ang == 90)
						{
							double a1 = m_vecPlane[i].A;
							double b1 = m_vecPlane[i].B;
							double c1 = m_vecPlane[i].C;
							double d1 = m_vecPlane[i].D;
							double a2 = m_vecPlane[0].A;
							double b2 = m_vecPlane[0].B;
							double c2 = m_vecPlane[0].C;
							double d2 = m_vecPlane[0].D;
							double z1 = m_vecPlanePoint[0].z() - 0.5;
							double x1 = ((-d1 - c1*z1)*b2 - (-d2 - c2*z1)*b1) / (b2*a1 - b1*a2);
							double y1 = ((-d1 - c1*z1)*a2 - (-d2 - c2*z1)*a1) / (a2*b1 - a1*b2);

							double z2 = m_vecPlanePoint[0].z() + 0.5;
							double x2 = ((-d1 - c1*z2)*b2 - (-d2 - c2*z2)*b1) / (b2*a1 - b1*a2);
							double y2 = ((-d1 - c1*z2)*a2 - (-d2 - c2*z2)*a1) / (a2*b1 - a1*b2);

							IDrawer->BaseDrawLineOperate(osg::Vec3d(x1, y1, z1), osg::Vec3d(x2, y2, z2), osg::Vec4(0, 0, 1, 1), 3.5, mEditGroup, worldMatrix);
						}
					}
				}

			}
		}
	}


	return false;
}

void COsgAutoExtractWallPointEvent::doBackSpaceEvent()
{
	if (mEditGroup->getNumChildren() > 0)
	{
		mEditGroup->removeChild(mEditGroup->getNumChildren() - 1);
		gVecWall.pop_back();
		iSigEventCallBack->EmitSigShowOpsHint("提示：删除一个房屋面");
	}
	if (gVecPW.size() > 0)
	{
		int index = mEditGroup->getNumChildren();//待删除墙面的序号
												 //删除和已删除面相关的特征点
		for (std::vector<CPointWall>::iterator ite = gVecPW.begin(); ite != gVecPW.end();)
		{
			if (std::find(((*ite).vecWallIndex).begin(), ((*ite).vecWallIndex).end(), index) != ((*ite).vecWallIndex).end())
			{
				ite = gVecPW.erase(ite);
			}
			else
			{
				ite++;
			}
		}
	}
}

void COsgAutoExtractWallPointEvent::doSearchPointEvent(float x,float y,osg::NodePath np,osg::Vec3d worldPt,osg::Vec3d normal, std::vector<point3D>& vecLineRes)
{
	std::vector<osg::Vec3d> vecTriangle;

	//自动采样
	COsgToolFunc osgToolFunc;
	std::string tableName;
	if (!osgToolFunc.getLodBottomLevelFile(x, y, np, mViewer, tableName))
	{
		iSigEventCallBack->EmitSigShowOpsHint("搜索最底层文件失败!");
		return ;
	}
	CPGController pgControl;
	//搜索种子面片
	CTri3D tri;
	int idSel;
	std::vector<int> ivecNeighbor;
	char msg[1024];
	int numTri = pgControl.getContourStartTri(conn, point3D(worldPt.x(), worldPt.y(), worldPt.z()), point3D(normal.x(), normal.y(), normal.z()),
		tableName, tri, idSel, ivecNeighbor);
	if (numTri < 1)
	{
		iSigEventCallBack->EmitSigShowOpsHint("获取种子面片失败!");
		return ;
	}
	else
	{
		sprintf_s(msg, "搜索到%d个种子面片,将进行筛选", numTri);
		iSigEventCallBack->EmitSigShowOpsHint(msg);
	}
	std::vector<CTri3D> vecTri;
	pgControl.DoSearchPlanePostGis(conn, tri, idSel, tableName, ivecNeighbor, vecLineRes, point3D(normal.x(), normal.y(), normal.z()), ANGLE, vecTri);
}
////////////////////////////////////////////////////////


