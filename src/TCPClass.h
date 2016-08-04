#ifndef TCPCLASS_H
#define TCPCLASS_H
#include <windows.h>
#include <WinSock2.h>
#include "ThreadClass.h"
#include "..\CommonSrc\datapackets.h"


 class TCPClass //: public ThreadClass
 {
 public:

  TCPClass(int port);

  virtual ~TCPClass() {}; // stop(); }
   
    //TCP functions
	bool initTCPclient(const char *machineName);
	bool sendTCPCommand(TCPcommandPacket *newPacket, bool verbose);
	int receiveTCP(char *newPacket, int length, unsigned timeout);
	void closeTCP();

	int t_doloop;
	bool newTCPdatareceived;
	HANDLE wakeTCPThreadEvent;
	HANDLE TCPNotBusyEvent;
	bool TCP_initialized;
	bool TCPrespawn;
    SOCKET theSocketTCP;
	int TCPport;
 private:

};
#endif
