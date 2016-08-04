#include "stdafx.h"
#include "GlobalsClass.h"

GlobalsClass::GlobalsClass()
{
	// initialize global variables here
	g_globalsMutex = new CMutex(false, NULL);
	
};
