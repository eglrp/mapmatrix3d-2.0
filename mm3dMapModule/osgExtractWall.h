#pragma once
#include "InterfaceOsgExtractWall.h"


class COsgExtractWall : public InterfaceOsgExtractWall
{
public:
	virtual void savePointDataToSqlite3(std::vector<std::vector<osg::Vec3d> >&vecTotalTriangle, InterfaceSqliteDatabase* iSqliteDatabase, char* tablename ,bool add = false,char* dbname = "runData.db");//点数据保存到数据库

	virtual void clearPointDataInSqlite3(InterfaceSqliteDatabase* iSqliteDatabase, char* tablename, char* dbname = "runData.db");

	virtual void saveWallDataToSqlite3(std::vector<WallInfo>& planePara, InterfaceSqliteDatabase* iSqliteDatabase, char* dbname);

	virtual void readPointDataFromSqlite3(std::vector<osg::Vec3d>& vecTotalTriangle, InterfaceSqliteDatabase* iSqliteDatabase, char* tablename, char* dbname = "runData.db");

	virtual void readWallDataFromSqlite3(std::vector<WallInfo>& planePara, InterfaceSqliteDatabase* iSqliteDatabase, char* dbname);

	virtual void clearWallDataInSqlite3(InterfaceSqliteDatabase* iSqliteDatabase, char* dbname);

	virtual bool DrawTriangle(osg::Group* mRoot, osg::NodePath np, osg::Vec3d worldPt, osg::Matrix worldMatrix, std::vector<osg::Vec3d> &vecTriangle, osg::Vec3d normal,float ang,  double colorErr);	//画三角形

	virtual bool DrawTriangleEx(osg::Group* mRoot, osg::NodePath np, osg::Vec3d worldPt, osg::Matrix worldMatrix, std::vector<osg::Vec3d> &vecTriangle, std::vector<osg::Vec3d> & vecnNormal, std::vector<osg::Vec4> & vecColor, float angle, double colorErr);

	virtual bool fittingWallSpace(vector<osg::Vec3d> &vecPt, std::vector<WallInfo>& planePara, osg::Vec3d& normal, osg::Matrix& worldMatrix);

	virtual void addDraggerToScene(osg::Group* mRoot, osg::Group* slotGroup, osg::Group* editWallGroup);

	virtual void getWorldMatrix(osg::Group* tempGroup, osg::Matrix& worldMatrix);

	virtual bool draggerEvent(bool& atWall,const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osgViewer::Viewer* mViewer, CDraggerPoint* _activeDragger, osg::Group* editWallGroup,
		osg::Group* tempGroup, osg::Vec3d& partnerPoint, WallInfo& wallSelected, osgManipulator::PointerInfo& _pointer,
		osg::Vec3d& endPoint, std::vector<WallInfo>& planePara, osg::Matrix& worldMatrix, osg::Vec4d&color);

	virtual bool wallInsect(osg::Group* mapperGroup, osg::Group* tempGroup, osg::Vec3d &pt, std::vector<WallInfo>& planePara, std::vector<WallInfo>& pS, osg::Vec3d& ptInsect, std::vector<int>& vecWallIndex, osg::Vec4d& color, osg::Matrix& worldMatrix, InterfaceSigEventCallBack* iSigEventCallBack);

	virtual void lineInsectOprate(std::vector<LineInfo>& vecLineforInsect, std::vector<osg::Vec3d>& vecCoord, osg::Vec3d& worldPt, osg::Vec3d& PointInLine1, osg::Vec3d& PointInLine2, char* transName1, char *transName2, osg::ref_ptr<osg::MatrixTransform>& selectTrans1, osg::ref_ptr<osg::MatrixTransform>& selectTrans2, osg::Group* mapperGroup, osg::Group* tempGroup, osg::Matrix worldMatrix, InterfaceSigEventCallBack* iSigEventCallBack);

	virtual void calcIntersectOfLine(vector<LineInfo>& vecLineforInsect, std::vector<osg::Vec3d>& vecCoord, osg::Group* mapperGroup, osg::Group* tempGroup, osg::Vec3d& PointInLine1, osg::Vec3d& PointInLine2, char* transName1, char *transName2, osg::ref_ptr<osg::MatrixTransform>& selectTrans1, osg::ref_ptr<osg::MatrixTransform>& selectTrans2, osg::Matrix worldMatrix);

	virtual void combineGroup(int& n, vector<osg::Vec3d>& vecCoord, std::vector<LineForCombine>& vecLFC, osg::Group* mapperGroup, osg::Vec4d& color, osg::Matrix& worldMatrix, InterfaceSigEventCallBack* iSigEventCallBack);

	virtual void drawHorizLineOprate(bool &islineStart, double& horiz_high, osg::Vec3d& worldPt, std::vector<LineInfo>& vecLine, std::vector<char*>& vecName, osg::Matrix worldMatrix, vector<osg::Vec3d>& vecCoord, osg::Group* tempGroup, osg::Group* mapperGroup, InterfaceSigEventCallBack* iSigEventCallBack);

	virtual void deleteHorizLine(std::vector<char*>& vecName, osg::Group* mapperGroup, osg::Group* tempGroup, std::vector<LineInfo>& vecLine, vector<osg::Vec3d>& vecCoord, bool& islineStart, osg::Matrix worldMatrix, InterfaceSigEventCallBack* iSigEventCallBack) ;

	virtual void FindTriangleRelation(osg::ref_ptr<osg::Node> node, std::vector<CTriangleRelation>& vecTriangleRelation, osg::Matrix worldMatrix);

	virtual bool FindRelationInVector(CTriangle& triangle, std::vector<CTriangle>& neighbor, std::vector<CTriangleRelation>& vecTriangleRelation) ;

	virtual bool findPointInVector(osg::Vec3d& pt, std::vector<osg::Vec3d>& vecPt);

	virtual bool GetInsectof3Plane(WallInfo& pl1,WallInfo& pl2,WallInfo& pl3,osg::Vec3d& ptInsect);
protected:
	void getAllWall(vector<LineForCombine>& vecLFC, vector<string> &vecWall);

	void analysisName(char* name, string& str1, string& str2);

	bool lineInWall(string& d, char* s);

	bool getEndPoints(bool &islineStart, osg::Vec3d& pt, std::vector<LineInfo>& vecLine, std::vector<char*>& vecName, vector<osg::Vec3d>& vecCoord, osg::Group* MapperGroup, osg::Matrix worldMatrix, double  dis);

	void getHorizPoint(osg::Vec3d& ptIn, osg::Vec3d& ptOut, osg::Vec3d& p1, osg::Vec3d& p2);
};