//make sure to select teensy 4.0 for boards
/*
Ino file for the Whistler Blackcomb rocket
*/


/*Includes------------------------------------------------*/
//#include "includes\statemachine.h"
#include "includes\ArduinoLabjack.h"
#include "includes\modbus.h"
#include "includes\stream.h"

#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

/*Constants-----------------------------------------------*/
float labjackData [STREAM_MAX_SAMPLES_PER_PACKET_TCP] = {0};


void setup() {
  labjackSetup();
}

void loop() {
  labjackRead(labjackData); 
}
