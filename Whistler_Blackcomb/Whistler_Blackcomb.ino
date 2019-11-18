//make sure to select teensy 4.0 for boards

/*Includes---------------------------------------------------------*/
#include "includes\sensors.h"
#include <SoftwareSerial.h>
//#include "includes\SparkFun_LIS331.h"

LIS331 accel1;
BNO080 accel2;
Adafruit_MCP9808 temp =Adafruit_MCP9808();

// GPS 
TinyGPSPlus gps;
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 4800;
float lat;
float lon;

SoftwareSerial ss(RXPin, TXPin); // The serial connection to the GPS device

//BNO080 myBNO080;
int16_t accel1Data[3];
float accel2Data[3];
float tempData;


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
  /*// put your main code here, to run repeatedly:
  xl.readAxes(x, y, z);
  Serial.println(xl.convertToG(6,x), 2); // The convertToG() function
  Serial.println(xl.convertToG(6,y), 2); // accepts as parameters the
  Serial.println(xl.convertToG(6,z), 2); // raw value and the current
  Serial.println(" ");                   // maximum g-rating.
  Serial.println(x2.dataAvailable());
  readAxesBNO080(&x2, xx, yy, zz);
  Serial.print(xx, 2);
  Serial.print(F(","));
  Serial.print(yy, 2);
  Serial.print(F(","));
  Serial.print(zz, 2);
  Serial.print(F(","));
  temp=x3.readTempC();
  Serial.println(temp);*/
  pollSensors(&accel1, &accel2, &temp, accel1Data, accel2Data, tempData, &gps, lat, lon);
  Serial.println(accel1Data[0]);
  Serial.println(accel1Data[1]);
  Serial.println(accel1Data[2]);
  Serial.println(accel2Data[0]);
  Serial.println(accel2Data[1]);
  Serial.println(accel2Data[2]);
  Serial.println(tempData);
  Serial.println("Lat: " + lat + " Lon: " + lon);
  
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
