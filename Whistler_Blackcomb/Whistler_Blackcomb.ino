    //make sure to select teensy 4.0 for boards

/*Includes---------------------------------------------------------*/
#include "includes\sensors.h"
#include "includes\calculations.h"
#include <SoftwareSerial.h>
#include "includes\modbus.h"
#include "includes\stream.h"

#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
//#include "includes\SparkFun_LIS331.h"

//int LIS331DataLength = 15;
//int BNO080DataLength = 15;
//int MCP9808DataLength = 15;
//int TinyGPSPlusDataLength = 15;

float labjackData [STREAM_MAX_SAMPLES_PER_PACKET_TCP] = {0};

LIS331 accel1;
BNO080 accel2;
Adafruit_MCP9808 temp =Adafruit_MCP9808();

// GPS 
TinyGPSPlus gps;
#define ss Serial1
static const uint32_t GPSBaud = 4800;
double lat;
double lon;

//SoftwareSerial ss(RXPin, TXPin); // The serial connection to the GPS device

int16_t accel1Data[3];
float accel2Data[3];

int16_t accel1DataOld[3][LIS331DataLength];
float accel2DataOld[3][15];
float tempDataOld[15];

float labjackTemp;

void setup() { 
  Serial.begin(9600);
  Wire.begin();
  delay(1000);
//  initLIS331(&accel1, 0x19);
//  initBNO080(&accel2, 0x4B, 50);
//  initMCP9808(&temp, 0x18, 3);
//  ss.begin(GPSBaud); 
  labjackSetup();  
}

void loop() {
  labjackRead(labjackData); 
  Serial.println(voltsToTempK(labjackData[1], labjackData[0] * (-92.6) + 194.45), 7);
//  Serial.println(voltsToTempK(4.409, 24));
//  pollSensors(&accel1, &accel2, &temp, &gps, accel1Data, accel2Data, tempData, lat, lon);
//  Serial.println(tempData);
//  Serial.println(accel1Data[0]);
//  Serial.println(accel2Data[0]);
//  Serial.println(lat);
//  Serial.println(lon);
//  updateAverageLIS331(accel1DataOld, accel1Data);
//  updateAverageBNO080(accel2DataOld, accel2Data);
//  updateAverageMCP9808(tempDataOld, &tempData);
  //Serial.println(tempData);
  
//  smartDelay(1000);
//
//  if (millis() > 5000 && gps.charsProcessed() < 10)
//    Serial.println(F("No GPS data received: check wiring"));
  //delay(1000);
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
