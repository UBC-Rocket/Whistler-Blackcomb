//make sure to select teensy 4.0 for boards
/*
Ino file for the Whistler Blackcomb rocket
*/

/*Includes------------------------------------------------*/
//#include "includes\statemachine.h"
#include "includes\ArduinoLabjack.h"
#include "includes\modbus.h"
#include "includes\stream.h"

#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

/*Constants-----------------------------------------------*/
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 178); // Our IP address. This is arbitrary
byte IP_ADDR[] = { 192, 168, 1, 214 }; // Labjack's IP address

unsigned char value [4] = {0};
//
//unsigned char data [4] = {0xC6, 0x1C, 0x3C, 0x00};

double startTime=0;
double endTime=0;
double lastPrint=0;

int gQuit=0;

// Ports for command response and streaming respectively
const int CR_PORT = 502;
const int SP_PORT = 702;

EthernetClient crSock;
EthernetClient arSock;

//Stream config. settings. Except for NUM_ADDRESSES, configured later.
enum {NUM_ADDRESSES = 2};
float scanRate = 0;
unsigned int numAddresses = 0;
unsigned int samplesPerPacket = 0;
float settling = 0;
unsigned int resolutionIndex = 0;
unsigned int bufferSizeBytes = 0;
unsigned int autoTarget = 0;
unsigned int numScans = 0;
unsigned int scanListAddresses[NUM_ADDRESSES] = {0};
unsigned short nChanList[NUM_ADDRESSES] = {0};
float rangeList[NUM_ADDRESSES] = {0.0};
unsigned int gainList[NUM_ADDRESSES]; //Based off rangeList

//Stream read returns
unsigned short backlog = 0;
unsigned short Status = 0;
unsigned short additionalInfo = 0;

//Stream read loop variables
unsigned int i = 0, j = 0;
unsigned int addrIndex = 0;
float volts = 0.0f;
unsigned char *rawData;
int printStream = 1;
int printStreamStart = 0;
const double printStreamTimeSec = 1.0; //How often to print to the terminal in seconds.
double scanTotal = 0;
double numScansSkipped = 0;

double getTimeSec()
{
  // If more time resolution is wanted, use this instead. However, the buffer will overflow after only 70 minutes, so be careful: 
  // return micros()/1000000;
  return millis()/1000.0;
}

void setup() {
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
  Serial.begin(9600);
  if(!crSock.connect(IP_ADDR, CR_PORT) || !arSock.connect(IP_ADDR, SP_PORT))
    Serial.println("Socket connection failed");
  delay(100);

  //Configure stream
  scanRate = 250.0f; //Scans per second. Samples per second = scanRate * numAddresses
  numAddresses = NUM_ADDRESSES;
  samplesPerPacket = STREAM_MAX_SAMPLES_PER_PACKET_TCP;  //Max is 512. For better throughput set this to high values.
  settling = 10.0; //10 microseconds
  resolutionIndex = 0; //Default
  bufferSizeBytes = 0; //Default
  autoTarget = STREAM_TARGET_ETHERNET; //Stream target is Ethernet.
  numScans = 0; //0 = Run continuously.

  //Using a loop to add Modbus addresses for AIN0 - AIN(NUM_ADDRESSES-1) to the
  //stream scan and configure the analog input settings.
  /*for(i = 0; i < numAddresses; i++)
  {
    scanListAddresses[i] = i*2; //AIN(i) (Modbus address i*2)
    nChanList[i] = 199; //Negative channel is 199 (single ended)
    rangeList[i] = 10.0; //0.0 = +/-10V, 10.0 = +/-10V, 1.0 = +/-1V, 0.1 = +/-0.1V, or 0.01 = +/-0.01V.
    gainList[i] = 0; //gain index 0 = +/-10V 
  }
  Serial.println("Configuring analog inputs.\n");
  if(ainConfig(&crSock, numAddresses, scanListAddresses, nChanList, rangeList) != 0)
    Serial.println("ainConfig failed");
  */
  
  scanListAddresses[0]=61520;
  scanListAddresses[1]=61522;
  
  Serial.println("Configuring stream settings.\n");
  if(streamConfig(&crSock, scanRate, numAddresses, samplesPerPacket, settling, resolutionIndex, bufferSizeBytes, autoTarget, numScans, scanListAddresses) != 0)
  {
    Serial.println("streamConfig failed");
  }

  //Read back stream settings
  Serial.println("Reading stream configuration.\n");
  if(readStreamConfig(&crSock, &scanRate, &numAddresses, &samplesPerPacket, &settling, &resolutionIndex, &bufferSizeBytes, &autoTarget, &numScans) != 0)
    Serial.println("StreamConfig not read correctly!");
  if(numAddresses != NUM_ADDRESSES)
  {
    Serial.println("Modbus addresses were not set correctly.\n");
  }

  Serial.println("Reading stream scan list.\n");
  if(readStreamAddressesConfig(&crSock, numAddresses, scanListAddresses) != 0)
    Serial.println("readSreamAddressesConfig failed");
  /*
  Serial.println("Reading analog inputs configuration.\n");
  if(readAinConfig(&crSock, numAddresses, scanListAddresses, nChanList, rangeList) != 0)
    Serial.println("readAinAddressesConfig failed");
    */
  
  Serial.println("Starting Stream...");
  if(streamStart(&crSock) != 0)
  {
    Serial.println("Stopping stream\n");
    streamStop(&crSock);
  }

  startTime = getTimeSec();
  lastPrint=startTime;
  rawData=(unsigned char *)malloc(samplesPerPacket*STREAM_BYTES_PER_SAMPLE);
  
}

void loop() {
  if(!crSock.connected()){
    crSock.connect(IP_ADDR, CR_PORT);
    Serial.println("Client Disconnected!");
    while(crSock.available()){
      crSock.read();
    }
  }
  if(!arSock.connected()){
    arSock.connect(IP_ADDR, SP_PORT);
    Serial.println("Client Disconnected!");
    while(arSock.available()){
      arSock.read();
    }
  }
//  Serial.println(readMultipleRegistersTCP(&client, 55124,2,value));
//  Serial.println("Data:");
//  for(int i=0; i<4; i++){
//    Serial.println(value[i], HEX);
//  }
//  Serial.println(writeMultipleRegistersTCP(&client, 55124,2,data));
  
  backlog = 0;
  Status = 0;
  additionalInfo = 0;

  if(spontaneousStreamRead(&arSock, samplesPerPacket, &backlog, &Status, &additionalInfo, rawData) != 0)
  {
    Serial.println("Stream read failed");
  }else Serial.println("Stream Read Finished");
  backlog = backlog / (numAddresses*STREAM_BYTES_PER_SAMPLE); //Scan backlog
  streamStatusCheck(Status, &gQuit);

  for(j = 0; j < samplesPerPacket; j++)
  {
    if(printStream)
      {
        Serial.println(rawData[j*STREAM_BYTES_PER_SAMPLE]);
        Serial.println(rawData[j*STREAM_BYTES_PER_SAMPLE+1]);
      }
  }

  Serial.println("Time spent:");
  Serial.println(endTime-getTimeSec());
  endTime = getTimeSec();

  //delay(1000);
  if(gQuit){
    free(rawData);
    Serial.println("Stopping Stream...");
    streamStop(&crSock);
    for(;;);
  }
}
