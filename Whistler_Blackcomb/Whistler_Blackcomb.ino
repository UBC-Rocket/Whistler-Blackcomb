//make sure to select teensy 3.6 for boards

/*Includes---------------------------------------------------------*/
#include "includes\sensors.h"
#include "includes\calculations.h"
#include "includes\prediction.h"

#include <SoftwareSerial.h>
#include "includes\modbus.h"
#include "includes\stream.h"

#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

/*
struct LJConfig conT7;
struct LJConfig conT4;

DeviceCalibrationT7 devCalT7;
DeviceCalibrationT4 devCalT4;

float labjackData[STREAM_MAX_SAMPLES_PER_PACKET_TCP] = {0};

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Our mac address. This is arbitrary
IPAddress ip(192, 168, 1, 178); // Our IP address. This is arbitrary

float labjackTemp; */

static unsigned long delta_time_set[15];

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

void loop()
{
	//labjackRead(&conT7, &devCalT7, labjackData);
	// Serial.println(voltsToTempT(labjackData[1], labjackData[0] * (-92.6) + 194.45), 7);
	//  Serial.println(voltsToTempT(-0.0005, -24));

	static unsigned long old_time = 0; //ms
    static unsigned long new_time = 0; //ms
    unsigned long delta_time;
	static float alt, x, y, z;

	new_time = millis();
    if ((new_time - old_time) >= 50) {
        delta_time = new_time - old_time;
        old_time = new_time;

		readAxesBNO080(&imu, x, y, z);

		predictionCalculation(&delta_time, delta_time_set, &alt, &x, &y, &z);

	}
}

// This custom version of delay() ensures that the gps object
// is being "fed".
// static void smartDelay(unsigned long ms)
// {
// 	unsigned long start = millis();
// 	do
// 	{
// 		while (ss.available())
// 			gps.encode(ss.read());
// 	} while (millis() - start < ms);
// }
