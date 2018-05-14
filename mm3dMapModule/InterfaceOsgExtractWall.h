#pragma once
#include <vector>
#include <osg/Node>
#include <osg/Group>
#include "InterfaceSqliteDatabase.h"
#include "InterfaceUserEventCallBack.h"
#include "publicDefined.h"
#include "CDraggerPoint.h"
#include "TriangleRelation.h"
#include <osgManipulator/Translate2DDragger>

#ifdef EXTRACTWALLLIBDLL  
#define EXTRACTWALLAPI _declspec(dllexport)  
#else  
#define EXTRACTWALLAPI  _declspec(dllimport)  
#endif  

class EXTRACTWALLAPI InterfaceOsgExtractWall
{
public:
	virtual void savePointDataToSqlite3(std::vector<std::vector<osg::Vec3d> >&vecTotalTriangle, InterfaceSqliteDatabase* iSqliteDatabase, char* tablename, bool add = false, char* dbname = "runData.db") = 0;  //将点存储到数据库

	virtual void clearPointDataInSqlite3(InterfaceSqliteDatabase* iSqliteDatabase, char* tablename, char* dbname = "runData.db") = 0;

	virtual void readPointDataFromSqlite3(std::vector<osg::Vec3d>& vecTotalTriangle, InterfaceSqliteDatabase* iSqliteDatabase, char* tablename,char* dbname = "runData.db") = 0;

	virtual void saveWallDataToSqlite3(std::vector<WallInfo>& planePara, InterfaceSqliteDatabase* iSqliteDatabase, char* dbname) = 0;//面信息存储到数据库

	virtual void clearWallDataInSqlite3(InterfaceSqliteDatabase* iSqliteDatabase, char* dbname) = 0;

	virtual void readWallDataFromSqlite3(std::vector<WallInfo>& planePara, InterfaceSqliteDatabase* iSqliteDatabase, char* dbname) = 0;

	virtual bool DrawTriangle(osg::Group* mRoot, osg::NodePath np, osg::Vec3d worldPt, osg::Matrix worldMatrix, std::vector<osg::Vec3d> &vecTriangle, osg::Vec3d normal,float angle,double colorErr) = 0;	//画三角形

	virtual bool DrawTriangleEx(osg::Group* mRoot, osg::NodePath np, osg::Vec3d worldPt, osg::Matrix worldMatrix, std::vector<osg::Vec3d> &vecTriangle, std::vector<osg::Vec3d> & vecnNormal, std::vector<osg::Vec4> & vecColor, float angle, double colorErr) = 0;

	virtual bool fittingWallSpace(vector<osg::Vec3d> &vecPt,  std::vector<WallInfo>& planePara, osg::Vec3d& normal, osg::Matrix& worldMatrix) = 0;

	virtual void addDraggerToScene(osg::Group* mRoot, osg::Group* slotGroup, osg::Group* editWallGroup) = 0;

	virtual void getWorldMatrix(osg::Group* tempGroup, osg::Matrix& worldMatrix) = 0;

	virtual bool draggerEvent(bool& atWall, const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osgViewer::Viewer* mViewer, CDraggerPoint* _activeDragger, osg::Group* editWallGroup,
		osg::Group* tempGroup, osg::Vec3d& partnerPoint, WallInfo& wallSelected, osgManipulator::PointerInfo& _pointer,
		osg::Vec3d& endPoint, std::vector<WallInfo>& planePara, osg::Matrix& worldMatrix, osg::Vec4d&color) = 0;

	virtual bool wallInsect(osg::Group* mapperGroup, osg::Group* tempGroup, osg::Vec3d &pt, std::vector<WallInfo>& planePara, std::vector<WallInfo>& pS, osg::Vec3d& ptInsect, std::vector<int>& vecWallIndex, osg::Vec4d& color, osg::Matrix& worldMatrix, InterfaceSigEventCallBack* iSigEventCallBack) = 0;

	virtual void lineInsectOprate(std::vector<LineInfo>& vecLineforInsect, std::vector<osg::Vec3d>& vecCoord, osg::Vec3d& worldPt, osg::Vec3d& PointInLine1, osg::Vec3d& PointInLine2, char* transName1, char *transName2, osg::ref_ptr<osg::MatrixTransform>& selectTrans1, osg::ref_ptr<osg::MatrixTransform>& selectTrans2, osg::Group* mapperGroup, osg::Group* tempGroup, osg::Matrix worldMatrix, InterfaceSigEventCallBack* iSigEventCallBack) = 0;

	virtual void calcIntersectOfLine(vector<LineInfo>& vecLineforInsect, std::vector<osg::Vec3d>& vecCoord, osg::Group* mapperGroup, osg::Group* tempGroup, osg::Vec3d& PointInLine1, osg::Vec3d& PointInLine2, char* transName1, char *transName2, osg::ref_ptr<osg::MatrixTransform>& selectTrans1, osg::ref_ptr<osg::MatrixTransform>& selectTrans2, osg::Matrix worldMatrix) = 0;

	virtual void combineGroup(int& n, vector<osg::Vec3d>& vecCoord, std::vector<LineForCombine>& vecLFC, osg::Group* mapperGroup, osg::Vec4d& color, osg::Matrix& worldMatrix, InterfaceSigEventCallBack* iSigEventCallBack) = 0;

	virtual void drawHorizLineOprate(bool &islineStart, double& horiz_high, osg::Vec3d& worldPt, std::vector<LineInfo>& vecLine, std::vector<char*>& vecName, osg::Matrix worldMatrix, vector<osg::Vec3d>& vecCoord, osg::Group* tempGroup, osg::Group* mapperGroup, InterfaceSigEventCallBack* iSigEventCallBack) = 0;

	virtual void deleteHorizLine(std::vector<char*>& vecName, osg::Group* mapperGroup, osg::Group* tempGroup, std::vector<LineInfo>& vecLine, vector<osg::Vec3d>& vecCoord, bool& islineStart, osg::Matrix worldMatrix, InterfaceSigEventCallBack* iSigEventCallBack) = 0;

	virtual void FindTriangleRelation(osg::ref_ptr<osg::Node> node, std::vector<CTriangleRelation>& vecTriangleRelation, osg::Matrix worldMatrix) = 0;

	virtual bool FindRelationInVector(CTriangle& triangle, std::vector<CTriangle>& neighbor, std::vector<CTriangleRelation>& vecTriangleRelation) = 0;    //查找指定面片的相邻面片

	virtual bool findPointInVector(osg::Vec3d& pt,std::vector<osg::Vec3d>& vecPt) = 0;

	virtual bool GetInsectof3Plane(WallInfo& pl1, WallInfo& pl2, WallInfo& pl3, osg::Vec3d& ptInsect) = 0;

protected:

	virtual void getAllWall(vector<LineForCombine>& vecLFC, vector<string> &vecWall) = 0;

	virtual void analysisName(char* name, string& str1, string& str2) = 0;

	virtual bool lineInWall(string& d, char* s) = 0;

	virtual bool getEndPoints(bool &islineStart, osg::Vec3d& pt, std::vector<LineInfo>& vecLine, std::vector<char*>& vecName, vector<osg::Vec3d>& vecCoord, osg::Group* MapperGroup, osg::Matrix worldMatrix, double  dis) = 0;

	virtual void getHorizPoint(osg::Vec3d& ptIn, osg::Vec3d& ptOut, osg::Vec3d& p1, osg::Vec3d& p2) = 0;
};

class EXTRACTWALLAPI InterfaceOsgExtractWallFactory
{
public:
	InterfaceOsgExtractWall* create();
};