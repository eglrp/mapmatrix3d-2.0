#include "StdAfx.h"
#include "InterfaceOsgEditor.h"
#include "COsgEditor.h"

InterfaceOsgEditor* InterfaceOsgEditorFactory::create()
{
	return new OsgEditor();
}