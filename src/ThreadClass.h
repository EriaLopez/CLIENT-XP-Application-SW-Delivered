#ifndef THREADCLASS_H
#define THREADCLASS_H

#include <windows.h>
#include "GlobalsClass.h"

class ThreadClass
 {
 public:
	  ThreadClass();
	  virtual ~ThreadClass() {}

	  void start();
	  void stop();
	  virtual void RunMain() = 0;
	  //void RunMain();// = 0;

 protected:
  static unsigned long __stdcall threadMain(void* ptr)
	{
	   ((ThreadClass *)ptr)->RunMain();
	   return 0;
	}

  ThreadClass *thisThread;
  HANDLE    hThread;
  DWORD ThreadID;

 };
 
#endif
