#include "includes\teensy_com.h"
#include "includes\options.h"
#include "includes\statemachine.h"

#include <Arduino.h>
#include <HardwareSerial.h>


//Setting up on can bus 1, rx = 23, tx = 22

#define Com_Baud 9600

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
void readMessage(unsigned char * message_1, unsigned char * message_2){
    if(Com_1.available())
        *message_1 = Com_1.read();
    if(Com_2.available())
        *message_2 = Com_2.read();
}