#ifndef RADIO_H
#define RADIO_H

#include "..\XBee.h"

#define GND_STN_ADDR_MSB 0x0013A200 //Ground Station - Body
#define GND_STN_ADDR_LSB 0x41678FC0

void sendRadioData(XBee &radio, ZBTxRequest* txPacket, unsigned char byte_data[], int length);

void checkRadioRx(XBee &radio);

void doCommand(char command);


#endif