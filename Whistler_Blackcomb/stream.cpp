#include "./includes/modbus.h"
#include "./includes/stream.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
#include <Arduino.h>

#define MAX_NUM_STREAM_ADDR 128
#define MAX_NUM_STREAM_ADDR_PER_PKT 63  //When using Modbus Write/Read Multiple Registers 

static unsigned short gCurTransID = 0; //The current stream responses transaction ID

int readAinConfig(EthernetClient * sock, unsigned int numAddresses, unsigned int *scanListAddresses, unsigned short *nChannelList, float *rangeList)
{
	unsigned char data[4] = {0};
	unsigned int i = 0;

	//Check for scanListAddresses for valid AIN addresses.
	for(i = 0; i < numAddresses; i++)
	{
		if(scanListAddresses[i]%2 != 0 || scanListAddresses[i] > 508)
		{
			Serial.println("readAinConfig error: Invalid AIN address");
			return -1;
		}
	}

	for(i = 0; i < numAddresses; i++)
	{
		//Reading AIN range.
		//Starting address is 40000 (AIN0_RANGE).
		if(readMultipleRegistersTCP(sock, 40000 + scanListAddresses[i], 2, data) < 0)
			return -1;
		bytesToFloat(data, &rangeList[i]);

		//Reading AIN negative channels.
		//Starting address is 41000 (AIN0_NEGATIVE_CH).
		if(readMultipleRegistersTCP(sock, 41000 + scanListAddresses[i]/2, 1, data) < 0)
			return -1;
		bytesToUint16(data, &nChannelList[i]);
	}
	return 0;
}

int ainConfig(EthernetClient * sock, unsigned int numAddresses, const unsigned int *scanListAddresses, const unsigned short *nChannelList, const float *rangeList)
{
	unsigned char data[4] = {0};
	unsigned int i = 0;
	int ret = 0;
	unsigned short ljErr = 0;

	//Check for scanListAddresses for valid AIN addresses.
	for(i = 0; i < numAddresses; i++)
	{
		if(scanListAddresses[i]%2 != 0 || scanListAddresses[i] > 508)
		{
			Serial.println("ainConfig error: Invalid AIN address\n");
			return -1;
		}
	}

	for(i = 0; i < numAddresses; i++)
	{
		//Setting AIN range.
		//Starting address is 40000 (AIN0_RANGE).
		floatToBytes(rangeList[i], data);
		if((ret = writeMultipleRegistersTCP(sock, 40000 + scanListAddresses[i], 2, data)) < 0)
			goto handle_error;

		//Setting AIN negative channel.
		//Starting address is 41000 (AIN0_NEGATIVE_CH).
		uint16ToBytes(nChannelList[i], data);
		if((ret = writeMultipleRegistersTCP(sock, 41000 + scanListAddresses[i]/2, 1, data)) < 0)
			goto handle_error;
	}
	return 0;
handle_error:
	if(ret == -2)
	{
		//Get the LabJack specific error code.
		if(readLabJackError(sock, &ljErr) >= 0)
			Serial.println("ainConfig LabJack error");
	}
	return -1;
}

int readStreamConfig(EthernetClient * sock, float *scanRate, unsigned int *numAddresses, unsigned int *samplesPerPacket, float *settling, unsigned int *resolutionIndex, unsigned int *bufferSizeBytes, unsigned int *autoTarget, unsigned int *numScans)
{
	unsigned char data[24] = {0};

	//Read stream Configuration.

	//Starting at address 4002
	if(readMultipleRegistersTCP(sock, 4002, 12, data) < 0)
		return -1;
	bytesToFloat(data, scanRate); //Address = 4002 (STREAM_SCANRATE_HZ)
	bytesToUint32(&data[4], numAddresses); //Address = 4004 (STREAM_NUM_ADDRESSES)
	bytesToUint32(&data[8], samplesPerPacket); //Address = 4006 (STREAM_SAMPLES_PER_PACKET)
	bytesToFloat(&data[12], settling); //Address = 4008 (STREAM_SETTLING_US)
	bytesToUint32(&data[16], resolutionIndex); //Address = 4010 (STREAM_RESOLUTION_INDEX)
	bytesToUint32(&data[20], bufferSizeBytes); //Address = 4012 (STREAM_BUFFER_SIZE_BYTES)

	//Starting address is 4016.
	if(readMultipleRegistersTCP(sock, 4016, 2, data) < 0)
		return -1;
	bytesToUint32(data, autoTarget); //Address = 4016 (STREAM_AUTO_TARGET)

	//Starting address is 4020.
	if(readMultipleRegistersTCP(sock, 4020, 2, data) < 0)
		return -1;
	bytesToUint32(data, numScans); //Address = 4020 (STREAM_NUM_SCANS)

	return 0;
}

