#include "stdafx.h"
#include <stdio.h>
#include "TCPClass.h"
#include "globalsClass.h"
#include <Winsock2.h>
//unsigned short tracebuf[1000000];
unsigned grandTotal = 0, Ms = 0;
int err;
TCPClass::TCPClass(int port)
{
	 TCPrespawn = true;
	 TCP_initialized = false;
   	 TCPport = port;

	 // Other initializations
	 theSocketTCP = INVALID_SOCKET;
//	 int i;
//	 for(i=0;i< 1000000;i++)
//		 tracebuf[i] = 0xff;

	 wakeTCPThreadEvent = CreateEvent(NULL, TRUE, FALSE, "WakeTCPEvent");
	 TCPNotBusyEvent = CreateEvent(NULL, TRUE, FALSE, "TCPNotBusyEvent");
	 SetEvent(TCPNotBusyEvent); // Set it here. TCP is not busy in the beginning.
}

//machineName may be host name or IP address
static char flushbuf[1000];
bool TCPClass::initTCPclient(const char *machineName)
{
   WORD wVersionRequested = MAKEWORD(1,1);
	WSADATA wsaData;
	int nRet;
	
	printf("Creating TCPsocket.\n");

	// Initialize WinSock and check the version
	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (wsaData.wVersion != wVersionRequested)
	{	
		fprintf(stderr,"\n Wrong version\n");
		return false;
	}

    DWORD dwError;
    struct hostent *remoteHost;
    struct in_addr addr;
// If the user input is an alpha name for the host, use gethostbyname()
// If not, get host by addr (assume IPv4)

    if (isalpha(machineName[0])) {        /* host address is a name */
        printf("Calling gethostbyname with %s\n", machineName);
        remoteHost = gethostbyname(machineName);
    } else {
//THIS DOES NOT WORK. Use gethostbyname with the hostname SRI500.
        printf("Calling gethostbyaddr with %s\n", machineName);
        addr.s_addr = inet_addr(machineName);
        if (addr.s_addr == INADDR_NONE) {
            printf("The IPv4 address entered must be a legal address\n");
            return 1;
        } else
            remoteHost = gethostbyaddr((char *) &addr, 4, AF_INET);

    }
	
    if (remoteHost == NULL) {
        dwError = WSAGetLastError();
        if (dwError != 0) {
            if (dwError == WSAHOST_NOT_FOUND) {
                printf("Host not found\n");
                return false;
            } else if (dwError == WSANO_DATA) {
                printf("No data record found\n");
                return false;
            } else {
                printf("Function failed with error: %ld\n", dwError);
                return false;
            }
        }
    } else {
        printf("Function returned:\n");
        printf("\tOfficial name: %s\n", remoteHost->h_name);
        printf("\tAlternate names: %s\n", remoteHost->h_aliases);
        printf("\tAddress type: ");
        switch (remoteHost->h_addrtype) {
        case AF_INET:
            printf("AF_INET\n");
            break;
        case AF_INET6:
            printf("AF_INET\n");
            break;
        case AF_NETBIOS:
            printf("AF_NETBIOS\n");
            break;
        default:
            printf(" %d\n", remoteHost->h_addrtype);
            break;
        }
        printf("\tAddress length: %d\n", remoteHost->h_length);
        addr.s_addr = *(u_long *) remoteHost->h_addr_list[0];
        printf("\tFirst IP Address: %s\n", inet_ntoa(addr));
    }

	// Create a command stream socket
	theSocketTCP = socket(remoteHost->h_addrtype,		//AF_INET,		// Address family
					   SOCK_STREAM,			        // Socket type
					   IPPROTO_TCP);		        // Protocol
	if (theSocketTCP == INVALID_SOCKET)
	{
		printf("ERROR_TCP: Unable to open socket\n");
		return false;
	}
	// Fill in the address structure
	SOCKADDR_IN saServer;
	saServer.sin_family = remoteHost->h_addrtype;
	saServer.sin_addr = addr;                                 //         *((LPIN_ADDR)*remoteHost->h_addr_list[0]);// ^ Server's address
	saServer.sin_addr.s_addr = *(u_long *) remoteHost->h_addr_list[0];
	saServer.sin_port = htons(TCPport);	
	// connect to the server

	nRet = connect(theSocketTCP,				// Socket
				   (LPSOCKADDR)&saServer,	// Server address
				   sizeof(struct sockaddr));// Length of server address structure
	if (nRet == SOCKET_ERROR)
	{
		err = WSAGetLastError();
		printf("ERROR_TCP: connect() error #%d\n", err);
		closesocket(theSocketTCP);
		return false;
	}

	int optval = 65536*128;
	int optsetval;
	int optlen = 4;
	int retval;
	retval = setsockopt(theSocketTCP, SOL_SOCKET, SO_RCVBUF, (char *)&optval, optlen);
	err = WSAGetLastError();
	if(retval == SOCKET_ERROR)
	{
		printf("ERROR_TCP: setsockopt() error #%d\n", err);
		closesocket(theSocketTCP);
		return false;
	}

	retval = getsockopt(theSocketTCP, SOL_SOCKET, SO_RCVBUF, (char *)&optsetval, &optlen);
	if(optval != optsetval)
	{				
		printf("ERROR_TCP: getsockopt() verify of setsockopt() failed\n");
		closesocket(theSocketTCP);
		return false;
	}
	printf("TCPsocket connected successfully.\n");
	return true;
}


