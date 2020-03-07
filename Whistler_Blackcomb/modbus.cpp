#include "includes/modbus.h"
#include <Arduino.h>

#define CPU_BIG_ENDIAN 2
#define CPU_LITTLE_ENDIAN 1
#define CPU_ENDIAN_NOT_CHECKED 0

static int CPU_ENDIAN =	CPU_ENDIAN_NOT_CHECKED;

//This reverses the data's byte order for little endian	processors. This is
//useful for converting data types to/from big endian.
void correctEndian(void *data, int numBytes)
{
	unsigned char temp = 0;
	int i = 0;
	int maxIndex = 0;
	unsigned short test = 0;
	unsigned char *bytes = (unsigned char*)data;

	if(CPU_ENDIAN == CPU_ENDIAN_NOT_CHECKED)
	{
		//figure out endianess
		test = 0x0001;
		if(*((unsigned char *)(&test)) == 0x01)
			CPU_ENDIAN = CPU_LITTLE_ENDIAN;
		else
			CPU_ENDIAN = CPU_BIG_ENDIAN;
	}
	
	if(CPU_ENDIAN == CPU_BIG_ENDIAN)
		return; //Nothing to be	done

	//little endian - need to reverse bytes
	maxIndex = numBytes - 1;
	for(i = 0; i < (int)(numBytes/2); i++)
	{
		temp = bytes[i];
		bytes[i] = bytes[maxIndex-i];
		bytes[maxIndex-i] = temp;

	}
}

void uint16ToBytes(unsigned	short uint16Value, unsigned char *bytes)
{
	memcpy(bytes, &uint16Value, sizeof(unsigned short));
	correctEndian(bytes, 2);
}

void bytesToUint16(const unsigned char *bytes, unsigned short *uint16Value)
{
	memcpy(uint16Value, bytes, sizeof(unsigned short));
	correctEndian(uint16Value, sizeof(unsigned short));
}

void uint32ToBytes(unsigned int uint32Value, unsigned char *bytes)
{
	memcpy(bytes, &uint32Value, sizeof(unsigned int));
	correctEndian(bytes, sizeof(unsigned int));
}

void bytesToUint32(const unsigned char *bytes, unsigned int *uint32Value)
{
	memcpy(uint32Value, bytes, sizeof(unsigned int));
	correctEndian(uint32Value, sizeof(unsigned int));
}

void floatToBytes(float floatValue, unsigned char *bytes)
{
	memcpy(bytes, &floatValue, sizeof(float));
	correctEndian(bytes, sizeof(float));
}

void bytesToFloat(const unsigned char *bytes, float *floatValue)
{
	memcpy(floatValue, bytes, sizeof(float));
	correctEndian(floatValue, sizeof(float));
}

int	readTCP(EthernetClient *socket, unsigned char *packet,	int	size){
	// Serial.println("Recieved message: ");
	int i = 0;
	while(!socket->available()){
		delay(1);
		Serial.print("message delayed");
		if(i>5000){
			Serial.println("Connection timed out while waiting for response");
			break;
		}
		else
			i++;
	}
	i=0;
	while(socket->available()){
		packet[i]=socket->read();
		//Serial.println(packet[i]);
		i++;
	}
	if(i != size)
	{
		Serial.println("Unexpected read response size:");

		if(i >= 0)
			printPacket(packet, i);
	}
	return i;
}

int	readMultipleRegistersTCP(EthernetClient * socket, unsigned short address, unsigned char numRegisters, unsigned char *data)
{
	unsigned short transID = 0;
	unsigned char com[READ_MULT_REGS_COM_SIZE] = {0}; 
	int resSize	= 0;
	unsigned char res[READ_MULT_REGS_RESP_DATA_INDEX + 255] = {0}; //max size
	int size = 0;

	transID = getNextTransactionID();

	if(setupReadMultRegsCom(transID, 0, address, numRegisters, com, &resSize) != 0)
		return -1;

	if(socket->write(com, READ_MULT_REGS_COM_SIZE) != READ_MULT_REGS_COM_SIZE){
		return -2;
	}

	// Serial.println("Sent message: ");
	// for(int i=0; i<READ_MULT_REGS_COM_SIZE;i++){
	// 	Serial.println(com[i]);
	// }
	//delay(100);
	if((size=readTCP(socket, res, resSize)) <= 0){
		return -3;
	}

	if(checkReadMultRegsRes(res, size, transID) != 0)
		return -4;

	memcpy(data, &res[READ_MULT_REGS_RESP_DATA_INDEX], numRegisters*BYTES_PER_REGISTER);
	return 0;
	
}

int writeMultipleRegistersTCP(EthernetClient * socket, unsigned short address, unsigned char numRegisters, const unsigned char *data)
{
	unsigned short transID = 0;
	unsigned char com[WRITE_MULT_REGS_COM_DATA_INDEX + 255] = {0}; //max size
	int comSize = 0;
	unsigned char res[WRITE_MULT_REGS_RESP_SIZE] = {0};
	int size = 0;

	transID	= getNextTransactionID();

	if(setupWriteMultRegsCom(transID, 0, address, numRegisters, data, com, &comSize) != 0)
		return -1;

	if(socket->write(com, comSize) != comSize)
		return -1;

	if((size = readTCP(socket, res, WRITE_MULT_REGS_RESP_SIZE)) <= 0)
		return -1;

	if(checkModbusResponse(res, size, transID, WRITE_MULT_REGS_COM_FUNCTION_CODE) != 0)
		return -2;

	return 0;
}

int readLabJackError(EthernetClient * socket, unsigned short *errorCode)
{
	unsigned char data[2];
	if(readMultipleRegistersTCP(socket,	55000, 1, data) < 0)
		return -1;
	bytesToUint16(data, errorCode);
	return 0;
}

