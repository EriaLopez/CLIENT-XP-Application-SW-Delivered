
#include "stdafx.h"
#include "ScannerClass.h"
#include "BasePCGUI.h"
#include "BasePCGUIDlg.h"

#include <stdio.h>
#include "../CommonSrc/datapackets.h"
#include "math.h"

#define SPEEDCAL (2.9979246e8/.0254)  //speed of light in inches per second
#define ZERO_OFFSET (63.5) //tenths of an inch
#define COUNTS_PER_NS (65536.0/1132.3)

int writecount;

void ScannerClass::logBinaryScanLine(SRIPacket *packet, int fnum)
 {
	char fname[40];

	if (!s_DataFile) 
	{
		sprintf(fname,"SRI500BinaryFile%d", fnum);
		s_DataFile = fopen(fname,"wb");
		if(!s_DataFile)
			printf("Error opening binary file for writing, errno %d\n", errno);
	}
	//write the scan line header
	if (s_DataFile) 
	{
		fwrite((const void *)&(packet->lineHdr), sizeof(SRIPacketHdr),1,s_DataFile);
		//write the line of data
		fwrite((const void *)&(packet->lineData),sizeof(scanDataPerSample),packet->lineHdr.samplesPerLine,s_DataFile);
		writecount++;
	}
	return;
 }

//coefficients for parabolic signal strength correction to range
double p1 = -0.00000004251;
double p2 = 0.00743044421;

//mirror face angle corrections
double  azcorrFace1 = -3;
double 	azcorrFace2 = 6; //(-.125/301.0)*16000.0/TWO_PI;
double 	azcorrFace3 = 0; //(.25/301.0)*16000.0/TWO_PI;
double 	elcorrFace1 = ((11.0/16.0)/301.0)*8192/TWO_PI;
double 	elcorrFace2 = ((-9.0/16.0)/301.0)*8192/TWO_PI;
double 	elcorrFace3 = 0;

#define LGeom 4.646
#define rMirror .917
#define x0 1.416

#define	pi 3.14159265358979
#define	gamma (10.0*pi/180.0)
#define theta0 (pi/4 + gamma/2)

//Mirror geometry correction. Range must be in inches, encoder is mirror angle in radians
void calibratePoint(double range, double elInRadians, double *targetElevAngle, double *targetToOrigin)
{	
static int h0=0,VertAxisToMirrorAtHorizElev,SensorToMirrorAtHorizElev;

	if(range < 36)
	{
		*targetElevAngle = *targetToOrigin = 0; 
		return;
	}

	if(!h0)
	{	//some constant values
		h0 =( LGeom - rMirror*cos(theta0) + (x0  - rMirror*sin(theta0))*tan(theta0) )/(1+tan(gamma)*tan(theta0));
		VertAxisToMirrorAtHorizElev = x0 - h0*tan(gamma); // f when beam going out horiz
		SensorToMirrorAtHorizElev = h0/cos(gamma);
	}
	double theta = elInRadians + pi/4 + gamma/2;
	double h = (LGeom - rMirror*cos(theta) + (x0  - rMirror*sin(theta))*tan(theta) )/(1+tan(gamma)*tan(theta));
	double f = x0 - h*tan(gamma); //dist of reflection point from vertical axis 
	double reflectionHeight = h - LGeom; //dist of reflection point from mirror axis (pos up)
	double rayAngle = 2*theta - gamma; //relative to straight down
	rayAngle -= pi/2; //relative to horizontal, pos up going out
	double mirrorToSensor = h/cos(gamma);

    range = range + SensorToMirrorAtHorizElev - VertAxisToMirrorAtHorizElev;
	double reflToTarget = (double)range - mirrorToSensor; //should already be zero offset corrected
	double reflToOrigin = sqrt(f*f + reflectionHeight*reflectionHeight);
	double includedAngleC = pi - rayAngle - (pi/2-theta);
	*targetToOrigin = sqrt(reflToOrigin*reflToOrigin + reflToTarget*reflToTarget - 2*reflToTarget*reflToOrigin*cos(includedAngleC));
	double includedAngleB = acos((reflToOrigin*reflToOrigin + (*targetToOrigin) * (*targetToOrigin) - reflToTarget*reflToTarget)/(2* (*targetToOrigin)*reflToOrigin));
	
	*targetElevAngle = includedAngleB + theta - pi/2;
	*targetElevAngle = TWO_PI*0.25 - *targetElevAngle; //convert to vertical axis

}

