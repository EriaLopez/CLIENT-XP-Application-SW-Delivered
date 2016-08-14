#include "stdafx.h"
#include "ScannerClass.h"
#include "ReceiveDataClass.h"
#include "BasePCGUI.h"
#include "BasePCGUIDlg.h"
#include <stdio.h>
#include "../CommonSrc/datapackets.h"

ScannerClass *ScannerClass::openScanner(int scannernum, int TCPport, int TCPdataport, char *scannerID)
{
	ScannerClass *scanner = new ScannerClass(scannernum, TCPport, TCPdataport, scannerID);
	return scanner;
}

ScannerClass::ScannerClass(int scannernum, int TCPport, int TCPdataport, char *scannerID) 
{ 

	s_Scannernum = scannernum;
	//printf("ScannerClass: scannernum %d\n", scannernum);
		
	// Initialize the pointer in GlobalsClass
	g_theGlobals.ScannerPtr[s_Scannernum] = (void *)this;

	s_scannerMutex = new CMutex(false,NULL);

	s_lineNumber_old = 0;

	s_DataFile = NULL;
	s_DataLogEvent = CreateEvent(NULL, TRUE, FALSE, "DataLogEvent");
	ackReceivedEvent = CreateEvent(NULL, TRUE, FALSE, "AckRecvEvent");
	scanPacketRecvdEvent = CreateEvent(NULL, TRUE, FALSE, "scanPacketRecvdEvent");
	scanInProgress = 0;
	s_status = OK_SLEEPING; //power up condition
	// initialization over
	this->start();

	printf("Scanner %d created\n", s_Scannernum);
}


void ScannerClass::RunMain()
{
    printf("eclopezv myOglDrawScene() \n");

	s_TCPcommandPacket.packetNumber = 0; // initialize the ID
	TCPClass TCPcmd(atoi(TCP_PORT));

	// save pointers for later use in closing scanner
	s_TCPCmdClassPtr = (TCPClass *)&TCPcmd;
	int TCP_initialized;
	TCP_initialized = TCPcmd.initTCPclient(SCANNER_ID);
//	while(!(TCP_initialized= TCPcmd.initTCPclient(s_IDforSRI500)))	
	{
		if(!TCP_initialized)
		{
			printf("TCP COMMAND/STATUS SOCKET OPENING PROBLEM ON CLIENT, port %d\n", TCPcmd.TCPport);
			return;
		}
	}
	printf("TCP COMMAND/STATUS SOCKET INITIALIZED, port %d. Scannerclass has started.\n", TCPcmd.TCPport);

	ReceiveDataClass Rcvr(this); //starts a thread to get and display points
	Sleep(500);

	s_doloop = 1;
	while (s_doloop==1)
	{
		ScannerComm();
	}
	if (s_DataFile)
		fclose(s_DataFile);
	s_DataFile = NULL;

	//printf("closing sockets\n");
	TCPcmd.closeTCP();
	return;	
}

extern TCPstatusPacket cmdAck;
extern CBasePCGUIDlg *dlgptr;

void ScannerClass::ScannerComm()
{
	while(1)
	{
		WaitForSingleObject(s_DataLogEvent, INFINITE);  //not presently used: logging done inline with receive
		ResetEvent(s_DataLogEvent); // SET THE EVENT to log data
	}	
}

void ScannerClass::closeScanner()
{	
	s_doloop = 0;	
	Sleep(100);
	this->stop();

	return;
}