#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

#include "ThreadClass.h"

ThreadClass::ThreadClass()
{
	hThread = 0;
	ThreadID = 0;
}

void ThreadClass::start()
{
   hThread = CreateThread(0, (SIZE_T)(4096*300), threadMain, this, 0, &ThreadID);
}

void ThreadClass::stop()
{

	//printf("Killing thread...\n");

	if(hThread)
		TerminateThread(hThread, 0);
}
