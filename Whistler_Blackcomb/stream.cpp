#include "./includes/modbus.h"
#include "./includes/calibration.h"
#include "./includes/stream.h"
#include <Arduino.h>

#define MAX_NUM_STREAM_ADDR 128
#define MAX_NUM_STREAM_ADDR_PER_PKT 63  //When using Modbus Write/Read Multiple Registers 

int labjackSetup(struct LJConfig * config, void * devCal){

	Serial.println((&config->crSock)->connect(config->IP_ADDR, config->CR_PORT));

	Serial.println((&config->arSock)->connect(config->IP_ADDR, config->SP_PORT));
	
	// Check if the stream is still running from a previous run, if so reset it and flush the buffer of reads
	byte readData[4]={0};
	readMultipleRegistersTCP((&config->crSock), 4990, 2, readData);
	if(readData[3]==1){
		Serial.println("Stream reset since it was already running. ");
		writeMultipleRegistersTCP((&config->crSock), 4990, 2, 0);
		while((&config->arSock)->available()){
			(&config->arSock)->read();
		}
		delay(1000);
	}

	// Get calibration from flash memory of Labjack. 
	// Note on void pointer: since this function should work on both a T7 and a T4, either device calibration should be expected. Therefore a void pointer of either calibration type is passed in, and it is casted to the correct type depending on what the config struct says it should be. 
	if(config->type == 7){
		getCalibrationT7((&config->crSock), (DeviceCalibrationT7 *)devCal);
	}else{
		getNominalCalibrationT4((DeviceCalibrationT4 *)devCal);
	}
	
	Serial.println("Configuring analog inputs.\n");
	if(ainConfig((&config->crSock), config->numAddresses, config->scanListAddresses, config->nChanList, config->rangeList) != 0)
		Serial.println("ainConfig failed");
	

	Serial.println("Configuring stream settings.\n");
	if(streamConfig((&config->crSock), config->scanRate, config->numAddresses, config->samplesPerPacket, config->settling, config->resolutionIndex, config->bufferSizeBytes, config->autoTarget, config->numScans, config->scanListAddresses) != 0)
	{
		Serial.println("streamConfig failed");
	}

	//Read back stream settings
	Serial.println("Reading stream configuration.\n");
	if(readStreamConfig((&config->crSock), &config->scanRate, &config->numAddresses, &config->samplesPerPacket, &config->settling, &config->resolutionIndex, &config->bufferSizeBytes, &config->autoTarget, &config->numScans) != 0)
		Serial.println("StreamConfig not read correctly!");

	Serial.println("Reading stream scan list.\n");
	if(readStreamAddressesConfig((&config->crSock), config->numAddresses, config->scanListAddresses) != 0)
		Serial.println("readSreamAddressesConfig failed");
	
	Serial.println("Reading analog inputs configuration.\n");
	if(readAinConfig((&config->crSock), config->numAddresses, config->scanListAddresses, config->nChanList, config->rangeList) != 0)
		Serial.println("readAinAddressesConfig failed");
		
	
	Serial.println("Starting Stream...");
	if(streamStart((&config->crSock), &config->gCurTransID) != 0)
	{
		Serial.println("Stopping stream\n");
		streamStop((&config->crSock));
	}

	return 0;
}

