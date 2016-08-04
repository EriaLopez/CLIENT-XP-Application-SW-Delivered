#include "stdafx.h"
#include "ScannerClass.h"
#include "ScannerFunctions.h"
#include "ReceiveDataClass.h"
#include "BasePCGUI.h"
#include "BasePCGUIDlg.h"


int haltScan(ScannerClass *scanner)
{
		scanner->s_TCPcommandPacket.commandID = haltScanCommandID;
		pushTCPCommand(scanner);
	return 0;
}

int abortScan(ScannerClass *scanner)
{
		scanner->s_TCPcommandPacket.commandID = abortScanCommandID;
		pushTCPCommand(scanner);
	return 0;
}

int elevationSpeed(ScannerClass *scanner, int elevationSpeed)
{
	scanner->s_TCPcommandPacket.commandID = elevationSpeedCommandID;
    scanner->s_TCPcommandPacket.elevSpeed = elevationSpeed;
	pushTCPCommand(scanner);
	return 0;
}

int azimuthSpeed(ScannerClass *scanner, int azimuthSpeed) 
{
	scanner->s_TCPcommandPacket.commandID = azimuthSpeedCommandID;
    scanner->s_TCPcommandPacket.azimuthSpeed = azimuthSpeed;
	pushTCPCommand(scanner);
	return 0;
}

int azimuthParameters(ScannerClass *scanner, int azimuthSpeed, int azimuthStart, int azimuthEnd)
{
	scanner->s_TCPcommandPacket.commandID = azimuthParametersCommandID;
	scanner->s_TCPcommandPacket.azimuthStart = azimuthStart;
    scanner->s_TCPcommandPacket.azimuthEnd = azimuthEnd;
    scanner->s_TCPcommandPacket.azimuthSpeed = azimuthSpeed;
	pushTCPCommand(scanner);
	return(0);
}

int elevationParameters(ScannerClass *scanner, int elevationSpeed, int elevationStart, int samplesPerLine, int elevSampleSpacing)
{
	scanner->s_TCPcommandPacket.commandID = elevationParametersCommandID;
	scanner->s_TCPcommandPacket.elevSpeed = elevationSpeed;
	scanner->s_TCPcommandPacket.elevationStart = elevationStart;
	scanner->s_TCPcommandPacket.samplesPerLine = samplesPerLine;
	scanner->s_TCPcommandPacket.elevSpeed = elevationSpeed;
	scanner->s_TCPcommandPacket.elevSampleSpacing = elevSampleSpacing;
	pushTCPCommand(scanner);
	return(0);
}

int azimuthPosition(ScannerClass *scanner, int azPos)
{
		scanner->s_TCPcommandPacket.commandID = azimuthPositionCommandID;
		scanner->s_TCPcommandPacket.azimuthPosn = azPos;
		pushTCPCommand(scanner);
	return 0;
}

int startScan(ScannerClass *scanner)
{
		scanner->s_TCPcommandPacket.commandID = singleScanCommandID;
		// reset the packet counter
		scanner->s_SRIdataPacketcount = NLINESMAX-1;
		pushTCPCommand(scanner);
		scanner->scanInProgress = 1;

	return 0;
}

int getStatus(ScannerClass *scanner)
{

	// Send command
	scanner->s_TCPcommandPacket.commandID = StatusCommandID;
	int ackstat = pushTCPCommand(scanner);
	if(ackstat == COMMAND_ACK_TIMEOUT)
		scanner->s_status = NO_RESPONSE;
	if(ackstat == CLIENT_WAIT_FAILED)
		scanner->s_status = ACK_WAIT_FAILED;
	return 0;
}

int getLog(ScannerClass *scanner)
{
	scanner->s_TCPcommandPacket.commandID = LogCommandID;
	int ackstat = pushTCPCommand(scanner);
	if(ackstat == COMMAND_ACK_TIMEOUT)
	{
		scanner->s_status = NO_RESPONSE;
		return(-1);
	}
	if(ackstat == CLIENT_WAIT_FAILED)
	{
		scanner->s_status = ACK_WAIT_FAILED;
		return(-1);
	}

	if(ackstat == CMD_RCVD_OK)
	{
		printf("Ack status %d. Scanner status %d\n", ackstat, scanner->s_status);
		printf("Current Elevation motor speed %d\n", scanner->log->elevationSpeed);
		printf("Current Azimuth motor speed %d\n", scanner->log->azimuthSpeed);
		printf("Current Azimuth motor position %d\n", scanner->log->azimuthPosition);
		printf("Current Temperature %.3f\n", ((double)scanner->log->temperature)/1000.0);
		printf("Sensor Voltage Offset %d\n", scanner->log->hv);
		printf("Detector Threshold Offset %d\n", scanner->log->th);
		printf("Calibration Values:%d %d\n", scanner->log->calValue1, scanner->log->calValue2);
		printf("%d Errors, most recent at top:\n", scanner->log->nerrors);
		int i;
		short *nptr = ((short *)(scanner->log->errlist)) + scanner->log->nerrors;
		if(scanner->log->nerrors == 0)
			printf("\tNo errors\n");
		for(i=0;i<scanner->log->nerrors;i++)
			printf("\tError %d occurred %d times.\n",(int)(scanner->log->errlist[i]), (int)*(nptr+i));
		return 0;
	}
	else return(-1);
}

int scannerSleep(ScannerClass *scanner)
{
	scanner->s_TCPcommandPacket.commandID = scanSleepCommandID;
	int ackstat = pushTCPCommand(scanner);
	if(ackstat == COMMAND_ACK_TIMEOUT)
		scanner->s_status = NO_RESPONSE;
	if(ackstat == CLIENT_WAIT_FAILED)
		scanner->s_status = ACK_WAIT_FAILED;
	return ackstat;
}

int calibrateScan(ScannerClass *scanner)
{
	scanner->CalibrateAndConvertToAscii();
	return 0;
}


extern CBasePCGUIDlg *dlgptr;

int pushTCPCommand(ScannerClass *scanner)
{
		dlgptr->upDateAckStatus(BLANK_LINE);
		Sleep(10);

		scanner->s_TCPcommandPacket.packetNumber++;
		printf("pushTCPCommand: Sending.\n");
		ResetEvent(scanner->ackReceivedEvent); //flush any that returned after timeout last time

		if(!scanner->s_TCPCmdClassPtr->sendTCPCommand(&scanner->s_TCPcommandPacket, true))
		{
			printf("TCP send failed.\n");
			return(CLIENT_SEND_FAILED);
		}

		int result = WaitForSingleObject(scanner->ackReceivedEvent, 1500);
		ResetEvent(scanner->ackReceivedEvent);
		if(scanner->ack.commandID != scanner->s_TCPcommandPacket.commandID)
			scanner->ack.status = MISMATCHED_COMMAND;

		if(result == WAIT_FAILED)
		{
			int dwError = WSAGetLastError();
			scanner->ack.status = CLIENT_WAIT_FAILED;
		}		
		if(result == WAIT_TIMEOUT)
			scanner->ack.status = COMMAND_ACK_TIMEOUT;
		
		dlgptr->upDateAckStatus(scanner->ack.status);
		return(scanner->ack.status);
}