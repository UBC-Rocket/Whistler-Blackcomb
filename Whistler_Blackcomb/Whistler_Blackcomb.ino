//make sure to select teensy 4.0 for boards
#include "includes\teensy_com.h"
#include "includes\options.h"

#include <Arduino.h>
#include <HardwareSerial.h>

int led = 13;
int time = 5000;



void setup() {

  SerialUSB.begin(9600);
  pinMode(13, OUTPUT);
  setupSerialCom();

}

void loop() {
  // put your main code here, to run repeatedly:






}
