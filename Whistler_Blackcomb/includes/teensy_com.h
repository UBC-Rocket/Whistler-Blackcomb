#ifndef TEENSY_COM_H
#define TEENSY_COM_H

#include "statemachine.h"
#include "options.h"

//Integrate the state machine into a full voter class that handles communication and state transitions
class Voter : public FlightState{
    public:
        Voter(unsigned char, bool);
        bool status;
        //status according to received messages and self reporting of the unit
        bool com1Status;
        bool com2Status;
        //Status according to other unit
        bool com1StatusAux;
        bool com2StatusAux;

        unsigned char kingID;
        //king according to other two units
        unsigned char kingID1;
        unsigned char kingID2;

        //if king act on status change
        bool king;

        //iterations since message recieved from units
        unsigned char noComCount1;
        unsigned char noComCount2;

        //desired states of unit messages
        FlightStates unitState1;
        FlightStates unitState2;

        //unit ID, 1 2 or 3
        unsigned char unitID;

        void systemUpdate();
        #ifndef SOFTWARE_TESTING
        void readMessages();
        #endif
        void parseMessage(unsigned char, unsigned char);
        void assessStates();
        void assessKings();
        void generateMessages(unsigned char*, unsigned char*);
};

#ifndef SOFTWARE_TESTING
void setupSerialCom();
void sendMessage(unsigned char message, unsigned char system_address);
#endif



#endif