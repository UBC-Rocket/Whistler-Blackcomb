/*
Sensors Source

Conatiains implementation of sensor initializations and data pulling
*/

/*Includes-----------------------------------------------------------------*/
#include "../includes/sensors.h"
#include "../includes/Adafruit_MCP9808.h"
#include "../includes/SparkFun_BNO080_Arduino_Library.h"
#include "../includes/SparkFun_LIS331.h"
#include "../includes/TinyGPS++.h"

/*Constants----------------------------------------------------------------*/

/*Functions----------------------------------------------------------------*/

// LIS331 acceleromter: https://github.com/sparkfun/SparkFun_LIS331_Arduino_Library
// address: I2C address to use, either 0x19 (default) or 0x18 (if SA0 is soldered closed)
// To read accelerometer, use LIS331::readAxes
void initLIS331(LIS331 *sensor, uint8_t address){
    sensor->setI2CAddr(address);
    sensor->begin(LIS331::USE_I2C);
}

// BNO080 accelerometer: https://github.com/sparkfun/SparkFun_BNO080_Arduino_Library
// address: I2C address to use, either 0x4B (default) or 0x4A (if ADR is grounded)
// period: time between every data update
void initBNO080(BNO080 *sensor, uint8_t address, uint8_t period){
    if(!sensor->begin(address)){
        Serial.println("BNO080 Connection Failed");
    }
    sensor->enableAccelerometer(period);
}

// x, y, z: variables to hold the data returned
void readAxesBNO080(BNO080 *sensor, float &x, float &y, float &z){
    sensor->dataAvailable();
    x = sensor->getAccelX();
    y = sensor->getAccelY();
    z = sensor->getAccelZ();
}

// MCP9808 temperature sensor: https://github.com/adafruit/Adafruit_MCP9808_Library
// address: I2C address to use, default is 0x18 and can be changed by adjusting A0, A1 or A2 (8 possibilities)
// res: Resolution index to use: 
//      Mode Resolution  SampleTime
//      0    0.5°C       30 ms
//      1    0.25°C      65 ms
//      2    0.125°C     130 ms
//      3    0.0625°C    250 ms
// Use Adafruit_MCP9808::readTempC() or Adafruit_MCP9808::readTempF() to read
void initMCP9808(Adafruit_MCP9808 *sensor, uint8_t address, uint8_t res){
    if(!sensor->begin(address)){
        Serial.println("MCP9808 Connection Failed");
    }
    sensor->setResolution(res);
}

void pollSensors(LIS331 *accel1, BNO080 *accel2, Adafruit_MCP9808 *temp, TinyGPSPlus *gps, int16_t accel1Data[], float accel2Data[], float &tempData, double &lat, double &lon){
    accel1->readAxes(accel1Data[0], accel1Data[1], accel1Data[2]);
    readAxesBNO080(accel2, accel2Data[0], accel2Data[1], accel2Data[2]);
    tempData = temp->readTempC();

    lat = gps->location.lat();
    lon = gps->location.lng();
}