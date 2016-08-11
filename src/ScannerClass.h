

#ifndef SCANNER_H
#define SCANNER_H

#include <WinSock2.h>
#include <afxmt.h>
#include "datapackets.h"
#include "ThreadClass.h"
#include "TCPClass.h"

#pragma comment(lib, "wsock32.lib")

class ScannerClass : public ThreadClass
{
 public:
	
	static ScannerClass *ScannerClass::openScanner(int scannernum, int TCPport, int TCPdataport, char *scannerIP);

	ScannerClass(int scannernum, int TCPport, int TCPdataport, char *scannerIP);
	virtual ~ScannerClass() {delete s_scannerMutex;}
	void ScannerComm();
	void closeScanner();

	CMutex *s_scannerMutex;

	void RunMain();
	int s_doloop;
	int s_Scannernum;
	   
// Variables related to a scanner
	int s_status;
	bool scanInProgress;
	//for writing scan files
	FILE *s_DataFile;

	ScannerLog *log;

	void logScanLine(SRIPacket *packet, int fnum);
	void logBinaryScanLine(SRIPacket *packet, int fnum);
	void CalibrateAndConvertToAscii();
	void ScanToXYZ();
    void updateMyGlobalsFromFile();

	HANDLE s_DataLogEvent;
	HANDLE ackReceivedEvent;
	HANDLE scanPacketRecvdEvent;

	//Image related variables
	int s_countsPerRev[2];
	int s_encoderOffset[2];

	TCPcommandPacket s_TCPcommandPacket;
	TCPstatusPacket s_TCPstatusPacket;
	TCPcommandPacket s_TCPcommmandPacketSingleScan;
	TCPClass *s_TCPCmdClassPtr;
	
	//data packet
	int s_lineNumber_old;
	int s_SRIdataPacketcount; // buffer counter
	int s_SRIdataPacketcount_log; // buffer counter used for logging only
	int s_scanID_old;
	//latest ack
	TCPstatusPacket ack;

	//display limits
	float Dmin, Dmax, Xmin, Xmax, Ymin, Ymax, Zmin, Zmax;
	bool useDist, useX, useY, useZ;
	SRIPacket s_SRIlinePackets[NLINESMAX]; // buffer size
};

#endif