unsigned short getNextTransactionID()
{
	static unsigned short transactionID = 0;
	return transactionID++;
}

void setModbusPacketHeader(unsigned char *packet, unsigned short transID, unsigned char length, unsigned char unitID)
{
	uint16ToBytes(transID, packet);
	packet[2] = 0; //Protcol ID (MSB)
	packet[3] = 0; //Protcol ID (LSB)
	uint16ToBytes(length, &packet[4]);
	packet[6] = unitID; //Unit ID;
}

int checkModbusResponse(const unsigned char *packet, int packetSize, unsigned short transID, unsigned char functionCode)
{
	unsigned short pTransID = 0;
	int ret = 0;
	ret = checkModbusResponseNoID(packet, packetSize, functionCode);
	if(ret != 0)
		return ret;
	bytesToUint16(packet, &pTransID);
	if(pTransID != transID)
	{
		Serial.println("checkModbusResponse error: Unexpected Modbus response transaction ID.");
		printPacket(packet, packetSize);
		return -1;
	}
	return 0;
}


int checkModbusResponseNoID(const unsigned char *packet, int packetSize, unsigned char functionCode)
{
	unsigned short hLength = 0;
	if(packetSize < 7)
	{
		Serial.println("checkModbusResponse error: Packet contains incomplete packet header.");
		printPacket(packet,	packetSize);
		return -1;
	}

	if(packetSize < 9)
	{
		Serial.println("checkModbusResponse error: Incomplete packet response.");
		printPacket(packet, packetSize);
		return -1;
	}

	if(functionCode != packet[7])
	{
		if( (functionCode | 0x80) == packet[7] )
		{
			Serial.println("checkModbusResponse error: Received Modbus exception.");
			printPacket(packet, packetSize);
			return packet[8];
		}
		else
		{
			Serial.println("checkModbusResponse error: Unexpected Modbus response function code.");
			printPacket(packet, packetSize);
			return -1;
		}
	}
	
	bytesToUint16(&packet[4], &hLength);
	if(packetSize < hLength + 6)
	{
		Serial.println("parseReadMultRegsRes error: Packet size is	not	too	small Modbus length.");
		printPacket(packet, packetSize);
		return -1;
	}

	return 0;
}

int	setupWriteMultRegsCom(unsigned short transID, unsigned char	unitID,	unsigned short address,	unsigned char numRegisters,	const unsigned char	*data, unsigned	char *comPacket, int *comPacketSize)
{
	int i = 0;
	if(numRegisters > 127)
	{
		//BYTES_PER_REGISTER*numRegisters needs to be under 255
		printf("setupWriteMultRegCom error: %d*numRegisters needs to be a value under 255.", BYTES_PER_REGISTER);
		return -1;
	}
	comPacket[7] = 16;
	uint16ToBytes(address, &comPacket[8]);
	uint16ToBytes((unsigned	char)numRegisters, &comPacket[10]);
	comPacket[12] = (unsigned char)((BYTES_PER_REGISTER*numRegisters)&0xFF);
	for(i = 0; i < ((BYTES_PER_REGISTER*numRegisters)&0xFF); i++)
		comPacket[WRITE_MULT_REGS_COM_DATA_INDEX + i] = data[i];
	setModbusPacketHeader(comPacket, transID, 7 + BYTES_PER_REGISTER*numRegisters, unitID);
	*comPacketSize = WRITE_MULT_REGS_COM_DATA_INDEX + BYTES_PER_REGISTER*numRegisters;
	return 0;
}

int	setupReadMultRegsCom(unsigned short	transID, unsigned char unitID, unsigned	short address, unsigned	char numRegisters, unsigned	char *comPacket, int *resSize)
{
	if(numRegisters > 127)
	{
		//BYTES_PER_REGISTER*numRegisters needs to be under 255
		Serial.println("setupReadMultRegsCom error: BYTES_PER_REGISTER*numRegisters needs to be a value under 255.");
		return -1;
	}
	comPacket[7] = 3;
	uint16ToBytes(address, &comPacket[8]);
	uint16ToBytes((unsigned char)numRegisters, &comPacket[10]);
	setModbusPacketHeader(comPacket, transID, 6, unitID);

	*resSize = READ_MULT_REGS_RESP_DATA_INDEX + BYTES_PER_REGISTER*numRegisters;
	return 0;
}

//Checks if	the	packet size	and	Modbus header length are the same. Used	by
//checkReadMultRegsRes and checkReadMultRegsResNoID.
int checkSizeReadMultRegsRes(const unsigned char *packet, int packetSize)
{
	if(packetSize != 9 + (unsigned char)packet[8])
	{
		Serial.println("checkReadMultRegsRes error: Packet size is too big/small for data amount.");
		return -1;
	}
	return 0;
}

int	checkReadMultRegsRes(const unsigned	char *packet, int packetSize, unsigned short transID)
{
	int ret = 0;
	ret = checkModbusResponse(packet, packetSize, transID, 3);
	if(ret != 0)
		return ret;
	return checkSizeReadMultRegsRes(packet, packetSize);
}

int	checkReadMultRegsResNoID(const unsigned	char *packet, int packetSize)
{
	int ret = 0;
	ret = checkModbusResponseNoID(packet, packetSize, 3);
	if(ret != 0)
		return ret;
	return checkSizeReadMultRegsRes(packet, packetSize);
}

void printPacket(const unsigned	char *packet, int size)
{
	int i = 0;
	Serial.println("PACKET: ");
	if(size > 0)
	{
		for(i = 0; i < size; i++)
			 Serial.println(packet[i]);
	}
}
