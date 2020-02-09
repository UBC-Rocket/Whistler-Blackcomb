#include "includes/radio.h"


#include <Arduino.h>
#include <HardwareSerial.h>

#define SerialRadio             Serial2

XBee radio = XBee();
static ZBTxRequest txPacket = ZBTxRequest();
static XBeeAddress64 gndAddr = XBeeAddress64(GND_STN_ADDR_MSB, GND_STN_ADDR_LSB);

void setup(){
    SerialRadio.begin(921600);
    Serial.begin(9600);
    while(!SerialRadio){}
    radio.setSerial(SerialRadio);
    txPacket.setAddress64(gndAddr);

    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
    Serial.println("Ititialization done");
}

void loop(){
    checkRadioRx(radio);
    if(Serial.available() > 0){
       unsigned char data[50];
        int pos = 0;
        while(Serial.available()>0){
            data[pos] = Serial.read();
            pos++;
        }
        sendRadioData(radio, &txPacket, data, pos);
    }

}