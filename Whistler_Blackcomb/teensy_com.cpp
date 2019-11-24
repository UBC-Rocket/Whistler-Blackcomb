#include "includes\teensy_com.h"
#include "includes\options.h"

#include <Arduino.h>
#include <HardwareSerial.h>


//Setting up on can bus 1, rx = 23, tx = 22

#define Com_Baud 9600
#ifdef SYSTEM_1
    #define Unit_2 Serial1
    #define Unit_3 Serial4
#endif
#ifdef SYSTEM_2
    #define Unit_1 Serial1
    #define Unit_3 Serial2
#endif
#ifdef SYSTEM_3
    #define Unit_1 Serial4
    #define Unit_2 Serial2
#endif

static int message_id;

void setupSerialCom(){
    #ifdef SYSTEM_1
        Unit_2.begin(Com_Baud);
        Unit_3.begin(Com_Baud);
    #endif
    #ifdef SYSTEM_2
        Unit_1.begin(Com_Baud);
        Unit_3.begin(com_Baud);
    #endif
    #ifdef SYSTEM_3
        Unit_1.begin(Com_Baud);
        Unit_2.begin(Com_Baud);
    #endif
}

void sendMessage(unsigned char message, int system_address){
    #ifdef SYSTEM_1
        if(system_address == 2)
            Unit_2.write(message);
        else if(system_address == 3)
            Unit_3.write(message);
    #endif
    #ifdef SYSTEM_2
        if(system_address == 1)
            Unit_1.write(message);
        else if(system_address == 3)
            Unit_3.write(message);
    #endif
    #ifdef SYSTEM_3
        if(system_address == 2)
            Unit_2.write(message);
        else if(system_address == 1)
            Unit_1.write(message);
    #endif


}
//message 1 is lowest system teensy id that it is communicating with
void readMessage(char * message_1, char * message_2){
    #ifdef SYSTEM_1
        if()


}