int labjackRead(struct LJConfig * config, void * devCal, float * data){
	
	double startTime=getTimeSec();

	int gQuit=0;

	//Stream read returns
	unsigned short backlog = 0;
	unsigned short Status = 0;
	unsigned short additionalInfo = 0;

	//Stream read loop variables
	float volts = 0.0f;
	int printStream = 1;

	unsigned char rawData[config->samplesPerPacket*STREAM_BYTES_PER_SAMPLE];
	float voltData[config->samplesPerPacket*STREAM_BYTES_PER_SAMPLE];

	// Checks to see if either socket has been disconnected, if so try and reconnect (doesn't currently really work)
	if(!(&config->crSock)->connected()){
		(&config->crSock)->connect(config->IP_ADDR, config->CR_PORT);
		Serial.println("crSock Disconnected!");
		while((&config->crSock)->available()){
			(&config->crSock)->read();
			}
	}
	if(!(&config->arSock)->connected()){
		(&config->arSock)->connect(config->IP_ADDR, config->SP_PORT);
		Serial.println("arSock Disconnected!");
		while((&config->arSock)->available()){
		(&config->arSock)->read();
		}
	}
	
	backlog = 0;
	Status = 0;
	additionalInfo = 0;

	// Waits until a read is available at which time returned data will be written to rawData
	while(!(&config->arSock)->available())
		delay(1);
	if(spontaneousStreamRead((&config->arSock), config->samplesPerPacket, &config->gCurTransID, &backlog, &Status, &additionalInfo, rawData) != 0)
	{
		Serial.println("Stream read failed");
	}
	backlog = backlog / (config->numAddresses*STREAM_BYTES_PER_SAMPLE); //Scan backlog
	streamStatusCheck(Status, &gQuit);

	// Loops through the returned data and converts each measurement to volts
	for(unsigned int j=0;j<config->numAddresses;j++){
		if(config->type == 7){
			ainBinToVoltsT7((DeviceCalibrationT7 *)devCal, &rawData[j*STREAM_BYTES_PER_SAMPLE], config->gainList[j], &volts);
		}else{
			ainBinToVoltsT4((DeviceCalibrationT4 *)devCal, &rawData[j*STREAM_BYTES_PER_SAMPLE], config->gainList[j], &volts);
		}
		if(printStream)
			Serial.println(volts, 7);
		voltData[j]=volts;
	}

	memcpy(data, &voltData[0], config->samplesPerPacket*STREAM_BYTES_PER_SAMPLE);

	if(printStream){
		Serial.println("Time spent:");
		Serial.println(getTimeSec()-startTime);
	}
	if(gQuit){
		free(rawData);
		Serial.println("Stopping Stream...");
		streamStop((&config->crSock));
		for(;;);
	}

	// readMultipleRegistersTCP(crSock, 0, 2, testData);
	// bytesToFloat(testData, &test);
	// Serial.println(test);

	return 0;
}

void configT7(struct LJConfig * config){
	config->CR_PORT = 502;
	config->SP_PORT = 702;
	config->IP_ADDR = IPAddress(192, 168, 1, 214);

	config->type = 7;

	config->scanRate = 100.0f;
	config->numAddresses = 3;
	// This is to ensure that the number of samples in a packet is divisible by the number of addresses, so the "first" piece of data is always consistently first in the array
	config->samplesPerPacket = STREAM_MAX_SAMPLES_PER_PACKET_TCP - STREAM_MAX_SAMPLES_PER_PACKET_TCP % config->numAddresses;
	config->settling = 10.0;
	config->resolutionIndex = 8;
	config->autoTarget = STREAM_TARGET_ETHERNET;
	config->numScans = 0;

	
	// Temperature sensor reading - reserved 1st index of scanListAddresses
	config->scanListAddresses[0]=28;
	config->nChanList[0] = 199; //Negative channel is 199 (single ended)
	config->rangeList[0] = 10; //0.0 = +/-10V, 10.0 = +/-10V, 1.0 = +/-1V, 0.1 = +/-0.1V, or 0.01 = +/-0.01V.
	config->gainList[0] = 0; //gain index 0 = +/-10V


	// Using a loop to add Modbus addresses for AIN0 - AIN(NUM_ADDRESSES-1) to the
	// stream scan and configure the analog input settings.
	// Thermocouple settings: nChanList=199, rangeList=0.01, gainList=3
	for(unsigned int i = 1; i < config->numAddresses; i++)
	{
		config->scanListAddresses[i] = (i-1)*2; //AIN(i) (Modbus address i*2)
		config->nChanList[i] = 199; //Negative channel is 199 (single ended)
		config->rangeList[i] = 0.01; //0.0 = +/-10V, 10.0 = +/-10V, 1.0 = +/-1V, 0.1 = +/-0.1V, or 0.01 = +/-0.01V.
		config->gainList[i] = 3; //gain index 0 = +/-10V 
	}
	
}

