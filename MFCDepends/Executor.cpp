#include "stdafx.h"
#include "Executor.h"
#include  <io.h>
#include "MessageDlg.h"
#include <shlwapi.h>

BOOL MakeDirectory(CString dd)
{
	HANDLE fFile; // File Handle
	WIN32_FIND_DATA fileinfo; // File Information Structure
	CStringArray m_arr; // CString Array to hold Directory Structures
	BOOL tt; // BOOL used to test if create Directory was successful
	int x1 = 0; // Counter
	CString tem = ""; // Temporary CString Object

	fFile = FindFirstFile(dd,&fileinfo);

	// if the file exists and it is a directory
	if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		//  Directory Exists close file and return
		FindClose(fFile);
		return FALSE;
	}
	m_arr.RemoveAll();

	for(x1 = 0; x1 < dd.GetLength(); x1++ ) // Parse the supplied CString Directory String
	{ 
		if(dd.GetAt(x1) != '\\') // if the Charachter is not a \ 
			tem += dd.GetAt(x1); // add the character to the Temp String
		else
		{
			m_arr.Add(tem); // if the Character is a \ 
			tem += "\\"; // Now add the \ to the temp string
		}
		if(x1 == dd.GetLength()-1) // If we reached the end of the String
			m_arr.Add(tem);
	}

	// Close the file
	FindClose(fFile);

	// Now lets cycle through the String Array and create each directory in turn
	for(x1 = 1; x1 < m_arr.GetSize(); x1++)
	{
		tem = m_arr.GetAt(x1);
		tt = CreateDirectory(tem,NULL);

		// If the Directory exists it will return a false
		if(tt)
			SetFileAttributes(tem,FILE_ATTRIBUTE_NORMAL);
		// If we were successful we set the attributes to normal
	}
	//  Now lets see if the directory was successfully created
	fFile = FindFirstFile(dd,&fileinfo);

	m_arr.RemoveAll();
	if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		//  Directory Exists close file and return
		FindClose(fFile);
		return TRUE;
	}
	else
	{
		// For Some reason the Function Failed  Return FALSE
		FindClose(fFile);
		return FALSE;
	}
}


LPWSTR MFCExcute::ConvertCharToLPWSTR(const char *szString)
{
	int dwLen = strlen(szString) + 1;
	int nwLen = MultiByteToWideChar(CP_ACP, 0, szString, dwLen, NULL, 0);//算出合适的长度
	LPWSTR lpszPath = new WCHAR[dwLen];
	MultiByteToWideChar(CP_ACP, 0, szString, dwLen, lpszPath, nwLen);

	return lpszPath;
};

