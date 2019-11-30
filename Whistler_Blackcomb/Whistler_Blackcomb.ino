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
  static bool on = true;
  static int old_time = millis();
  static unsigned char message_1 = 0;
  static unsigned char message_2 = 0;

  // if(Serial1.available())
  //   SerialUSB.println(Serial1.read());

  // delay(1000);

  // SerialUSB.println("one loop");

  readMessage(&message_1, &message_2);


  int new_time = millis();
  if(new_time - old_time > time){
    SerialUSB.println("Message 1:");
    SerialUSB.println(message_1);
    SerialUSB.println("Message 2:");
    SerialUSB.println(message_2);
    sendMessage(1, 1);
    sendMessage(1, 2);
    old_time = new_time;
  }





}