int readStreamAddressesConfig(EthernetClient * sock, unsigned int numAddresses, unsigned int *scanListAddresses)
{
	unsigned int i = 0;
	unsigned char data[252] = {0};
	unsigned short curRegAddr = 0;
	unsigned short curNumAddr = 0;

	if(numAddresses > MAX_NUM_STREAM_ADDR)
	{
		Serial.println("readStreamAddressesConfig error: Invalid numAddresses. ");
		return -1;
	}

	//Read stream scanlist. Starting address is 4100.
	curRegAddr = 4100; //Address = 4100 (STREAM_SCANLIST_ADDRESS0)
	while(numAddresses)
	{
		if(numAddresses/MAX_NUM_STREAM_ADDR_PER_PKT >= 1)
			curNumAddr = MAX_NUM_STREAM_ADDR_PER_PKT;
		else
			curNumAddr = numAddresses;

		//1 stream address = 2 registers

		if(readMultipleRegistersTCP(sock, curRegAddr, curNumAddr*2, data) < 0)
			return -1;
		for(i = 0; i < curNumAddr; i++)
			bytesToUint32(&data[i*4], &scanListAddresses[(curRegAddr-4100)/2 + i]);

		numAddresses -= curNumAddr;
		curRegAddr += curNumAddr*2;
	}
	return 0;
}

int streamConfig(EthernetClient * sock, float scanRate, unsigned int numAddresses, unsigned int samplesPerPacket, float settling, unsigned int resolutionIndex, unsigned int bufferSizeBytes, unsigned int autoTarget, unsigned int numScans, const unsigned int *scanListAddresses)
{
	unsigned char data[252] = {0};
	unsigned int i = 0;
	unsigned short curRegAddr = 0;
	unsigned short curNumAddr = 0;
	int ret = 0;
	unsigned short ljErr = 0;

	if(numAddresses > MAX_NUM_STREAM_ADDR)
	{
		Serial.println("streamConfig error: Invalid numAddresses.");
		return -1;
	}

	//Write Stream configuration.

	//Starting address is 4002.
	floatToBytes(scanRate, data); //Address = 4002 (STREAM_SCANRATE_HZ)
	uint32ToBytes(numAddresses, &data[4]); //Address = 4004 (STREAM_NUM_ADDRESSES)
	uint32ToBytes(samplesPerPacket, &data[8]); //Address = 4006 (STREAM_SAMPLES_PER_PACKET)
	floatToBytes(settling, &data[12]); //Address = 4008 (STREAM_SETTLING_US)
	uint32ToBytes(resolutionIndex, &data[16]); //Address = 4010 (STREAM_RESOLUTION_INDEX)
	uint32ToBytes(bufferSizeBytes, &data[20]); //Address = 4012 (STREAM_BUFFER_SIZE_BYTES)
	if((ret = writeMultipleRegistersTCP(sock, 4002, 12, data)) < 0)
		goto handle_error;

	//Starting address is 4016.
	uint32ToBytes(autoTarget, data); //Address = 4016 (STREAM_AUTO_TARGET)
	uint32ToBytes(0, &data[4]); //Address = 4018 (STREAM_DATATYPE: Set to 0)
	uint32ToBytes(numScans, &data[8]); //Address = 4020 (STREAM_NUM_SCANS)
	if((ret = writeMultipleRegistersTCP(sock, 4016, 6, data)) < 0)
		goto handle_error;

	//Starting at address 4100.
	curRegAddr = 4100; //STREAM_SCANLIST_ADDRESS0
	while(numAddresses)
	{
		if(numAddresses/MAX_NUM_STREAM_ADDR_PER_PKT >= 1)
			curNumAddr = MAX_NUM_STREAM_ADDR_PER_PKT;
		else
			curNumAddr = numAddresses;

		for(i = 0; i < curNumAddr; i++)
			uint32ToBytes(scanListAddresses[(curRegAddr-4100)/2 + i], &data[i*4]);
		if((ret = writeMultipleRegistersTCP(sock, curRegAddr, curNumAddr*2, data)) < 0)
			goto handle_error;

		numAddresses -= curNumAddr;
		curRegAddr += curNumAddr*2;
	}
	return 0;

handle_error:
	if(ret == -2)
	{
		//Get the LabJack specific error code.
		if(readLabJackError(sock, &ljErr) >= 0)
			Serial.println("streamConfig LabJack error:");
			Serial.println(ljErr);
	}
	return -1;
}

//Used to start (1) and stop (0) the stream.
int streamEnable(EthernetClient * sock, unsigned int enable)
{
	unsigned char data[4] = {0};
	int ret = 0;
	unsigned short ljErr = 0;

	//Write to address 4990
	uint32ToBytes(enable, data); //Address = 4990 (STREAM_ENABLE)
	if((ret = writeMultipleRegistersTCP(sock, 4990, 2, data)) < 0)
	{
		if(ret == -2)
		{
			//Get the LabJack specific error code.
			if(readLabJackError(sock, &ljErr) >= 0)
				Serial.println("streamEnable LabJack error");
				Serial.println(ljErr);
		}
		return -1;
	}
	return 0;
}

