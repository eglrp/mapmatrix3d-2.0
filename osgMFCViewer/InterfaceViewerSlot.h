#pragma once
#include <string>
#include "OSG_Plus.h"

#ifdef ORISLOTLIBDLL  
#define ORISLOTAPI _declspec(dllexport)  
#else  
#define ORISLOTAPI  _declspec(dllimport)  
#endif  

class ORISLOTAPI InterfaceOrientSlot
{
public:
	virtual BOOL AbsOrient(std::string inFilePath) = 0;															//绝对定向

	virtual void ReClickCtrlPt(int ptNum) = 0;																	//绝对定向重新刺点

	virtual void LoadCtrlPt() = 0;																				//加载控制点

	virtual void MatchSingleModel() = 0;																		//匹配单个模型

	virtual void InternetAbsOrient() = 0;																		//互联网绝对定向

	virtual void ManualHorizon() = 0;																			//手动置平

	virtual void ResetControlPtStatus(int ptNum, bool isCheckPt) = 0;											//重新设置控制点状态

	virtual void MatchWholeModel() = 0;
};


class ORISLOTAPI InterfaceEditSlot
{
public:
	virtual void ClipByPolyon(std::string inFilePath) = 0;														//范围裁切

	virtual void HollowOut(std::string inFilePath) = 0;															//挖空

	virtual void HollowOutSuspend(std::string inFilePath) = 0;													//悬空物挖空

	virtual void RepairHole() = 0;																				//修补空洞

	virtual void ClipByHeight() = 0;																			//高度裁切

	virtual void InvertClipHeight() = 0;																		//高度裁切翻转

	virtual void AbsoluteHollowOutByPolygon(std::string inFilePath) = 0;										//按范围内进行裁切

	virtual void FlatByPolygon(std::string inFilePath) = 0;														//按范围压平
};

class ORISLOTAPI InterfaceMapperSlot
{
public:
	virtual void outForDxfAndShp() = 0;																			 //DXF输出

	virtual void ProduceDom(std::string strSceneFileName, double res) = 0;										 //输出DOM

	virtual void ProduceDsm(std::string strSceneFileName, double res) = 0;										 //输出DSM

	virtual bool ExtractContour(double high) = 0;																//提取等高线			

	virtual void setPtCloudParameter(double angle, double alpha,double color) = 0;                               //设置点云提取相关参数

	virtual void getPtCloudParameter(double& angle, double& alpha,double& color) = 0;							//获得点云提取相关参数
};

class ORISLOTAPI InterfaceMonomerSlot
{
public:
	virtual void StartExtractTexture(double res, double deltaZ) = 0;											//提取纹理

	virtual void ExtractObjectByPolygon(std::string inFilePathName) = 0;

	virtual void QuickClipByPolygon() = 0;	

	virtual void QuickClipBySolid() = 0;

	virtual void SetObjectProperty(std::string strObjectProperty) = 0;

	virtual void LookUpOjbectProperty(std::string objectPropertyName, std::string objectPropertyValue) = 0;

	virtual void OutputObjectModelWithoutTexture(std::string outModelWithoutFileName) = 0;

	virtual void SelectHousePt() = 0;
};