void MFCExcute::ExecuteCmdLine(std::string batFilePath, std::string strCMD)
{
	std::string batFileName = batFilePath + "\\a.cmd";
	FILE* fp = fopen(batFileName.c_str(), "w");
	fprintf(fp, "@echo off\n");
	std::string str = "cd /d " + batFilePath + "\n";
	fprintf(fp, str.c_str());
	fprintf(fp, "%s\n", strCMD.c_str());
	fclose(fp);

	std::string cmdLine = "cmd /c " + batFileName;

	STARTUPINFO si = {sizeof(si)};
	PROCESS_INFORMATION pi;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	BOOL bRet = ::CreateProcess(NULL, (LPSTR)(LPCTSTR)(cmdLine.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	if (bRet)
	{
		DWORD dw = WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	remove(batFileName.c_str());
}

bool MFCExcute::createFolder(std::string createFilePath)
{
	if (!IsFileExists(createFilePath))
	{
		char strMsg[MAX_PATH];
		sprintf_s(strMsg, "%s", createFilePath.c_str());
		CString str = strMsg;
		bool isSuccess = MakeDirectory(str);
		return isSuccess;
	}

	return false;
}

void MFCExcute::MFCPeekMessage()
{
	MSG  message;      
	while(::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))   
	{   
		::TranslateMessage(&message);   
		::DispatchMessage(&message);   
	}//来响应windows消息如移动，进度条刷新
}

void MFCExcute::PopUpMessage(std::string message)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	/*CString str(message.c_str());
	AfxMessageBox(str);*/
	CMessageDlg dlg(message);
	dlg.DoModal();
}

bool MFCExcute::MessageBoxIsReload()
{
	/*if (::MessageBox(NULL, _T("场景成功输出，是否重新加载输出的场景？"), _T("提示"), MB_OKCANCEL|MB_ICONQUESTION) == IDOK)  
	{  
	return true;
	}
	else
	{
	return false;
	}*/
	InterfaceMFCExcuteFactory IMFCExecuteFactory;
	InterfaceMFCExcute* IMFCExecute = IMFCExecuteFactory.create();
	IMFCExecute->PopUpMessage("场景输出成功，系统将自动为您加载新场景");
	return true;

}

bool MFCExcute::MessageBoxIsReloadWithFileName(std::string msg)
{
	/*char strMsg[MAX_PATH];
	sprintf_s(strMsg, "场景成功输出%s，是否重新加载输出的场景？", msg.c_str());

	CString tempStr = strMsg;

	if (::MessageBox(NULL, tempStr, _T("提示"), MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
	{
		return true;
	}
	else
	{
		return false;
	}*/
	InterfaceMFCExcuteFactory IMFCExecuteFactory;
	InterfaceMFCExcute* IMFCExecute = IMFCExecuteFactory.create();
	std::string str = "场景成功输出"+ msg +"，系统将自动为您加载新场景";
	IMFCExecute->PopUpMessage(str);
	return true;
	
}

bool MFCExcute::OpenDialogEx(std::string &outputFileName, std::string extSelect, bool isOpenDialog,std::string ext1,std::string ext2)
{
	LPCTSTR ext = extSelect.c_str();
	CFileDialog dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,ext,AfxGetMainWnd());
	CString strPath;  
	CFileException e;
	if(dlg.DoModal() == IDOK)         //判断是否按下"保存"按钮
	{
		strPath = dlg.GetPathName();       //获得文件保存路径，为下面right(4)方便，而不用GetFileName();


		//根据用户选择的扩展名下拉框来确定添加的扩展名
		switch (dlg.m_ofn.nFilterIndex)          //此处可以获取扩展名下拉框是采用的哪种过滤串
		{
		case 1:
			{
				if(strPath.Right(ext1.length()) !=ext1.c_str() )       //判断文件扩展名
					strPath += ext1.c_str();
				break;
			}
		case 2:
			{
				if(strPath.Right(ext2.length()) !=ext2.c_str() )       //判断文件扩展名
					strPath += ext2.c_str();
				break;
			}
		}

		if( access(strPath, 0) == 0 )//需要#include  <io.h> //上述CFileDialog 只能检查名字一模一样的文件是否已经存在，此段代码用来检测保存的文件未设置扩展名，该文件夹内是否有添加扩展名后名字一样的文件，但是有个问题，若文件夹内有 “1”的文件，想保存的文件名也是“1”，则会提示无法保存，而要的实际情况是，仍然可以保存，因为会添加扩展名进去。
		{
			CString strQuery;
			strQuery.Format("%s 已经存在，要替换掉吗？", strPath);
			 if ( IDYES == ::MessageBoxA(NULL, strQuery, "文件覆盖", MB_ICONQUESTION | MB_YESNO) )
			{
				remove(strPath);
				outputFileName = strPath;
				return true;
			}
			return false;
		}

		outputFileName = strPath;
		return true;
	}
	else
		return false;
}

bool MFCExcute::OpenDialog(std::string &outputFileName, std::string extSelect, bool isOpenDialog)
{
	LPCTSTR ext = extSelect.c_str();
	CFileDialog dlg(isOpenDialog, NULL, NULL, OFN_HIDEREADONLY, ext, NULL);

	if (dlg.DoModal() == IDOK)
	{
		CString filePath = dlg.GetPathName();
		CString fileExt = dlg.GetFileExt();

		int posStart = extSelect.find("*.") + 1;
		int posEnd = extSelect.length() - 1;
		std::string subStr = extSelect.substr(posStart);
		posEnd = subStr.find(")");
		std::string ext = subStr.substr(0, posEnd);
		CString strExt(ext.c_str());

		if (fileExt == _T(""))									//防止忘了输出后缀名
		{
			filePath += strExt;
		}

		outputFileName = filePath;

		return true;
	}
	else
	{
		return false;
	}
}

void MFCExcute::SelectDir(std::string &filePath)
{
	char szDir[MAX_PATH];
	BROWSEINFO bi; 
	bi.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDir;
	bi.lpszTitle = "选择位置：";
	bi.iImage = 0;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.ulFlags = BIF_EDITBOX;
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);

	if (lp)
	{
		SHGetPathFromIDList(lp, szDir);
		filePath = szDir;
	}
	else
	{
		return;
	}
}