void configT4(struct LJConfig * config){
	config->CR_PORT = 502;
	config->SP_PORT = 702;
	config->IP_ADDR = IPAddress(192, 168, 1, 215);

	config->type = 4;

	config->scanRate = 1000.0f;
	config->numAddresses = 1;
	// This is to ensure that the number of samples in a packet is divisible by the number of addresses, so the "first" piece of data is always consistently first in the array
	config->samplesPerPacket = STREAM_MAX_SAMPLES_PER_PACKET_TCP - STREAM_MAX_SAMPLES_PER_PACKET_TCP % config->numAddresses;
	config->settling = 10.0;
	config->resolutionIndex = 5;
	config->autoTarget = STREAM_TARGET_ETHERNET;
	config->numScans = 0;

	// Temp test sensor
	config->scanListAddresses[0]=0;
	config->nChanList[0] = 199; //Negative channel is 199 (single ended)
	config->rangeList[0] = 10; //0.0 = +/-10V, 10.0 = +/-10V, 1.0 = +/-1V, 0.1 = +/-0.1V, or 0.01 = +/-0.01V.
	config->gainList[0] = 0; //gain index 0 = +/-10V
	
}

int configTC(EthernetClient * sock, unsigned int numSensors, const unsigned int * posAINList, const unsigned short * negAINList, const unsigned int * typeList){

	unsigned char data[2] = {0};

	for(unsigned int i = 0; i < numSensors; i++){
		uint16ToBytes(negAINList[i], data);
		writeMultipleRegistersTCP(sock, 41000+posAINList[i], 1, data);

		uint32ToBytes(typeList[i], data);
		writeMultipleRegistersTCP(sock, 9000+2*posAINList[i], 2, data);
		
		uint32ToBytes(0, data);
		writeMultipleRegistersTCP(sock, 9300+2*posAINList[i], 2, data);

		uint32ToBytes(60052, data);
		writeMultipleRegistersTCP(sock, 9600+2*posAINList[i], 2, data);

		floatToBytes(1.0, data);
		writeMultipleRegistersTCP(sock, 10200+2*posAINList[i], 2, data);

		floatToBytes(0.0, data);
		writeMultipleRegistersTCP(sock, 10500+2*posAINList[i], 2, data);
	}

	// readMultipleRegistersTCP(crSock, 7000, 2, testData);
	// bytesToFloat(testData, &test);
	// Serial.println(test);

	return 0;
}

double getTimeSec()
{
  // If more time resolution is wanted, use this instead. However, the buffer will overflow after only 70 minutes, so be careful: 
  // return micros()/1000000.0;
  return millis()/1000.0;
}

