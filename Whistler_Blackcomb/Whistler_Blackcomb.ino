//make sure to select teensy 4.0 for boards
#include "includes\teensy_com.h"
#include "includes\options.h"

#include <Arduino.h>
#include <HardwareSerial.h>


void setup() {

  SerialUSB.begin(9600);
  setupSerialCom();

}

void loop() {
  // put your main code here, to run repeatedly:

}
