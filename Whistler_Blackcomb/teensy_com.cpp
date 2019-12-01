#include "includes\teensy_com.h"
#include "includes\options.h"
#include "includes\statemachine.h"

#include <Arduino.h>
#include <HardwareSerial.h>


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
//message 1 is lowest system teensy id that it is communicating with
void readMessage(unsigned char * message1, unsigned char * message2, bool * com1Status, bool * com2Status, unsigned char * noComCount1, unsigned char * noComCount2){
    if(Com_1.available()){
        *message1 = Com_1.read();
        *com1Status = true;
        *noComCount1 = 0;
    }
    else{
        *noComCount1 = *noComCount1 + 1;
        if(*noComCount1 > Count_Out){
            *com1Status = false;
        }
    }

    if(Com_2.available()){
        *message2 = Com_2.read();
        *com2Status = false;
        *noComCount2 = 0;
    }
    else{
        *noComCount2 = *noComCount2 + 1;
        //if the unit on com2 has not responded in a long enough time then set it to no longer effective
        if(*noComCount2 > Count_Out){
            *com2Status = false;
        }
    }
}

/*for bits ABCD EFGH
A is status of unit sending the message
B is status of the unit that is not being sent the message
CD is the King unit
EFGH is the state that the unit desires to be in
*/
unsigned char generateMessage(FlightState desiredState, bool unitStatus, bool nonComUnitStatus, unsigned char king){
    unsigned char message= 0x0;

    //Set Unit status bit to true
    if(unitStatus)
        message = message | 0xA0;
    //Set the unit status of the unit that is not being sent a message
    if(nonComUnitStatus)
        message = message | 0x40;

    //set the king bits of the message
    king = king << 4;
    message = message | king;

    //setting the state portion of the message
    unsigned char state = (unsigned char) desiredState;
    //making sure that it does not go over the first 4 bits and mess up king
    state = state & 0x0F;
    message = message | state;

    return message;
}

void systemsUpdate(FlightState * currentState, FlightState * desiredState, bool * unit1Status,
                    bool * unit2Status, bool * unit3Status, unsigned char * king, unsigned char * noComCount1, unsigned char * noComCount2){
    unsigned char receivedMessage1 = 0;
    unsigned char receivedMessage2 = 0;

    //Check for messages and if there is no message on line update iteration where there is no communication
    #ifdef SYSTEM_1
        readMessage(&receivedMessage1, &receivedMessage2, unit2Status, unit3Status, noComCount1, noComCount2);
    #endif
    #ifdef SYSTEM_2
        readMessage(&receivedMessage1, &receivedMessage2, unit1Status, unit3Status, noComCount1, noComCount2);
    #endif
    #ifdef SYSTEM_3
        readMessage(&receivedMessage1, &receivedMessage2, unit1Status, unit2Status, noComCount1, noComCount2);
    #endif

    unsigned char receivedState1 = receivedMessage1 & 0x0F;
    unsigned char recievedState2 = receivedMessage2 & 0x0F;

    unsigned char king1 = receivedMessage1 & 0x30;
    king1 = king1 >> 4;

    unsigned char king2 = receivedMessage2 & 0x30;
    king2 = king2 >> 4;

    bool msg1ReceivedUnit1Status = ((receivedMessage1 | 0x7F) == 0xFF);
    bool msg1ReceivedUnit2Status = ((receivedMessage1 | 0xBF) == 0xFF);

    bool msg2ReceivedUnit2Status = ((receivedMessage2 | 0x7F) == 0xFF);
    bool msg2ReceivedUnit1Status = ((receivedMessage2 | 0xBF) == 0xFF);

    if(*currentState != receivedState1){

    }
    else if(*currentState != recievedState2){

    }








}