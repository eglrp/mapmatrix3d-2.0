#ifndef _ENUM_STRUCT_DEFINED_

#define _ENUM_STRUCT_DEFINED_

#define		STR_HIGHLIGHT		"OutlineHighLight"

#include <vector>
#include "coord.hpp"
#include "OSG_Plus.h"
#include <map>

#ifdef OPSLIBDLL  
#define OPSAPI _declspec(dllexport)  
#else  
#define OPSAPI  _declspec(dllimport)  
#endif  

class CTransNodeandMatrix
{
public:
	osg::Matrix matrix;
	osg::ref_ptr<osg::MatrixTransform> mTrans;
};

class CPropertyListText
{
public:
	std::string strFirstWord;
	std::string strSecondWord;
};

class OPSAPI CPickResult
{
public:
	osg::Vec3d localPt;					//原始点
	osg::Vec3d worldPt;					//世界坐标点
	osg::Vec3d worldNm;
	int modelIndex;						//第几个模型
	osg::NodePath nodePath;				//碰撞点的结构
	osg::ref_ptr<osg::Node> parent;		//最低层点的父子节点
	osg::Matrix matrix;					//矩阵
};

class OPSAPI CScope
{
public:
	CScope()
	{
		minH = 0;
		maxH = 0;
	}

public:
	std::vector<osg::Vec3d> vecCoord;
	osg::Matrix matrix;
	double minH;
	double maxH;
};

struct SCOPEINFO
{
	CScope scope;
	osg::Vec3d pt;//对角点
	int pointIndex;//点号（0——3）
	bool point_up;//选中的点是否在上方
};

class PLane//平面
{
public:
	double A;
	double B;
	double C;
	double D;
	PLane()
	{
		A = 0;
		B = 0;
		C = 0;
		D = 0;
	}
	PLane(double a,double b,double c,double d)
	{
		A = a;
		B = b;
		C = c;
		D = d;
	}
};
//记录墙面信息
struct WallInfo
{
	PLane* pl;
	std::vector<osg::Vec3d> coord;//绘制顶点
	float angle;//倾斜角度
	double err;//拟合误差
	std::string normal;//法向量

	double minH;
	double maxH;
	double minx;
	double maxx;
	double miny;
	double maxy;
};

struct CPointWall
{
	osg::Vec3d pt;
	std::vector<int> vecWallIndex;
};

struct CLine
{
	CLine(osg::Vec3d P1, osg::Vec3d P2)
	{
		p1 = P1;
		p2 = P2;
	}
	CLine()
	{
	}
	bool operator == (const CLine& d)//重载 ==  
	{
		return (this->p1 == d.p1 && this->p2 == d.p2) || (this->p1 == d.p2 && this->p2 == d.p1);
	}
	//直线两个端点
	osg::Vec3d p1;
	osg::Vec3d p2;
};

struct LineInfo
{
	LineInfo(osg::Vec3d P1,osg::Vec3d P2)
	{
		p1 = P1;
		p2 = P2;
		pl1 = NULL;
		pl2 = NULL;
	}
	LineInfo()
	{
		pl1 = NULL;
		pl2 = NULL;
	}
	//直线两个端点
	osg::Vec3d p1;
	osg::Vec3d p2;
	//直线所在的两个面
	PLane* pl1;
	PLane* pl2;
};

struct LineForCombine
{
	string name;
	osg::Vec3d p1;
	osg::Vec3d p2;
};

class CPickNodePath
{
public:
	osg::Vec3d pt;
	osg::NodePath nodePath;
};

inline double distance2(osg::Vec3d p1, osg::Vec3d p2)
{
	return sqrt((p1.x() - p2.x())*(p1.x() - p2.x()) + (p1.y() - p2.y())*(p1.y() - p2.y()) + (p1.z() - p2.z())*(p1.z() - p2.z()));
}