bool MFCExcute::OtainFileExt(std::string extSelect, std::string &extFileName)
{
	int posStart = extSelect.find("*.") + 1;

	if (posStart == -1)
	{
		return false;
	}

	int posEnd = extSelect.length() - 1;
	std::string subFileName = extSelect.substr(posStart, posEnd);

	posEnd = subFileName.find(")");

	if (posEnd == -1)
	{
		return false;
	}

	extFileName = subFileName.substr(0, posEnd);
	return true;
}

std::string MFCExcute::ObtainTileFileName(std::string fileName)
{
	int pos;

	if (fileName.find("Clip") != -1)
	{
		pos = fileName.find("Clip");
	}
	else
	{
		pos = fileName.find("Tile");
	}

	int length = fileName.length();
	std::string tileFileName = ".\\" + fileName.substr(pos, length - pos);
	return tileFileName;
}

std::string MFCExcute::ObtainBlockName(std::string fileName)
{
	int pos;

	if (fileName.find("Clip") != -1)
	{
		pos = fileName.find("Clip");
	}
	else
	{
		pos = fileName.find("Tile");
	}

	std::string blockName = fileName.substr(0, pos);

	return blockName;
}

void MFCExcute::SearchFile(std::string dir, std::vector<std::string>* vecStrFileName)
{
	CString strDir = dir.c_str();
	CFileFind Finder;
	bool bWorking = Finder.FindFile(strDir);

	while(bWorking) 
	{ 
		bWorking = Finder.FindNextFile(); 
		CString DataFileName = Finder.GetFilePath() ;		//GetFilePath：得到全路径名 

		if(Finder.IsDirectory() && !Finder.IsDots())			//如果是目录          
		{  
			DataFileName+= _T("\\*.osgb");
			std::string temp = DataFileName;
			SearchFile(temp, vecStrFileName);
		} 
		else if(!Finder.IsDirectory() && !Finder.IsDots())		//不为目录 
		{  
			CString DataFileTitle = Finder.GetFileName();
			int pos = DataFileTitle.ReverseFind('.');
			CString DataFileTitleName = DataFileTitle.Left(pos);
			CString DataFileTitleExt = DataFileTitle.Right(DataFileTitle.GetLength() - 1 - pos);

			if (DataFileTitleName.Find(_T('L')) == -1 && DataFileTitleExt == _T("osgb") && DataFileTitleName.Find(_T("Tile")) != -1)
			{
				std::string temp = DataFileName;
				vecStrFileName->push_back(temp);
			}
		}
	}

	Finder.Close();
}

bool MFCExcute::IsFileExists(std::string fileName)
{
	char strMsg[MAX_PATH];
	sprintf_s(strMsg, "%s", fileName.c_str());
	CString str = strMsg;

	if (PathFileExists(str))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void MFCExcute::RemoveFile(std::string fileName)
{
	char strMsg[MAX_PATH];
	sprintf_s(strMsg, "%s", fileName.c_str());
	CString str = strMsg;
	DeleteFile(str);
}

void MFCExcute::FileCopyTo(std::string source, std::string destination, std::string searchStr, BOOL cover)
{
	char sourceStr[MAX_PATH];
	sprintf_s(sourceStr, "%s", source.c_str());
	CString strSourcePath = sourceStr;

	char destStr[MAX_PATH];
	sprintf_s(destStr, "%s", destination.c_str());
	CString strDesPath = destStr;

	char StrSearch[MAX_PATH];
	sprintf_s(StrSearch, "%s", searchStr.c_str());
	CString strFileName = StrSearch;
	CFileFind filefinder;
	CString strSearchPath = strSourcePath + "\\" + strFileName;
	CString filename;
	BOOL bfind = filefinder.FindFile(strSearchPath);
	CString SourcePath, DisPath;

	while (bfind)
	{
		bfind = filefinder.FindNextFile();
		filename = filefinder.GetFileName();
		SourcePath = strSourcePath + "\\" + filename;
		DisPath = strDesPath + "\\" + filename;
		CopyFile((LPCSTR)SourcePath, (LPCSTR)DisPath, cover);
	}

	filefinder.Close();
}