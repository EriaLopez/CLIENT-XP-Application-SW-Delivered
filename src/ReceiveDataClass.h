#ifndef UDPCLASS_H
#define UDPCLASS_H

#include "ThreadClass.h"
#include "ScannerClass.h"

 class ReceiveDataClass : public ThreadClass
 {

 public:

   ReceiveDataClass(ScannerClass *sptr);
   virtual ~ReceiveDataClass() { stop(); }
  
   void RunMain();
	int t_doloop;
	ScannerClass *Sptr; // Pointer to the scanner class invoking this UDP class and thread
	TCPClass *TCPDataClassPtr;	
 };


#endif