//定义当前场景的操作状态
enum OperatingState{
	OPS_None=0,							//初始状态,0
	OPS_MEASURE_POINT,					//测点状态 1
	OPS_MEASURE_LINE,					//测线状态 2
	OPS_MEASURE_AREA,					//测面状态 3
	OPS_MEASURE_ANGLE,					//测量角度 4
	OPS_MEASURE_VOLUME,					//测体状态 5
	OPS_MEASURE_HEIGHT,					//测量高度 6
	OPS_MEASURE_MULTIlINE,				//测量周长 26
	OPS_MEASURE_CURLINE,				//测量曲线 33
	OPS_MEASURE_MULTICURLINE,			//测量多条曲线 34
	OPS_SCENE_STITCH,					//场景拼接		   49
	OPS_SELECT_CONTROL_POINT,			//选择控制点	   50
	OPS_ABSOLUTE_ORIENTATION,			//绝对定向		   51
	OPS_MANUAL_ORIENTATION,				//手动定向
	OPS_INTERNET_ORIENTATION,			//互联网绝对定向
	OPS_MODEL_MATCHING,					//模型匹配		   52
	OPS_NONE2,
	OPS_ABS_NONE,						//绝对定向置空
	OPS_EDIT_WHOLE_SCENE_Z,				//修改整个场景信息 47
	OPS_EDIT_WHOLE_SCENE_XY,			//修改整个场景信息 48
	OPS_CLIP_BY_POLYGON,				//按范围裁剪 35
	OPS_CLIP_BY_HEIGHT,					//按高度裁剪 36
	OPS_SEL_TILE_REMOVE,				//移除选择的tile   37
	OPS_EDIT_NONE,						//编辑结束	
	OPS_HOLLOW_OUT,						//场景挖空		   56
	OPS_REPAIR_STOP,					//暂停修复		   57
	OPS_HOLLOW_OUT_STOP,				//暂停挖空		   58
	OPS_HOLLOW_OUT_SUSPENDED,			//挖空悬浮		   59
	OPS_HOLLOW_OUT_SUSPENDED_STOP,		//挖空悬浮暂停	   60
	OPS_FINE_CUTTING,					//精细裁切		   61
	OPS_ENTITY_create_DRAG_MOVE_UP,		//向上创建立体模型 62
	OPS_ENTITY_DRAG_MOVE_UP,			//立体整体向上移动 63
	OPS_FLAT_PLANES_MOVE,				//平面向上移动	   64
	OPS_FLAT_PLANES_START,				//平面开始移动	   65
	OPS_FLAT_PLANE_STOP,				//暂停平面移动	   66 
	OPS_REPAIR_HOLE,					//修补空洞		   67
	OPS_EXTRACT_HATCH,					//提取剖面线		
	OPS_EXTRACT_OUTLINE,				//提取轮廓线
	OPS_DRAW_SINGLE_PT,					//画单点
	OPS_DRAW_MULTI_PT,					//画多点
	OPS_DRAW_SINGLE_LINE,				//画单线
	OPS_DRAW_MULTI_SINGLE_LINE,			//画多条单线
	OPS_DRAW_MULTI_LINE,				//画多线
	OPS_DRAW_CLOSED_LINE,				//画闭合线
	OPS_DRAW_FLAT_PLANE,				//画闭合环
	OPS_DRAW_SUSPEND_OBJ_SCOPE,			//画悬空物范围
	OPS_EXTRACT_ACCURA_OUTLINE,         //精确获取轮廓
	OPS_EXTRACT_HAND_PAINT,             //手工绘图
	OPS_DRAW_CLOSED_LINE_HAND,          //手工绘图画闭合线
	OPS_EXTRACT_HAND_PAINT_RING,        //提取手工绘制的闭合线
	OPS_EXTRACT_HAND_PAINT_LINE,        //提取手工绘制的线段
	OPS_DRAW_MULTI_LINE_HAND,           //手工绘制线段
	OPS_DRAW_MULTI_CLOSED_LINE,			//画多个闭合线
	OPS_EXTRACT_TRIANGLE_OUTLINE,		//精确提取轮廓线
	OPS_DRAW_RECTANGLE,					//画框
	OPS_SELECT_LINE_BY_RECTANGLE,		//框选线段
	OPS_SELECT_LINE_BY_Area,			//画范围选线段
	OPS_DRAW_MULTI_CLOSED_LINE_DBCLICK,	//画多个闭合线
	OPS_SELECT_LINE_BY_CLICK,           //点选线
	OPS_EXTRACT_TRIANGLE_OUTLINE_CHANGE_HEIGHT, //以新高度提取轮廓线
	OPS_HOLLOW_OUT_SUSPENDED_UP,                //向上选择范围
	OPS_DRAW_MULTI_CLOSED_LINE_FOR_AREA,        //用于面积测量的画多闭合线事件
	OPS_DRAW_MULTI_LINE_FOR_MULTI_LINE,         //用于多线测量的画多线事件
	OPS_EXTRACT_WALLSPACE,                      //提取墙面
	OPS_SELECT_WALLSPACE_AUTO,                  //自动选择墙面
	OPS_SELECT_WALLSPACE_HAND,                  //手动选择墙面
	OPS_DRAW_HORIZ_LINE,                         //画水平直线
	OPS_COMBINE,                                  //组合节点
	OPS_LINE_INSECT,                               //直线求交
	OPS_MANUAL_EXTRACT_TEXTURE,					 //单体化提取纹理
	OPS_WHOLE_MATCH_SCENE,				//全场景匹配
	OPS_FITTING_WALLSPACE,                //拟合墙面
	OPS_WALLSPACE_INSECT,                 //墙面求交
	OPS_EXTRACT_OBJECT_BY_POLYGON,				//单个对象提取
	OPS_QUICK_CLIP_MODEL_BY_TRIANGLE,			//快速删除三角网
	OPS_QUICK_DELETE_SINGLE_TRIANGLE,			//快速删除单个三角网
	OPS_QUICK_DELETE_SINGLE_TRIANGLE_BY_LINE,	//通过画线快速删除单个三角网
	OPS_QUICK_DELETE_SINGLE_TRIANGLE_BY_SOLID,	//通过体快速删除单个三角网
	OPS_ABS_HOLLOW_OUT_BY_POLYGON,				//边缘内部挖空
	OPS_FLAT_SCENE_BY_POLYGON,					//范围内压平
	OPS_DRAG_POINT,                              //拖拽移动点
	OPS_EXTRACT_TRIANGLE_OUTLINE_DRAW_AREA,
	OPS_EXTRACT_TRIANGLE_OUTLINE_IN_AREA,
	OPS_EXTRACT_TRIANGLE_OUTLINE_IN_AREA_CHANGE_HEIGHT,
	OPS_HIGHLIGHT_OBJECT_BY_CLICK,				//高亮单体化物体
	OPS_LINK_PROPERTY_WITH_OBJECT,				//关联物体
	OPS_EXTRCT_PT_CLOUD_EDGE,                   //提取点云边缘
	OPS_SELECT_PTCLOUD_BY_Area,                 //范围选取点云
	OPS_EXTRCT_PT_CLOUD,						//提取点云
	OPS_SIMPLIFY_OBJECT,						//模型简化
	OPS_DRAW_CLOSED_LINE_WITH_POINT,				//画闭合线和单击事件
	OPS_EXTRACT_ROAD,                              //提取道路
	OPS_DRAW_ROAD_PATH,                             //画路径
	OPS_DRAW_EDGE_BY_HAND,                           //手动绘制边缘
	OPS_DRAW_EDGE_BY_HAND_1,
	OPS_EXTRACT_ROAD_BY_DOM,
	OPS_BREAKOUT_LINE,
	OPS_HORIZON_ACCURATE ,                        //精确置平
	OPS_EXTRACT_CONTOUR,                           //提取等高线
	OPS_EXTRACT_OUTLINE_BY_POSTGIS                 //数据库提取轮廓
};

