//DO NOT EDIT
//This file is shared with the embedded software in the scanner.
#ifndef DATAPACKETS_H
#define DATAPACKETS_H

#define SCANNER_ID "SRI500" 
#define SERVER_PORT 9760 //command/status TCP port for scanner
#define TCP_PORT "9760" //the same but in quotes for client
#define SCANNER_DATA_PORT 9761  //data TCP port for scanner
#define TCPDATA_PORT "9761" //the same but in quotes for client

#define COUNTS_PER_ELEV_ROTATION 16384
#define COUNTS_PER_AZ_ROTATION 16000

#define BUFLENBYTES (512*sizeof(SRIPacketHdr)) // 24096 bytes for 502     502*sizeof(SRIPacketHdr)) //in chars must be divisible by 8. 502 is 3001 samples plus 1 header, plus a little
#define BUFLEN (BUFLENBYTES/2)  //3001 points + four line headers, in shorts

//MIN AND MAX LIMITS OF COMMAND PARAMETERS ARE CHECKED IN THE SCANNER

#define MIN_ELEV_START -1500
#define MAX_ELEV_START 1500

#define MAX_ELEV_SPEED 2400000
#define MIN_ELEV_SPEED 100000

#define MIN_AZIMUTH_SPEED (-48000)
#define MAX_AZIMUTH_SPEED (48000) //3 rev/sec in encoder counts

#define SLOWEST_AZIMUTH_SPEED 10


//Scanner State values
#define OK_SLEEPING 1
#define OK_WAKING 2
#define OK_READY 3
#define OK_STARTING_SCAN 4
#define OK_WAITING_ELEV_MOTOR_SPEED 5
#define OK_WAITING_AZ_MOTOR 6
#define OK_IN_FIRST_SCAN 7
#define OK_IN_LAST_SCAN 8
#define OK_SCAN_FINISHED 9
#define NO_RESPONSE 10	//used on client side only
#define ACK_WAIT_FAILED 11  //used on client side only
#define ERROR_HALT 12

//Internal scanner error codes

#define CHANNEL_1_OVERFLOW 17
#define CHANNEL_2_OVERFLOW 18
#define SPI_RCV_OVERFLOW 19
#define NO_EL_MOTOR_RESPONSE 20
#define MISSED_INDEX_PULSE 21
#define NO_MOTOR_RESPONSE 22
#define UART_ERROR 23
#define USB_COMM_BAD_TRANSFER_HANDLE 24
#define NO_TDC1_COMM 25
#define NO_TDC2_COMM 26
#define BAD_CMD_CHECKSUM 27
#define SPI_TDC1_INIT_FAIL 28
#define SPI_TDC2_INIT_FAIL 29
#define NO_COMM_ELEV_MOTOR 30
#define NO_COMM_AZ_MOTOR 31
#define ELEV_MOTOR_SPEED_FAIL 32
#define AZ_MOTOR_SPEED_FAIL 33
#define SPI1_ERROR 34
#define SPI2_ERROR 35
#define AZIMUTH_SCAN_SPEED_SETUP_FAILURE 36
#define AZIMUTH_SCAN_FAILURE 37
#define AZ_MOTOR_STOP_CMD_FAILURE 38
#define OVERTEMP 39
#define ELEV_SPEED_SET_FAILURE 40
#define AZIMUTH_SPEED_SET_FAILURE 41
#define AZIMUTH_POSITION_SET_FAILURE 42
#define COMMAND_ACK_TIMEOUT 43
#define CLIENT_WAIT_FAILED 44
#define ELEV_MOTOR_HOME_TIMEOUT 45
#define AZ_MOTOR_HOME_TIMEOUT 46
#define DMA_ERROR 47
#define SCAN_END_TIMEOUT 48
#define ERRSTACK_OVERFLOW 49
#define CLIENT_SEND_FAILED 50

