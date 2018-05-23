#pragma once
#include <string>
#include "InterfaceMFCExcute.h"

class EXCUTEAPI MFCExcute : public InterfaceMFCExcute
{
public:
	LPWSTR ConvertCharToLPWSTR(const char *szString);

	void ExecuteCmdLine(std::string batFilePath, std::string strCMD);

	bool createFolder(std::string createFilePath);

	void MFCPeekMessage();

	void PopUpMessage(std::string message);

	bool OpenDialog(std::string &outputFileName, std::string extSelect, bool isOpenDialog);

	bool OpenDialogEx(std::string &outputFileName, std::string extSelect, bool isOpenDialog,std::string ext1,std::string ext2);

	std::string ObtainTileFileName(std::string fileName);

	std::string ObtainBlockName(std::string fileName);

	bool MessageBoxIsReload();

	bool MessageBoxIsReloadWithFileName(std::string msg);

	void SelectDir(std::string &filePath);

	void SearchFile(std::string dir, std::vector<std::string>* vecStrFileName);

	bool IsFileExists(std::string fileName);

	void RemoveFile(std::string fileName);

	void FileCopyTo(std::string source, std::string destination, std::string searchStr, BOOL cover = TRUE);

private:
	bool OtainFileExt(std::string extSelect, std::string &extFileName);
};
	

