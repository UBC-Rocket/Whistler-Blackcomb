//make sure to select teensy 3.6 for boards

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

struct LJConfig conT7;
struct LJConfig conT4;

DeviceCalibrationT7 devCalT7;
DeviceCalibrationT4 devCalT4;

float labjackData[STREAM_MAX_SAMPLES_PER_PACKET_TCP] = {0};

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Our mac address. This is arbitrary
IPAddress ip(192, 168, 1, 178); // Our IP address. This is arbitrary

// GPS
TinyGPSPlus gps;
#define ss Serial1


float labjackTemp;

void setup()
{
	Serial.begin(9600);
	Wire.begin();
	
	// Ethernet Setup
	// Required because the WIZ820io requires a reset pulse and the Teensy doesn't have a reset pin (pin 9 is used instead)
	pinMode(9, OUTPUT);
	digitalWrite(9, LOW);    // begin reset the WIZ820io
	pinMode(10, OUTPUT);
	digitalWrite(10, HIGH);  // de-select WIZ820io
	// Note: delay's weren't in the original code here: https://www.pjrc.com/store/wiz820_sd_adaptor.html
	// Delays were added because of reset pin specs here: http://wiznethome.cafe24.com/wp-content/uploads/wiznethome/Network%20Module/WIZ820io/Document/WIZ820io_User_Manual_V1.0.pdf
	delay(500);
	digitalWrite(9, HIGH);   // end reset pulse
	// When this delay is any lower, the ethernet client disconnects after 1-3 reads, after which it reconnects and works perfectly after that. With this delay that never happens, but not sure exactly why. 
	delay(4000);

	Ethernet.begin(mac, ip);

	configT7(&conT7);

	configT4(&conT4);

	labjackSetup(&conT7, &devCalT7);
	
	// labjackSetup(&conT4);
}

void loop()
{
	labjackRead(&conT7, &devCalT7, labjackData);
	// Serial.println(voltsToTempT(labjackData[1], labjackData[0] * (-92.6) + 194.45), 7);
	//  Serial.println(voltsToTempT(-0.0005, -24));
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