typedef struct {
    double theta[MAX_SAMPLES_PER_LINE]; //azimuth angle, radians
	double phi[MAX_SAMPLES_PER_LINE]; //elev angle, radians
	double R[MAX_SAMPLES_PER_LINE]; //distance in inches
	int sigStr[MAX_SAMPLES_PER_LINE]; //return signal strength
}calibratedLine;

//calibrate and filter unreliable points
void calibrateLine(SRIPacket *packet, calibratedLine *calLine)
{
	int lastAzimuth, azimuthHi, correctedAz;
	//packet->lineHdr.azimuthHi is bits 30 thruough 15 (32 bit azimuth >> 15): test overlapping low bit of azimuthHi and hi bit of 16 bit sample az  
		
	lastAzimuth = packet->lineHdr.azimuth;
	azimuthHi = lastAzimuth / 65536;

	int azcorr, elcorr;
	double temp;
	if(packet->lineHdr.temperature)
		//convert to degrees C
		temp = (((packet->lineHdr.temperature-9929))*10.0)/1986.0; //198.6 ADC counts / C. Subtract 9929 to get from -50 (0 of sensor) to 0 C. Ans in thousandths of a C
	else
		temp = 0;

	if(packet->lineHdr.face == 1)
	{
		azcorr = (int)azcorrFace1;
		elcorr = (int)elcorrFace1;
	}
	else if(packet->lineHdr.face == 2)
	{
		azcorr = (int)azcorrFace2;
		elcorr = (int)elcorrFace2;
	}
	else
	{
		azcorr = (int)azcorrFace3;
		elcorr = (int)elcorrFace3;
	}
	for (int i = 0; i < packet->lineHdr.samplesPerLine; i++)
	{
		//combine low and hi words of azimuth 
		int azimuth = (lastAzimuth & 0xffff0000) | (packet->lineData[i].azimuthEncoder & 0x0000ffff);

		//point sample az data is only 16 bits so check for wrap
		if((azimuth - lastAzimuth) > 32768)
			azimuth -= 65536;
		if((azimuth - lastAzimuth) < -32768)
			azimuth += 65536;
		lastAzimuth = azimuth;
		correctedAz = azimuth + azcorr;

		//raw signal strength is inverted: higher value is lower sig str
		calLine->sigStr[i] = (double)(4*packet->lineData[i].signalStrength);
		if(calLine->sigStr[i] < 32000.0) calLine->sigStr[i] = 32000.0; //min (strongest) possible value 

		double signalStrCorrection = (p1*calLine->sigStr[i]*calLine->sigStr[i] + p2*calLine->sigStr[i]);//parabolic corr for sig str
		double temperatureCorrection = 0.25*(temp-43.5); //tenths of an inch //indicated range is lower at higher temps	
		double rawrange = packet->lineData[i].range - signalStrCorrection;
		double timeOfFlight = rawrange/COUNTS_PER_NS; //nanoseconds
		double range = (timeOfFlight*SPEEDCAL/1e8)/2.0 + temperatureCorrection + ZERO_OFFSET; //tenths of an inch at this point // div by 2 for round trip
		if(range < 0) range = 0;
		if(calLine->sigStr[i] > 55000) //not reliable above this (low signal)
			range = 0;
		if(range > 56000) //out of range
			range = 0;

		calLine->theta[i] = -correctedAz*2*pi/COUNTS_PER_AZ_ROTATION;

		int encoder = packet->lineHdr.elevationStart + elcorr - i*packet->lineHdr.elevSampleSpacing;
		double encoderInRadians = (double)encoder*2*pi/COUNTS_PER_ELEV_ROTATION;
		//adjust for mirror geometry (input in inches 
		calibratePoint(((double)range)/10.0, encoderInRadians, &(calLine->phi[i]), &(calLine->R[i]));
	}
}
int filenumber;

