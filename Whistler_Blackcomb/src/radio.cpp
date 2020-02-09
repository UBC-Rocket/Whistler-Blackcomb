/*Includes---------------------------------------------------------*/
#include "../includes/radio.h"
#include <Arduino.h>


void sendRadioData(XBee &radio, ZBTxRequest * txPacket, unsigned char byte_data[], int length){
    txPacket->setPayloadLength(length);
    txPacket->setPayload(byte_data);
    radio.send(*txPacket);
}

void checkRadioRx(XBee &radio){
    static ZBRxResponse rx = ZBRxResponse();
    radio.readPacket();
    while(radio.getResponse().isAvailable() || radio.getResponse().isError()){
        if(radio.getResponse().isError()) { //will we use this?
            // #ifdef TESTING
            //     SerialUSB.println("Radio error");
            // #endif
            Serial.println("Error");
        }
        else if(radio.getResponse().getApiId() == ZB_RX_RESPONSE) {
            //received command from radio
            radio.getResponse().getZBRxResponse(rx);
            //test
            int length = rx.getDataLength();
            Serial.print("Radio data is: ");
            for(int i = 0; i < length; i++){
                Serial.print((char)rx.getData(i));
            }
            Serial.println();

        }
        else{
            Serial.println("Packet Delivered");
        }
        radio.readPacket();
    }
}

// void checkRadioRx(Xbee * radio, ZBTxRequest * txPacket, ){

// }





