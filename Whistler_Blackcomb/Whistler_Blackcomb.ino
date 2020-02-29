/*Includes---------------------------------------------------------*/
#include "includes/radio.h"
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

#define SerialRadio             Serial3

float labjackData [STREAM_MAX_SAMPLES_PER_PACKET_TCP] = {0};

XBee radio = XBee();
static ZBTxRequest txPacket = ZBTxRequest();
static XBeeAddress64 gndAddr = XBeeAddress64(GND_STN_ADDR_MSB, GND_STN_ADDR_LSB);
static struct Dataset dataset;

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
float tempData;

int16_t accel1DataOld[3][LIS331DataLength];
float accel2DataOld[3][15];
float tempDataOld[15];

void setup() {
    SerialRadio.begin(921600);
    Serial.begin(9600);
    while(!SerialRadio){}
    radio.setSerial(SerialRadio);
    txPacket.setAddress64(gndAddr);
    Serial.begin(9600);
    Wire.begin();
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    delay(1500);
    digitalWrite(13, LOW);
    labjackSetup();
    Serial.println("Initialized");
}

void loop() {
    Serial.println("Reading labjack");
    labjackRead(labjackData);
    dataset.dataset1[0] = labjackData[0];
    dataset.dataset1[1] = labjackData[1];
    dataset.dataset1[2] = labjackData[2];
    Serial.println("Dataset numbers uwu: ");
    for(int i = 0; i<3; i++){
        Serial.print(dataset.dataset1[i]);
        Serial.print(" ");
    }
    Serial.println("");

    sendRadioDataset(radio, &txPacket, &dataset, 1);


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