//complete calibration and transformation sequence 
//from a binary scan line file captured with logBinaryScanLine() 
//to a Cartesian list of XYZ points in inches. 
void ScannerClass::ScanToXYZ()
{
	SRIPacket packet;
	calibratedLine calLine;
	char fname[40];
	int nread;

	sprintf(fname,"SRI500BinaryFile%d", filenumber);
	s_DataFile = fopen(fname, "rb");

	int point = 0;
	double mindist = 1e10;
	double maxdist = 0;
	while(1)
	{
		fread((void *)&(packet.lineHdr), sizeof(SRIPacketHdr),1,s_DataFile);
		nread = fread((void *)&(packet.lineData), sizeof(scanDataPerSample),packet.lineHdr.samplesPerLine,s_DataFile);
		if(nread <= 0) break;

		calibrateLine(&packet, &calLine); //radius in is fixed point in tenths of an inch, out is double in inches
		double Sphi, R;
		for (int i = 0; i < packet.lineHdr.samplesPerLine; i++)
		{
			if(useDist && ((calLine.R[i] < Dmin) || (calLine.R[i] > Dmax)))
				continue;

			//signal str has an inverted scale
//			if((packet.lineData[i].signalStrength >= 32000) &&  (packet.lineData[i].signalStrength < 55000))
			{
				Sphi = sin(calLine.phi[i]);
				g_theGlobals.R[point] = R = calLine.R[i];
				g_theGlobals.X[point] = R*cos(calLine.theta[i])*Sphi; 
				g_theGlobals.Y[point] = R*sin(calLine.theta[i])*Sphi;
				g_theGlobals.Z[point] = R*cos(calLine.phi[i]);

				if(useX && ((g_theGlobals.X[point] < Xmin) || (g_theGlobals.X[point] > Xmax)))
					continue;
				if(useY && ((g_theGlobals.Y[point] < Ymin) || (g_theGlobals.Y[point] > Ymax)))
					continue;
				if(useZ && ((g_theGlobals.Z[point] < Zmin) || (g_theGlobals.Z[point] > Zmax)))
					continue;

				if(g_theGlobals.R[point] > maxdist)
					maxdist = g_theGlobals.R[point];
				if(g_theGlobals.R[point] < mindist)
					mindist = g_theGlobals.R[point];
				point++;
			}
		}
	}
	g_theGlobals.Npoints = point;
	g_theGlobals.minDist = mindist;
	g_theGlobals.maxDist = maxdist;
	fclose(s_DataFile);
}



void ScannerClass::CalibrateAndConvertToAscii()
{
	SRIPacket packet;
	char fname[40];
	int nread;
	FILE *AsciiFile;
	calibratedLine calLine;

	sprintf(fname,"SRI500BinaryFile%d", filenumber);
	s_DataFile = fopen(fname, "rb");

	sprintf(fname,"SRI500File%d.txt", filenumber);
	AsciiFile = fopen(fname,"w");

	while(1)
	{
		//read a line
		fread((void *)&(packet.lineHdr), sizeof(SRIPacketHdr),1,s_DataFile);
		nread = fread((void *)&(packet.lineData), sizeof(scanDataPerSample),packet.lineHdr.samplesPerLine,s_DataFile);
		if(nread > 0)
		{
			calibrateLine(&packet, &calLine);

			double temp;
			if(packet.lineHdr.temperature)
				//convert to degrees C
				temp = (((double)(packet.lineHdr.temperature-9929))*10.0)/1986.0; //198.6 ADC counts / C. Subtract 9929 to get from -50 (0 of sensor) to 0 C. Ans in thousandths of a C
			else
				temp = 0;

			//one scan line is one long ASCII line for easy matlab import
			// first the line header
			fprintf(AsciiFile, "%u %u %u %u %u %u %u %u %u %u %u %.3f %u %u ", 
				packet.lineHdr.lineNumber,
				packet.lineHdr.scanNumber,
				packet.lineHdr.ack.packetNumber,
				packet.lineHdr.ack.commandID,
				packet.lineHdr.ack.status,
				packet.lineHdr.packetTimeStamp, 				
				packet.lineHdr.elevSpeed, 	
				packet.lineHdr.errorCode,
				packet.lineHdr.samplesPerLine,				
				packet.lineHdr.elevSampleSpacing,
				packet.lineHdr.face, 
				temp,	
				packet.lineHdr.status,
				packet.lineHdr.checksum);

			for (int i = 0; i < packet.lineHdr.samplesPerLine; i++)
			{
				fprintf(AsciiFile, " %d %.3f %u %.4f %.4f",
					i,
					calLine.R[i], 
					calLine.sigStr[i],
					calLine.theta[i],
					calLine.phi[i]);				
			}
			fprintf(AsciiFile, "\n");
		}		
		else break;	
	}
	fclose(AsciiFile); 
	fclose(s_DataFile); 
	s_DataFile = NULL;
}