#ifndef GLOBALSCLASS_H
#define GLOBALSCLASS_H

#include <afxmt.h>
//#include "timer.h"
#include "../CommonSrc/datapackets.h"

#define NSCANNERS 1  // number of scanners
#define TWO_PI 6.28318530717959

#define TCP_SELECT_TIMEOUT 10000 // in microseconds

#define NLINESMAX 3000

extern unsigned short calcCheckSum(unsigned char *data, int length);

 class GlobalsClass
 {
 public:
  GlobalsClass();
  virtual ~GlobalsClass(){delete g_globalsMutex;}
  
  //declare global variables here
   void *ScannerPtr[NSCANNERS]; // This void pointer gets type-cast as (ScannerClass *)
  
  CMutex *g_globalsMutex;

  // variables used in displaying point cloud in openGL
  #define NPTS 20000000
  float X[NPTS], Y[NPTS], Z[NPTS]; // these are shared with the OpenGL thread
  float R[NPTS];
  unsigned int Npoints;
  double minDist, maxDist;

 private: 
  
 };
 
 extern GlobalsClass g_theGlobals;
 extern GlobalsClass g_myGlobals;

#endif