int readAinConfig(EthernetClient * sock, unsigned int numAddr, unsigned int *scanListAddresses, unsigned short *nChannelList, float *rangeList)
{
	unsigned char data[4] = {0};
	unsigned int i = 0;

	//Check for scanListAddresses for valid AIN addresses.
	for(i = 0; i < numAddr; i++)
	{
		if(scanListAddresses[i]%2 != 0 || scanListAddresses[i] > 508)
		{
			Serial.println("readAinConfig error: Invalid AIN address");
			Serial.println(scanListAddresses[i]);
			return -1;
		}
	}

	for(i = 0; i < numAddr; i++)
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

int ainConfig(EthernetClient * sock, unsigned int numAddr, const unsigned int *scanListAddresses, const unsigned short *nChannelList, const float *rangeList)
{
	unsigned char data[4] = {0};
	unsigned int i = 0;
	int ret = 0;
	unsigned short ljErr = 0;

	//Check for scanListAddresses for valid AIN addresses.
	for(i = 0; i < numAddr; i++)
	{
		if(scanListAddresses[i]%2 != 0 || scanListAddresses[i] > 508)
		{
			Serial.println("ainConfig error: Invalid AIN address\n");
			return -1;
		}
	}

	for(i = 0; i < numAddr; i++)
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

int readStreamConfig(EthernetClient * sock, float *scanRate, unsigned int *numAddr, unsigned int *samplesPerPac, float *settling, unsigned int *resolutionIndex, unsigned int *bufferSizeBytes, unsigned int *autoTarget, unsigned int *numScans)
{
	unsigned char data[24] = {0};

	//Read stream Configuration.

	//Starting at address 4002
	if(readMultipleRegistersTCP(sock, 4002, 12, data) < 0)
		return -1;
	bytesToFloat(data, scanRate); //Address = 4002 (STREAM_SCANRATE_HZ)
	bytesToUint32(&data[4], numAddr); //Address = 4004 (STREAM_NUM_ADDRESSES)
	bytesToUint32(&data[8], samplesPerPac); //Address = 4006 (STREAM_SAMPLES_PER_PACKET)
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

int readStreamAddressesConfig(EthernetClient * sock, unsigned int numAddr, unsigned int *scanListAddresses)
{
	unsigned int i = 0;
	unsigned char data[252] = {0};
	unsigned short curRegAddr = 0;
	unsigned short curNumAddr = 0;

	if(numAddr > MAX_NUM_STREAM_ADDR)
	{
		Serial.println("readStreamAddressesConfig error: Invalid numAddresses. ");
		return -1;
	}

	//Read stream scanlist. Starting address is 4100.
	curRegAddr = 4100; //Address = 4100 (STREAM_SCANLIST_ADDRESS0)
	while(numAddr)
	{
		if(numAddr/MAX_NUM_STREAM_ADDR_PER_PKT >= 1)
			curNumAddr = MAX_NUM_STREAM_ADDR_PER_PKT;
		else
			curNumAddr = numAddr;

		//1 stream address = 2 registers

		if(readMultipleRegistersTCP(sock, curRegAddr, curNumAddr*2, data) < 0)
			return -1;
		for(i = 0; i < curNumAddr; i++)
			bytesToUint32(&data[i*4], &scanListAddresses[(curRegAddr-4100)/2 + i]);

		numAddr -= curNumAddr;
		curRegAddr += curNumAddr*2;
	}
	return 0;
}

int streamConfig(EthernetClient * sock, float scanRate, unsigned int numAddr, unsigned int samplesPerPac, float settling, unsigned int resolutionIndex, unsigned int bufferSizeBytes, unsigned int autoTarget, unsigned int numScans, const unsigned int *scanListAddresses)
{
	unsigned char data[252] = {0};
	unsigned int i = 0;
	unsigned short curRegAddr = 0;
	unsigned short curNumAddr = 0;
	int ret = 0;
	unsigned short ljErr = 0;

	if(numAddr > MAX_NUM_STREAM_ADDR)
	{
		Serial.println("streamConfig error: Invalid numAddresses.");
		return -1;
	}

	//Write Stream configuration.

	//Starting address is 4002.
	floatToBytes(scanRate, data); //Address = 4002 (STREAM_SCANRATE_HZ)
	uint32ToBytes(numAddr, &data[4]); //Address = 4004 (STREAM_NUM_ADDRESSES)
	uint32ToBytes(samplesPerPac, &data[8]); //Address = 4006 (STREAM_SAMPLES_PER_PACKET)
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
	while(numAddr)
	{
		if(numAddr/MAX_NUM_STREAM_ADDR_PER_PKT >= 1)
			curNumAddr = MAX_NUM_STREAM_ADDR_PER_PKT;
		else
			curNumAddr = numAddr;

		for(i = 0; i < curNumAddr; i++)
			uint32ToBytes(scanListAddresses[(curRegAddr-4100)/2 + i], &data[i*4]);
		if((ret = writeMultipleRegistersTCP(sock, curRegAddr, curNumAddr*2, data)) < 0)
			goto handle_error;

		numAddr -= curNumAddr;
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

int streamStart(EthernetClient * sock, unsigned short * transID)
{
	transID = 0; //Reset the current stream transaction ID
	return streamEnable(sock, 1);
}

int spontaneousStreamRead(EthernetClient * sock, unsigned int samplesPerPac, unsigned short * transID, unsigned short *backlog, unsigned short *status, unsigned short *additionalInfo, unsigned char *rawData)
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
	resSize = 16+samplesPerPac*STREAM_BYTES_PER_SAMPLE;
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
	if(checkModbusResponse(res, size, *transID, 76) != 0)
	{
		ret = -1;
		goto end;
	}
	(*transID)++; //Expected next transaction ID 
	
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
	memcpy(rawData, &res[16], samplesPerPac*STREAM_BYTES_PER_SAMPLE);
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
		//gQuit = 1;
	}
	else if(Status == STREAM_STATUS_AUTO_RECOVER_END_OVERFLOW)
	{
		//During auto recovery the skipped samples counter (16-bit) overflowed.
		//Stopping the stream because of unknown amount of skipped samples.
		Serial.println("\nReceived stream Status error 2943 - STREAM_AUTO_RECOVER_END_OVERFLOW. Stopping stream.\n");
		//gQuit = 1;
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
		//gQuit = 1;
	}
	else if(Status != 0)
	{
		Serial.println("\nReceived nonstandard stream Status\n");
	}
	return 0;
}