enum QyManipulatorMode
{
	ManipulatorMode_Normal,		//常规操作
	ManipulatorMode_Path,		//路径漫游
	ManipulatorMode_Panorama,	//全景漫游
	ManipulatorMode_MAP_REGISTRATION  //底图配准
};

typedef struct tagGridDem
{
	double x0, y0;    // 方格网DEM左下角坐标
	double x1, y1;    // 方格网DEM右上角坐标
	double dx, dy;    // 方格网DEM格网间距
	double kappa;     // 方格网DEM的旋转角
	int cols, rows;   // 方格网DEM格网的列数和行数
	float* z;         // 方格网DEM格网点的高程坐标
	float avgz;       // 方格网DEM的平均高程
	float minz, maxz; // 方格网DEM的最小和最大高程
	float denum;
	float* nx;        // 方格网DEM的法向量X分量
	float* ny;        // 方格网DEM的法向量Y分量
	float* nz;        // 方格网DEM的法向量Z分量
	float valid;
	// 初始化方格网DEM数据结构
	tagGridDem(){
		x0 = y0 = 0;
		x1 = y1 = 0;
		dx = dy = 0;
		kappa = 0;
		cols = rows = 0;
		z = NULL;
		avgz = minz = maxz = 0.0;
		denum = 0.0f;
		nx = NULL;
		ny = NULL;
		nz = NULL;
		valid=0;
	};
}GridDem;

class PropertyDataType : public osg::Referenced
{
public:
	std::map<std::string, std::string> mapProperty;
};

#endif 