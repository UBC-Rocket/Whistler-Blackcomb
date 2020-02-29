#ifndef RADIO_H
#define RADIO_H

#include "..\XBee.h"

#define GND_STN_ADDR_MSB 0x0013A200 //Ground Station - Body
#define GND_STN_ADDR_LSB 0x41678FC0

#define DATASETSIZE 3
// const uint8_t DATASETSIZE 4;
// const uint8_t DATASETSIZE 4;

struct Dataset{
    float dataset1[DATASETSIZE];
};

void sendRadioData(XBee &radio, ZBTxRequest* txPacket, unsigned char byte_data[], int length);

void sendRadioDataset(XBee &radio, ZBTxRequest* txPacket, Dataset * data, uint8_t id);

void checkRadioRx(XBee &radio);

void doCommand(char command);


#endif