int streamStart(EthernetClient * sock)
{
	gCurTransID = 0; //Reset the current stream transaction ID
	return streamEnable(sock, 1);
}

int spontaneousStreamRead(EthernetClient * sock, unsigned int samplesPerPacket, unsigned short *backlog, unsigned short *status, unsigned short *additionalInfo, unsigned char *rawData)
{
	/*
	Modbus Feedback Response:
		Bytes 0-1: Transaction ID
		Bytes 2-3: Protocol ID
		Bytes 4-5: Length, MSB-LSB
		Byte 6: 1 (Unit ID)
		Byte 7: 76 (Function #)
		Byte 8: 16
		Byte 9: Reserved
		Byte 10-11: Backlog
		Byte 12-13: Status Code
		Byte 14-15: Additional status information
		Byte 16+: Stream Data (raw sample = 2 bytes MSB-LSB)

	Status Codes:
		2940: Auto Recovery Active.
		2941: Auto Recovery End. Additional Status Information is the number of scans skipped.
		2942: Scan Overlap
		2343: Auto Recovery End Overflow
	*/

	const unsigned char STREAM_TYPE = 16;
	unsigned char *res;
	int resSize = 0;
	int size = 0;

	int ret = 0;
	resSize = 16+samplesPerPacket*STREAM_BYTES_PER_SAMPLE;
	res = (unsigned char *)malloc(resSize); //Could also hardcode for max. size 1040

	size = readTCP(sock, res, resSize);
	if(size <= 0)
	{
		ret = -1;
		goto end;
	}

	//Check the response for errors and make sure the transaction ID is the expected one.
	//The transaction ID increments in the response packets. If a transaction ID is skipped,
	//that could indicate missing packets.
	if(checkModbusResponse(res, size, gCurTransID, 76) != 0)
	{
		ret = -1;
		goto end;
	}
	gCurTransID++; //Expected next transaction ID 
	
	if(res[8] != STREAM_TYPE)
	{
		Serial.println("arStreamRead error: Unexpected stream type");
		printPacket(res, resSize);
		ret = -1;
		goto end;
	}

	//res[9]; //reserved
	bytesToUint16(&res[10], backlog);
	bytesToUint16(&res[12], status);
	bytesToUint16(&res[14], additionalInfo);

	//streamData
	memcpy(rawData, &res[16], samplesPerPacket*STREAM_BYTES_PER_SAMPLE);
	ret = 0;
end:
	free(res);
	return ret;
}

int streamStop(EthernetClient * sock)
{
	return streamEnable(sock, 0);
}

int streamStatusCheck(unsigned short Status, int * gQuit){
	//Check Status
	if(Status == STREAM_STATUS_SCAN_OVERLAP)
	{
		//Stream scan overlap occured. This usually indicates the scan rate
		//is too fast for the stream configuration.
		//Stopping the stream.
		Serial.println("\nReceived stream Status error 2942 - STREAM_SCAN_OVERLAP. Stopping stream.\n");
		gQuit = 1;
	}
	else if(Status == STREAM_STATUS_AUTO_RECOVER_END_OVERFLOW)
	{
		//During auto recovery the skipped samples counter (16-bit) overflowed.
		//Stopping the stream because of unknown amount of skipped samples.
		Serial.println("\nReceived stream Status error 2943 - STREAM_AUTO_RECOVER_END_OVERFLOW. Stopping stream.\n");
		gQuit = 1;
	}
	else if(Status == STREAM_STATUS_AUTO_RECOVER_ACTIVE)
	{
		//Stream buffer overload occured. In auto recovery mode. Continue
		//reading existing samples from the T7's stream buffer which is still valid.
		Serial.println("\nReceived stream Status 2940 - STREAM_AUTO_RECOVER_ACTIVE.\n");
	}
	else if(Status == STREAM_STATUS_AUTO_RECOVER_END)
	{
		//Auto recover mode has ended. The number of skipped scans are reported
		//and new samples are coming in.
		//numScansSkipped += (double)additionalInfo; //# skipped scans
		Serial.println("\nReceived stream Status 2941 - STREAM_AUTO_RECOVER_END. %u scans were skipped.\n");
	}
	else if(Status == STREAM_STATUS_BURST_COMPLETE)
	{
		//Stream burst has completed. Status used when numScans
		//(Address 4020 - STREAM_NUM_SCANS) is configured to a non-zero value.
		Serial.println("Stream burst has completed\n");
		gQuit = 1;
	}
	else if(Status != 0)
	{
		Serial.println("\nReceived nonstandard stream Status\n");
	}
}