bool TCPClass::sendTCPCommand(TCPcommandPacket *newPacket, bool verbose)
{
	// Send data to the scanner
   if (theSocketTCP==INVALID_SOCKET)
   {
      if (verbose)
         printf("ERROR_TCP: Socket is invalid\n");
      return false;
   }

   newPacket->checksum=calcCheckSum((unsigned char *)newPacket, sizeof(* newPacket)-sizeof(short));

	int nRet = send(theSocketTCP,				// Connected socket
				((const char *)newPacket),					// Data buffer
				sizeof(*newPacket),			// Length of data
				0);						// Flags
	if (nRet == SOCKET_ERROR)
	{
      if (verbose)
		   printf("ERROR_TCP: send()\n");
		closesocket(theSocketTCP);
       theSocketTCP=INVALID_SOCKET;
		return false;
	}

   return true;
}
int total = 0;

int TCPClass::receiveTCP(char * newPacket, int length, unsigned timeout)
{


	//select function with timeout
	fd_set socketset;
	struct timeval tv;
	int retval;


		FD_ZERO(&socketset);
		FD_SET(theSocketTCP,&socketset);

		// timed wait
		tv.tv_sec = 0; // number of seconds TIMEOUT
		tv.tv_usec = timeout; //TCP_SELECT_TIMEOUT; // number of microseconds TIMEOUT
		
		if(timeout == 0)
		{
			//printf("TCPClass: SELECT WAITING forever...\n");
			retval = select(1, &socketset, NULL, NULL, NULL); //wait forever
		}
		else
		{
			//printf("TCPClass: SELECT WAITING...\n");
			retval = select(1, &socketset, NULL, NULL, &tv); //waits for specified time for each select
		}
//		printf("TCPClass: Out of Select: %d retval\n", retval);
		if(retval == SOCKET_ERROR)
		{
//			printf("TCPClass: Error reported by select()\n");
			err = WSAGetLastError();
			return -1;
		}	
	
		// check if data has arrived
		if(retval == 0)
		{
//				printf("TCP timeout. No Message ready\n");
				return 0;
		}
		if (!FD_ISSET(theSocketTCP, &socketset))
		{
			printf("Select Error: Inconsistent return.\n");
				return -1;
		}

		int nRet;
	    total = 0;
		while(total < length)
		{
//			printf("TCPClass: Entering recv requesting %d bytes.\n", length-total);
			nRet = recv(theSocketTCP, 						// Connected client
				newPacket+total,							// Receive buffer
				length-total,								// Length of buffer
				0);

//			printf("recv returned %d bytes.\n",nRet);
			if(nRet == SOCKET_ERROR)
			{
				printf("TCPClass: recv error\n");
				int err = WSAGetLastError();
				
				return(-1);
			}
			else 
			{
//				memcpy(&tracebuf[grandTotal/2], newPacket+total, nRet);
				total+=nRet;
				grandTotal += nRet;
			}
		}
		if(grandTotal >= 200000)
		{
			grandTotal = 0;
			Ms++;
		}
		return(total); 
}

void TCPClass::closeTCP()
{
   // close the socket
   if (theSocketTCP != INVALID_SOCKET)
   	closesocket(theSocketTCP);
#if 0	
   // close the sockets
   if (remoteSocketTCP != INVALID_SOCKET)
   	closesocket(remoteSocketTCP);

      // close the sockets
   if (dataSocketTCP != INVALID_SOCKET)
   	closesocket(dataSocketTCP);
#endif
   // Release WinSock
   WSACleanup();
   return;
}