//command ack error codes
#define CMD_RCVD_OK 0
#define INVALID_START_POINT 150  // not between -1500 and 1500
#define INVALID_FINAL_POINT 151  // elevationStart minus pointsPerLine, and elevPointSpace is less than -1500
#define INVALID_POINT_SPACING 152 // elevSpeed and elevPointSpace results in too much time between points
#define INVALID_CMD 153          //commandID value not recognized
#define INVALID_ELEV_START 154
#define INVALID_ELEV_SPEED 155
#define INVALID_SAMPLE_RATE 156
#define INVALID_AZIMUTH_START 158
#define INVALID_AZIMUTH_END 159
#define INVALID_AZIMUTH_PARAMS 160
#define INVALID_AZIMUTH_SPEED 161
#define INVALID_POINTS_PER_LINE 164
#define INVALID_ELEVATION_START 165
#define SCANNER_NOT_READY_FOR_COMMAND 166
#define BLANK_LINE 168
#define LASER_NOT_IDLE 169
#define MISMATCHED_COMMAND 170
#define NO_LINE_PARAMS_RECD 171
#define NO_AZIMUTH_PARAMS_RECD 172

//WARNING CODES
#define SERVO_ACK_TIMEOUT 201
#define SERVO_ERROR_ACK 202
#define SERVO_ECHO_TIMEOUT 203
#define SERVO_ECHO_MISMATCH 204

//longest possible command packet: Not all fields used for every command
typedef struct {
   unsigned packetNumber;
   unsigned short commandID;
   int azimuthPosn;
   int azimuthStart;
   int azimuthEnd;
   int azimuthSpeed;
   int elevationStart;
   int samplesPerLine;
   int elevSpeed;
   int elevSampleSpacing;
   int numberOfScans;
   unsigned short checksum;
} TCPcommandPacket;

// For samples in  scan line
#define MAX_SAMPLES_PER_LINE   3001

typedef struct{
	unsigned short range;  
	unsigned short signalStrength;
    unsigned short temp;
    short azimuthEncoder;
} scanDataPerSample;

//reply by scanner to commands or inquiries received
typedef struct
{
   unsigned packetNumber; //matches the packet number in the command packet
   unsigned short status;
   unsigned short commandID;
} TCPstatusPacket;

//48bytes: That is header size in transmitted packet. Must be multiple of 2xsample size: Each sample is 8 bytes so 6 samples
typedef struct {
	// once per line
	unsigned lineNumber; //at 0
	unsigned scanNumber; //at 4
	TCPstatusPacket ack; //at 8  8 bytes long 
	unsigned packetTimeStamp; // at 16
    unsigned elevSpeed; //vert motor speed, in counts per sec //at 20 bytes
	unsigned errorCode; //most recent error         //at 24 bytes
    int azimuth; //at 28 
	unsigned short samplesPerLine; //at offset 32   number of points per scan line
	unsigned short elevSampleSpacing; //34 number of encoder counts between each sample
	short elevationStart; //36 vert encoder value at beginning of this line
 	unsigned short face; //38
    unsigned short temperature; //40
	unsigned short status; //42
	unsigned short reserved; //44
	unsigned short checksum; //at 46
}SRIPacketHdr;

typedef struct {
    SRIPacketHdr lineHdr;
    scanDataPerSample lineData[MAX_SAMPLES_PER_LINE];
}SRIPacket;

//COMMAND ID DEFINITIONS FOR TCP MESSAGES
enum TCPcommandIDs {
    nullCommandID = 0,
	azimuthParametersCommandID,
	elevationParametersCommandID,
	azimuthPositionCommandID,
	azimuthSpeedCommandID,
	elevationSpeedCommandID,
	numberOfScansCommandID,
	singleScanCommandID,
	repeatingScanCommandID,
	haltScanCommandID,
	abortScanCommandID,
	laserEnableCommandID,
	laserDisableCommandID,
	scanSleepCommandID,
	StatusCommandID,        //brief status, 1 value
    LogCommandID  //detailed state and history. Fills scan data, can only be used when not scanning
};

typedef struct {
   SRIPacketHdr lineHeader;
   int azimuthPosn;
   int azimuthSpeed;
   int azimuthStart;
   int azimuthEnd;
}scanParams;

//in the scan data region, in response to Log Command
typedef struct
{
    scanParams currentScanParameters;
    scanParams nextScanParameters;
    int azimuthPosition;
    int azimuthSpeed;
    int elevationSpeed;
	int temperature;
    int hv;
	int th;
	int calValue1;
	int calValue2;
    int nerrors;
    short errlist[5000];
} ScannerLog;


#endif