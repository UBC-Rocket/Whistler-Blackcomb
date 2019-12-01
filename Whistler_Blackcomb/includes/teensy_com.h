#ifndef TEENSY_COM_H
#define TEENSY_COM_H

#include "statemachine.h"

//Integrate the state machine into a full voter class that handles communication and state transitions
class Voter : public FlightState{
    public:
        Voter(id);
        bool status;
        bool com1Status;
        bool com2Status;
        unsigned char kingID;
        bool bishop;
        bool king;
        unsigned char noComCount1;
        unsigned char noComCount2;
        unsigned char unitID;
        void systemUpdate();
}


void setupSerialCom();

//void systemsUpdate(FlightState & flightState, bool * unit1Status, bool * unit2Status, bool * unit3Status,  unsigned char * king, unsigned char * noComCount1, unsigned char * noComCount2);

void sendMessage(unsigned char message, unsigned char system_address);

void readMessage(unsigned char * message1, unsigned char * message2, bool * com1Status, bool * com2Status, unsigned char * noComCount1, unsigned char * noComCount2);

unsigned char generateMessage(unsigned char , bool unitStatus, bool nonComUnitStatus, unsigned char king_id);


#endif