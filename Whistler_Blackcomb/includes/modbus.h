#ifndef MODBUS_H_
#define MODBUS_H_

#define BYTES_PER_REGISTER 2

#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>

#ifdef __cplusplus
extern "C" {
#endif


//Converts an unsigned short value to a 2 byte, big endian byte array.
void uint16ToBytes(unsigned short uint16Value, unsigned char *bytes);

//Converts 2 byte, big endian byte array to an unsigned short value.
void bytesToUint16(const unsigned char *bytes, unsigned short *uint16Value);

//Converts an unsigned integer value to a 4 byte, big endian byte array.
void uint32ToBytes(unsigned int uint32Value, unsigned char *bytes);

//Converts 4 byte, big endian byte array to an unsigned integer value.
void bytesToUint32(const unsigned char *bytes, unsigned int *uint32Value);

//Converts a float value to a 4 byte, big endian byte array.
void floatToBytes(float floatValue, unsigned char *bytes);

//Converts 4 byte, big endian byte array to a float value.
void bytesToFloat(const unsigned char *bytes, float *floatValue);

int	readTCP(EthernetClient * socket, unsigned char *packet,	int	size);

//Reads multiple registers over TCP using Modbus function 3 (Read Multiple
//Registers). Returns -1 on general error, -2 on Modbus response error, and 0
//on success. On -2, you can use readLabJackError to get the LabJack error
//from the device.
//socket: The T7's socket. The socket needs to be on port 502.
//address: The starting register address.
//numRegisters: The number of registers to read.
//data: Byte array of read data. Data is big endian, and needs to be
//      BYTES_PER_REGISTER*numRegisters in size.
int readMultipleRegistersTCP(EthernetClient * socket, unsigned short address,
                             unsigned char numRegisters, unsigned char *data);

//Writes multiple registers over TCP using Modbus function 16 (Write Multiple
//Registers). Returns -1 on general error, -2 on Modbus response error, and 0
//on success. On -2, you can use the readLabJackError function to get the
//LabJack error from the device.
//socket: The T7's socket. The socket needs to be on port 502.
//address: The starting register address.
//numRegisters: The number of registers to send.
//data: Byte array of data to send. Data is big endian, and needs to be
//      BYTES_PER_REGISTER*numRegisters in size.
// int writeMultipleRegistersTCP(TCP_SOCKET socket, unsigned short address,
//                               unsigned char numRegisters, const unsigned char *data);
int writeMultipleRegistersTCP(EthernetClient * socket, unsigned short address,
                              unsigned char numRegisters, const unsigned char *data);

//Reads the error code (Address 55000) from the LabJack device.
//Returns -1 on error and 0 on success.
//socket: The T7's socket. The socket needs to be on port 502.
//errorCode: The read LabJack error code from the device.
int readLabJackError(EthernetClient * socket, unsigned short *errorCode);

#define WRITE_MULT_REGS_COM_FUNCTION_CODE 16
#define WRITE_MULT_REGS_COM_DATA_INDEX 13
#define WRITE_MULT_REGS_RESP_SIZE 12
#define READ_MULT_REGS_COM_SIZE 12
#define READ_MULT_REGS_RESP_BYTES_INDEX 8
#define READ_MULT_REGS_RESP_DATA_INDEX 9

//Returns the next incremental transactions ID. Values are 0 to 65535.
unsigned short getNextTransactionID();

//Sets the header portion (bytes 0 - 6) of the Modbus packet.
//packet: The Modbus packet. The array needs to have at least 7 elements.
//transID: The transaction ID. This will be echoed in the response.
//length: The length of the Modbus command
//unitID: The unit ID of the Modbus command.
void setModbusPacketHeader(unsigned char *packet, unsigned short transID,
                           unsigned char length, unsigned char unitID);

//Checks a Modbus response for errors. Returns -1 if an error is detected, 0 if
//the response seems valid.
//packet: The array packet containing the Modbus response (including the
//        header).
//packetSize: The size of the packet array.
//transID: The expected transaction ID in the response. This should be the
//         Modbus command's transaction ID.
//functionCode: The expected Modbus function code. This should be the Modbus
//              commands's function code.
int checkModbusResponse(const unsigned char *packet, int packetSize,
                        unsigned short transID, unsigned char functionCode);

//Checks a Modbus response for errors. Returns -1 if an error is detected, 0 if
//the response seems valid. No transaction ID check is performed, otherwise
//this performs the same checks as the checkModbusResponse function.
int checkModbusResponseNoID(const unsigned char *packet, int packetSize,
                            unsigned char functionCode);

//Creates a Write Multiple Registers (write data) Modbus packet including its
//header. Returns -1 on error, and 0 on success.
//transID: The transaction ID. This will be echoed in the response.
//unitID: The unit ID.
//address: The starting register address.
//numRegisters: The number of registers to send.
//data: The byte array data to send. Data is big endian, and needs to be
//      BYTES_PER_REGISTER*numRegisters in size.
//comPacket: The created Modbus command packet array. This needs to have at
//           least 13 + BYTES_PER_REGISTER*numRegisters elements.
//comPacketSize: The size of the command array. 
// int setupWriteMultRegsCom(unsigned short transID, unsigned char unitID,
//                           unsigned short address, unsigned char numRegisters,
//                           const unsigned char *data, unsigned char *comPacket,
//                           int *comPacketSize);
int setupWriteMultRegsCom(unsigned short transID, unsigned char unitID,
                          unsigned short address, unsigned char numRegisters,
                          const unsigned char *data, unsigned char *comPacket,
                          int *comPacketSize);

//Creates a Read Multiple Registers (read data) Modbus packet including its
//header. Returns -1 on error, and 0 on success.
//transID: The transaction ID. This will be echoed in the response.
//unitID: The unit ID. Set to 1 for stream packets, otherwise 0.
//address: The starting register address.
//numRegisters: The number of registers to read.
//comPacket: The created Modbus command packet array. This needs to have at
//           least 12 elements.
//comPacketSize: The size of the command array. 
int setupReadMultRegsCom(unsigned short transID, unsigned char unitID,
                         unsigned short address, unsigned char numRegisters,
                         unsigned char *comPacket, int *resSize);

//Checks a Read Multiple Registers response for errors including the header.
//Returns -1 if an error is detected, 0 if the response seems valid.
//packet: The packet array to check. Valid packets should have
//        9 + BYTES_PER_REGISTER*numRegisters elements with data starting at
//        byte 9.
//packetSize: The packet array's size.
//transID: The expected transaction ID in the response. This should be the
//         command's transaction ID.
int checkReadMultRegsRes(const unsigned char *packet, int packetSize,
                         unsigned short transID);

//Checks a Read Multiple Registers response for errors including the header.
//No transaction ID check is performed, otherwise this performs the same checks
//as the checkReadMultRegsRes function. Returns -1 if an error is detected, 0
//if the response seems valid.
int checkReadMultRegsResNoID(const unsigned char *packet, int packetSize);

//For debugging purposes. Prints a packet/array to the terminal.
void printPacket(const unsigned char *packet, int size);

#ifdef  __cplusplus
}
#endif

#endif
