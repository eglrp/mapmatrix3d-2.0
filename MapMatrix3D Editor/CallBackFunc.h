#pragma once
#include <string>

class CallBackNotifyMeasureMsg
{
public:
	void operator() (char* msg) const;
};

class CallBackCloseMeasureDlg
{
public:
	void operator() () const;
};

class CallBackCloseAbsDlg
{
public:
	void operator() () const;
};

class CallBackSelModelIndex
{
public:
	int operator() () const;
};

class CallBackGetSelRowFromAbsDlg
{
public:
	int operator() () const;
};

class CallBackChangeCtrlList
{
public:
	bool operator() () const;
};

class CallBackGetSelPtNumFromAbsDlg
{
public:
	int operator() () const;
};

class CallBackObtainRelativePoint
{
public:
	void operator() (int ptNum, double xRelaPt, double yRelaPt, double zRelaPt, bool isCheckPt) const;
};

class CallBackSetProgressBarPos
{
public:
	void operator() (int pos) const;
};

class CallBackStartProgress
{
public:
	void operator() () const;
};

class CallBackEndProgress
{
public:
	void operator() () const;
};

class CallBackShowOpsBarHint 
{
public:
	void operator() (char* strHint) const;
};

class CallBackRemoveAllScene
{
public:
	void operator() () const;
};

class CallBackAddScene
{
public:
	void operator() (char* strName) const;
};

class CallBackInsertControlListRecord
{
public:
	void operator() (int ptNum, double ptX, double ptY, double ptZ) const;
};

class CallBackPopUpPropertyListDlg
{
public:
	void operator() (char* propertyListText) const;
};


class CallBackObjectMenuDlg
{
public:
	void operator() (int mouseX, int mouseY) const;
};
