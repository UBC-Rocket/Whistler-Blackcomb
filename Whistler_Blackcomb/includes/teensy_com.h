#ifndef TEENSY_COM_H
#define TEENSY_COM_H

#include "statemachine.h"




void setupSerialCom();

void systemsUpdate(FlightState * currentState, FlightState * desiredState, bool * unit1Status, bool * unit2Status, bool * unit3Status,  unsigned char * king, unsigned char * noComCount1, unsigned char * noComCount2);

void sendMessage(unsigned char message, unsigned char system_address);

void readMessage(unsigned char * message1, unsigned char * message2, bool * com1Status, bool * com2Status, unsigned char * noComCount1, unsigned char * noComCount2);

unsigned char generateMessage(FlightState desiredState, bool unitStatus, bool nonComUnitStatus, unsigned char king);


#endif