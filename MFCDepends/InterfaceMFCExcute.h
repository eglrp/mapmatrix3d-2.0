#pragma once
#include <string>
#include <vector>

#ifdef EXCUTELIBDLL  
#define EXCUTEAPI _declspec(dllexport)  
#else  
#define EXCUTEAPI  _declspec(dllimport)  
#endif  

class EXCUTEAPI InterfaceMFCExcute
{
public:
	virtual LPWSTR ConvertCharToLPWSTR(const char *szString) = 0;

	virtual void ExecuteCmdLine(std::string batFilePath, std::string strCMD) = 0;

	virtual bool createFolder(std::string createFilePath) = 0;

	virtual void MFCPeekMessage() = 0;

	virtual void PopUpMessage(std::string message) = 0;

	virtual bool OpenDialog(std::string &outputFileName, std::string extSelect, bool isOpenDialog) = 0;

	virtual bool OpenDialogEx(std::string &outputFileName, std::string extSelect, bool isOpenDialog,std::string ext1,std::string ext2) = 0;

	virtual std::string ObtainTileFileName(std::string fileName) = 0;

	virtual std::string ObtainBlockName(std::string fileName) = 0;

	virtual bool MessageBoxIsReload() = 0;

	virtual bool MessageBoxIsReloadWithFileName(std::string msg) = 0;

	virtual void SelectDir(std::string &filePath) = 0;

	virtual void SearchFile(std::string dir, std::vector<std::string>* vecStrFileName) = 0;

	virtual bool IsFileExists(std::string fileName) = 0;

	virtual void RemoveFile(std::string fileName) = 0;

	virtual void FileCopyTo(std::string source, std::string destination, std::string searchStr, BOOL cover = TRUE) = 0;
};

class EXCUTEAPI InterfaceMFCExcuteFactory
{
public:
	InterfaceMFCExcute* create();
};