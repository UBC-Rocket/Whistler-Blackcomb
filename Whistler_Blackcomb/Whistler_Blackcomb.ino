//make sure to select teensy 4.0 for boards

/*Includes---------------------------------------------------------*/
#include "includes\sensors.h"
#include "includes\calculations.h"
#include <SoftwareSerial.h>
//#include "includes\SparkFun_LIS331.h"

/*int LIS331DataLength = 15;
int BNO080DataLength = 15;
int MCP9808DataLength = 15;
int TinyGPSPlusDataLength = 15;*/

LIS331 accel1;
BNO080 accel2;
Adafruit_MCP9808 temp =Adafruit_MCP9808();

// GPS 
TinyGPSPlus gps;
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 4800;
double lat;
double lon;

SoftwareSerial ss(RXPin, TXPin); // The serial connection to the GPS device

int16_t accel1Data[3];
float accel2Data[3];
float tempData;

int16_t accel1DataOld[3][LIS331DataLength];
float accel2DataOld[3][15];
float tempDataOld[15];

void setup() { 
  Serial.begin(9600);
  Wire.begin();
  delay(500);
  initLIS331(&accel1, 0x19);
  initBNO080(&accel2, 0x4B, 50);
  initMCP9808(&temp, 0x18, 3);
  ss.begin(GPSBaud); 
  
}

void loop() {
  pollSensors(&accel1, &accel2, &temp, &gps, accel1Data, accel2Data, tempData, lat, lon);
  updateAverageLIS331(accel1DataOld, accel1Data);
  updateAverageBNO080(accel2DataOld, accel2Data);
  updateAverageMCP9808(tempDataOld, tempData);
  Serial.println(accel1Data[0]);
  Serial.println(accel1Data[1]);
  Serial.println(accel1Data[2]);
  Serial.println(accel2Data[0]);
  Serial.println(accel2Data[1]);
  Serial.println(accel2Data[2]);
  Serial.println(tempData);
  //Serial.println("Lat: " + lat + " Lon: " + lon);
  
  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
  delay(1000);
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
