#include "..\includes\teensy_com.h"
#include "..\includes\options.h"
#include "..\includes\statemachine.h"

#ifndef SOFTWARE_TESTING
#include <Arduino.h>
#include <HardwareSerial.h>
#endif


//Setting up on can bus 1, rx = 23, tx = 22

#define Com_Baud 9600
#define Count_Out 10

//For Each system Com 1 is the highest address other than itself

#ifdef SYSTEM_1
    #define Com_1 Serial1
    #define Com_2 Serial4
#endif
#ifdef SYSTEM_2
    #define Com_1 Serial1
    #define Com_2 Serial2
#endif
#ifdef SYSTEM_3
    #define Com_1 Serial1
    #define Com_2 Serial4
#endif

#ifndef SOFTWARE_TESTING
void setupSerialCom(){
    Com_1.begin(Com_Baud);
    Com_2.begin(Com_Baud);
}
//address 1 or 2 for the higher or lower system
void sendMessage(unsigned char message, unsigned char system_address){
    if(system_address == 1)
        Com_1.write(message);
    if(system_address == 2)
        Com_2.write(message);
}
#endif

Voter::Voter(unsigned char id, bool stat){
    status = stat;
    kingID = 1;
    kingID1 = 1;
    kingID2 = 1;
    com1Status = true;
    com2Status = true;
    unitID = id;
    if(unitID == 1)
        king = true;
    noComCount1 = 0;
    noComCount2 = 0;
    unitState1 = (FlightStates)0;
    unitState2 = (FlightStates)0;
}

#ifndef SOFTWARE_TESTING
void Voter::readMessages(){
    if(Com_1.available()){
        unsigned char message1 = Com_1.read();
        noComCount1 = 0;
        parseMessage(1, message1);
    }
    else{
        noComCount1 = noComCount1 + 1;
        if(noComCount1 > Count_Out){
            com1Status = false;
        }
    }

    if(Com_2.available()){
        unsigned char message2 = Com_2.read();
        noComCount2 = 0;
        parseMessage(2, message2);
    }
    else{
        noComCount2 = noComCount2 + 1;
        if(noComCount2 > Count_Out){
            com2Status = false;
        }
    }

}
#endif

void Voter::parseMessage(unsigned char source, unsigned char message){
    unsigned char state = message & 0x0F;
    bool stat1 = (message | 0x7F) == 0xFF;
    bool stat2 = (message | 0xBF) == 0xFF;
    unsigned char king = message & 0x30;
    king = king >> 4;

    if(source == 1){
        unitState1 = (FlightStates)state;
        com1Status = stat1;
        com2StatusAux = stat2;
        kingID1 = king;
    }
    else if(source == 2){
        unitState2 = (FlightStates)state;
        com2Status = stat1;
        com1StatusAux = stat2;
        kingID2 = king;
    }
}

/*for bits ABCD EFGH
A is status of unit sending the message
B is status of the unit that is not being sent the message
CD is the King unit
EFGH is the state that the unit desires to be in
*/
void Voter::generateMessages(unsigned char * message1, unsigned char * message2){
    *message1 = 0x0;
    *message2 = 0x0;
    if(status){
        *message1 = *message1 | 0x80;
        *message2 = *message2 | 0x80;
    }
    if(com1Status)
        *message2 = *message2 | 0x40;
    if(com2Status)
        *message1 = *message1 | 0x40;

    //make sure that the king is an acceptable value
    if(kingID <=3){
        unsigned char temp = kingID;
        temp = temp << 4;
        *message1 = *message1 | temp;
        *message2 = *message2 | temp;
    }

    *message1 = *message1 | (unsigned char)desiredState;
    *message2 = *message2 | (unsigned char)desiredState;
}


//for status the first number is the unit the second number is the unit the message came from
void Voter::assessKings(){
    if(unitID == 1){
        if((kingID1 == 1 && com1Status) | (kingID2 == 1 && com2Status)){
            king = true;
        }
        else{
            king = false;
        }
    }
    else if(unitID == 2){
        if(com1Status){
            kingID = 1;
            king = false;
        }
        else if(kingID2 == 2 && com2Status){
            kingID = 2;
            king = true;
        }
        else{
            king = false;
            kingID = 2;
        }
    }
    else if(unitID == 3){
        if(com1Status)
            kingID = 1;
        else
            kingID = 2;
    }

}

//updates the system desired and current states
void Voter::assessStates(){
    //use noComCount to make sure that a message was received this iteration
    if(unitState1 == unitState2 && unitState2 != getState() && com2Status && com1Status){
        desiredState = unitState2;
        setState(desiredState);
    }
    else if(desiredState == unitState1 && desiredState != getState() && com1Status){
        setState(desiredState);
    }
    else if(desiredState == unitState2 && desiredState != getState() && com2Status){
        setState(desiredState);
    }
}


void Voter::systemUpdate(){
    #ifndef SOFTWARE_TESTING
        readMessages();
    #endif
    assessKings();
    assessStates();

    unsigned char send1 = 0x0;
    unsigned char send2 = 0x0;

    generateMessages(&send1, &send2);

    #ifndef SOFTWARE_TESTING
    sendMessage(send1, 1);
    sendMessage(send2, 2);
    #endif
}

