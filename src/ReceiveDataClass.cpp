#include "stdafx.h"
#include "ReceiveDataClass.h"
#include "../CommonSrc/datapackets.h"

#include <math.h>
#include <stdio.h>

#include "GlobalsClass.h"


#define BINARYLOG 1
#define CONVERTTOASCII 0
#define CHECKAZIMUTH 0

extern GlobalsClass g_theGlobals;
unsigned ackRecd = 0;


ReceiveDataClass::ReceiveDataClass(ScannerClass *sptr)
{
	 Sptr = sptr; //pointer to scanner class that started this ReceiveDataClass
//	 Sptr->s_SRIdataPacketHdrs[0].packetNumber = 0; // initialize the ID
	 Sptr->s_SRIdataPacketcount = NLINESMAX-1; // initialize buffer data counter so first filled will be 0

	this->start(); // Start the thread
}
int nlines=0;
int scannum = 0;
int lastAz = 0;
void ReceiveDataClass::RunMain()
{
	TCPClass TCPdata(atoi(TCPDATA_PORT));

	// save pointers for later use in closing scanner
	TCPDataClassPtr = (TCPClass *)&TCPdata;

	int TCP_initialized;
	TCP_initialized = TCPdata.initTCPclient(SCANNER_ID);
	if(!TCP_initialized)
	{
		printf("TCP RECEIVE DATA SOCKET OPENING PROBLEM ON CLIENT, port %d\n", TCPdata.TCPport);
		Sleep(1000);
		return;
	}
	printf("TCP RECEIVE DATA SOCKET INITIALIZED, port %d. ReceiveDataClass has started.\n", TCPdata.TCPport);

	int nrecd=0, totalrecd = 0, len;
	SRIPacket *pktptr;

	//flush input
	pktptr = &Sptr->s_SRIlinePackets[Sptr->s_SRIdataPacketcount+1];
	while(nrecd = TCPDataClassPtr->receiveTCP((char *)&pktptr->lineHdr, sizeof(SRIPacketHdr), 100)); //timeout of 0 waits until packet Hdr recd

	while(1)
	{
		if(Sptr->s_SRIdataPacketcount == NLINESMAX-1)
			pktptr = &Sptr->s_SRIlinePackets[0];
		else
			pktptr = &Sptr->s_SRIlinePackets[Sptr->s_SRIdataPacketcount+1];

		// Receive messages
		//printf("Getting Header.\n");
		nrecd = TCPDataClassPtr->receiveTCP((char *)&pktptr->lineHdr, sizeof(SRIPacketHdr), 0); //timeout of 0 waits until packet Hdr recd
		if(nrecd != sizeof(SRIPacketHdr))
		{ //problem: need to reconnect
			continue; 
		}
		//printf("%d\n",Sptr->s_SRIdataPacketcount);
		SetEvent(Sptr->scanPacketRecvdEvent);
		unsigned short cksum =  calcCheckSum((unsigned char *)&pktptr->lineHdr, sizeof(SRIPacketHdr) - sizeof(short)); //dont include checksum
		if (pktptr->lineHdr.checksum != cksum)
        {
			//This can occur if the client is too busy or not fast enough to keep up with the scan data stream.
   				printf("Header checksum did not match.\n");
	//			continue;
		}

		Sptr->s_status = pktptr->lineHdr.status; 

		//if this data is part of a scan, keep the data. If it is just a response to a command, while not sending data, dont keep.
		//packets are always a fixed length, but scan line data may be broken across packets and scan lines are variable length

		//get the line of points. If not in a scan we should get the entire packet
		switch(Sptr->s_status)
		{
			case OK_IN_FIRST_SCAN:
			case OK_IN_LAST_SCAN:
				Sptr->scanInProgress = 1;
			case OK_SCAN_FINISHED:  //extra header sent at end of scan. samplesPerLine will say how much to read to finish out packet	
				Sptr->scanInProgress = 0;
				len = pktptr->lineHdr.samplesPerLine*sizeof(scanDataPerSample);
				scannum = pktptr->lineHdr.scanNumber;
				if(pktptr->lineHdr.lineNumber != Sptr->s_lineNumber_old+1)
				{
					printf("Header line count mismatch.\n");
	//				continue;	
				}
				Sptr->s_lineNumber_old = pktptr->lineHdr.lineNumber;
				
				if(Sptr->s_status == OK_SCAN_FINISHED)
					Sptr->s_lineNumber_old = 0;
				
				break;		
			case OK_SLEEPING:
			case OK_READY:
			case OK_STARTING_SCAN:
			case OK_WAITING_ELEV_MOTOR_SPEED:
			case OK_WAITING_AZ_MOTOR:
			default:
				len = BUFLENBYTES-sizeof(SRIPacketHdr);
				break;
		}

		if(len)
		{
			nrecd = TCPDataClassPtr->receiveTCP((char *)&pktptr->lineData, len, 0);
			if(nrecd != len)
			{   //problem: need to reconnect
				continue; 
			}
		}
		//Will have valid log data if it was a log report command if(pktptr->lineHdr.ack.commandID == LogCommandID)
			Sptr->log = (ScannerLog *)pktptr->lineData;

		//if there is a command ack in the header, extract it.
		if(pktptr->lineHdr.ack.commandID)
		{
			Sptr->ack = pktptr->lineHdr.ack;
			ackRecd++;

			SetEvent(Sptr->ackReceivedEvent); //ack was received: for timeout check and display
			printf("Ack received.\n");
		}

		if(Sptr->s_status == OK_SCAN_FINISHED)
		{

			printf("Scan Finished.\n");
#if CHECKAZIMUTH
			int i,j;
			//test that azimuth psoition is not generating spurious values. Only valid at low turret speed (1 az count per line or less)
			for(i=0;i<=Sptr->s_SRIdataPacketcount;i++)
			{
				for(j=1;j<Sptr->s_SRIlinePackets[i].lineHdr.samplesPerLine;j++)
				{
					if(Sptr->s_SRIlinePackets[i].lineData[j].azimuthEncoder > lastAz+1 || Sptr->s_SRIlinePackets[i].lineData[j].azimuthEncoder < lastAz-1)				
						printf("Az error: Line %d sample %d: Last:%d This:%d\n", i, j, lastAz, Sptr->s_SRIlinePackets[i].lineData[j].azimuthEncoder);
					lastAz = Sptr->s_SRIlinePackets[i].lineData[j].azimuthEncoder;
				}
			}
#endif
#if BINARYLOG
			if(Sptr->s_DataFile)
			{	
				fclose(Sptr->s_DataFile);
				Sptr->s_DataFile = NULL;
				printf("Binary Logging Completed.\n");
#if CONVERTTOASCII
				printf("Converting binary file to ASCII.\n");
				Sptr->CalibrateAndConvertToAscii();
				printf("Done converting.\n");
#endif
			}
#endif
			//get ready for next scan
			Sptr->s_SRIdataPacketcount = NLINESMAX-1; // initialize buffer data counter so first filled will be 0

		}
		//if in a scan this will contain point data
		if(Sptr->s_status == OK_IN_FIRST_SCAN || Sptr->s_status == OK_IN_LAST_SCAN)
		{
//			g_theGlobals.g_globalsMutex->Lock(); //no longer used
			Sptr->s_SRIdataPacketcount++;
			if(Sptr->s_SRIdataPacketcount == NLINESMAX)
				Sptr->s_SRIdataPacketcount = 0;
//			g_theGlobals.g_globalsMutex->Unlock();

#if BINARYLOG
			Sptr->logBinaryScanLine(&Sptr->s_SRIlinePackets[Sptr->s_SRIdataPacketcount], 0);
#endif
		}		
	} //while (1)
	return;
} 

unsigned short calcCheckSum(unsigned char *data, int length)
{
    unsigned short sum=0;
	int i;
	for(i=0;i<length;i++)
		sum+= data[i];
	return(sum);
}