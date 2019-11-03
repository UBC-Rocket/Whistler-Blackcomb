
#include <Arduino.h>
#include "ArduinoLabjack.h"
#include <Ethernet.h>
#include <SPI.h>
#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

int LJM_Open(EthernetClient *client, ModbusTCPClient *modbusTCPClient, IPAddress server){
    if (!modbusTCPClient->begin(server, 502)) {
        Serial.println("Modbus TCP Client failed to connect!");
    } else {
        Serial.println("Modbus connected");
    } 
    Serial.println(client->connected());
};

void LJM_eReadAddress(ModbusTCPClient * modbusTCPClient, int Address, int Type, float * Value){
    if(!Type){
        // UINT16
        modbusTCPClient->requestFrom(HOLDING_REGISTERS, Address, 1); //error in docs, it says to use HOLD_REGISTER instead
    }else{
        // Other 32 bit numbers
        modbusTCPClient->requestFrom(HOLDING_REGISTERS, Address, 2); //error in docs, it says to use HOLD_REGISTER instead
    }

    long high;
    long low;
    int i=0;
    // Retrieve the data you requested from the Labjack
    while(modbusTCPClient->available()){
        if(!i){
            high=modbusTCPClient->read();
        }else{
            low=modbusTCPClient->read();
        }
        i++;
    }
    // Serial.println(high, HEX);
    // Serial.println(low, HEX);
    // Serial.println(high<<16, HEX);
    // Serial.println(low | (high<<16), HEX);

    // Sets value depending on what type was specified
    if(Type==0){
        // 16 bit unsigned int
        *Value = (uint16_t)high;
    }else if(Type==1){
        // 32 bit unsigned int  
        *Value = (uint32_t)(low | (high<<16));
    }else if(Type==2){
        // 32 bit signed int
        *Value = (int32_t)(low | (high<<16));
    }else if(Type==3){
        // 32 bit float
        long output = (low | (high<<16));
        memcpy(Value, &output, sizeof(float));
    }
};

void LJM_eWriteAddress(ModbusTCPClient *modbusTCPClient, int Address, int Type, float Value){
    if(!Type){
        // UINT16
        modbusTCPClient->beginTransmission(HOLDING_REGISTERS, Address, 1); //error in docs, it says to use HOLD_REGISTER instead
        modbusTCPClient->write(Value);
    }else{
        // Other 32 bit numbers
        modbusTCPClient->beginTransmission(HOLDING_REGISTERS, Address, 2); //error in docs, it says to use HOLD_REGISTER instead
    }
    if(Type==0){
        // 16 bit unsigned int
        modbusTCPClient->write((uint16_t)(Value));
    }else if(Type==1){
        // 32 bit unsigned int  
        modbusTCPClient->write((uint32_t)(Value)>>16);
        modbusTCPClient->write((uint32_t)(Value) & 0xFFFF);
    }else if(Type==2){
        // 32 bit signed int
        modbusTCPClient->write((int32_t)(Value)>>16);
        modbusTCPClient->write((int32_t)(Value) & 0xFFFF);
    }else if(Type==3){
        // 32 bit float
        uint32_t a;
        float2Bytes(Value, &a);
        modbusTCPClient->write(a>>16);
        modbusTCPClient->write(a & 0xFFFF);
    }

    modbusTCPClient->endTransmission();
};

void float2Bytes(float val,uint32_t* uint){
  // Create union of shared memory space
  union {
    float float_variable;
    uint32_t tempuint;
  } u;
  u.float_variable = val;
  // Assign bytes to input array
  memcpy(uint, &u.tempuint, 4);
}
