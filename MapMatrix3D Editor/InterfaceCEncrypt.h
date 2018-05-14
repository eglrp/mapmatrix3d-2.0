#pragma once

#ifdef ENCRYPTLIBDLL  
#define ENCRYPTAPI _declspec(dllexport)  
#else  
#define ENCRYPTAPI  _declspec(dllimport)  
#endif  


class ENCRYPTAPI InterfaceCEncrypt 
{
public:
	virtual bool CheckModuleNum(int iModuleID,int CheckDongleLicense) = 0;														//¼ì²éÄ£ÐÍ±àºÅ
};


class ENCRYPTAPI InterfaceCEncryptFactory
{
public:
	InterfaceCEncrypt* Create();
};

