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

float voltsToTempK(float volts, float temp){
    
    float vHot = volts*1000;
    float tHot = 0;

    Serial.println(vHot, 7);

    float tCoefficients[11];
    if(temp >= 0){
        tCoefficients[0] = -0.176004136860E-01;
        tCoefficients[1] = 0.389212049750E-01;
        tCoefficients[2] = 0.185587700320E-04;
        tCoefficients[3] = -0.994575928740E-07;
        tCoefficients[4] = 0.318409457190E-09;
        tCoefficients[5] = -0.560728448890E-12;
        tCoefficients[6] = 0.560750590590E-15;
        tCoefficients[7] = -0.320207200030E-18;
        tCoefficients[8] = 0.971511471520E-22;
        tCoefficients[9] = -0.121047212750E-25;
        tCoefficients[10] = 0;

        float a0 = 0.118597600000E+00;
        float a1 = -0.118343200000E-03;
        float a2 = 0.126968600000E+03;

        vHot += a0 * exp(a1 * pow((temp - a2), 2));    

    }else{
        tCoefficients[0] = 0;
        tCoefficients[1] = 0.394501280250E-01;
        tCoefficients[2] = 0.236223735980E-04;
        tCoefficients[3] = -0.328589067840E-06;
        tCoefficients[4] = -0.499048287770E-08;
        tCoefficients[5] = -0.675090591730E-10;
        tCoefficients[6] = -0.574103274280E-12;
        tCoefficients[7] = -0.310888728940E-14;
        tCoefficients[8] = -0.104516093650E-16;
        tCoefficients[9] = -0.198892668780E-19;
        tCoefficients[10] = -0.163226974860E-22;

    }
    for(int i = 0; i < 11; i++){
        vHot+=tCoefficients[i]*pow(temp, i);
    }

    // Serial.println(vHot, 7);

    float vCoefficients[10];

    if(temp >= 0 && temp <= 500){
        vCoefficients[0] = 0;
        vCoefficients[1] = 2.508355E+01;
        vCoefficients[2] = 7.860106E-02;
        vCoefficients[3] = -2.503131E-01;
        vCoefficients[4] = 8.315270E-02;
        vCoefficients[5] = -1.228034E-02;
        vCoefficients[6] = 9.804036E-04;
        vCoefficients[7] = -4.413030E-05;
        vCoefficients[8] = 1.057734E-06;
        vCoefficients[9] = -1.052755E-08;
    }else if(temp<0){
        vCoefficients[0] = 0;
        vCoefficients[1] = 2.5173462E+01;
        vCoefficients[2] = -1.1662878E+00;
        vCoefficients[3] = -1.0833638E+00;
        vCoefficients[4] = -8.9773540E-01;
        vCoefficients[5] = -3.7342377E-01;
        vCoefficients[6] = -8.6632643E-02;
        vCoefficients[7] = -1.0450598E-02;
        vCoefficients[8] = -5.1920577E-04;
        vCoefficients[9] = 0;
    }

    
    for(int i = 0; i < 10; i++){
        tHot+=vCoefficients[i]*pow(vHot, i);
    }

    return tHot;

}