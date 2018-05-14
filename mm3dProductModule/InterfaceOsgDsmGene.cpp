#include "StdAfx.h"
#include "InterfaceOsgDsmGene.h"
#include "osgDSMGene.h"

InterfaceOsgDsmGene* InterfaceOsgDsmGeneFactory::create()
{
	return new osgDSMGene();
}