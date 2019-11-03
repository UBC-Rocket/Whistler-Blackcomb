

#ifndef	ARDUINO_LABJACK_H
#define ARDUINO_LABJACK_H

#include <Arduino.h>
#include "ArduinoLabjack.h"
#include <Ethernet.h>
#include <SPI.h>
#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

#ifdef __cplusplus
extern "C" {
#endif

int LJM_Open(EthernetClient *client, ModbusTCPClient *modbusTCPClient, IPAddress server);

void LJM_eReadAddress(ModbusTCPClient *modbusTCPClient, int Address, int Type, float * Value);

void LJM_eWriteAddress(ModbusTCPClient *modbusTCPClient, int Address, int Type, float Value); 

void float2Bytes(float val,uint32_t* uint);

#ifdef  __cplusplus
}
#endif

#endif
