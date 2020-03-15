/**
 * Name: stream.h
 * Desc: Provides T7 streaming functions over TCP.
 *       Low-level streaming configuration and auto response documentation can
 *       be found here:
 *       http://labjack.com/support/datasheets/t7/communication/stream-mode/low-level-streaming
 * Auth: LabJack Corp.
**/

#ifndef STREAM_H_
#define STREAM_H_

#include "./modbus.h"
#include "./calibration.h"
#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>

//Target types for stream configuration
#define STREAM_TARGET_ETHERNET 0x01  //Ethernet
#define STREAM_TARGET_USB 0x02  //USB
#define STREAM_TARGET_CR 0x10  //Command/Response

//Max samples per packet
#define STREAM_MAX_SAMPLES_PER_PACKET_TCP 512
#define STREAM_MAX_SAMPLES_PER_PACKET_USB 24

//Stream response statuses
#define STREAM_STATUS_AUTO_RECOVER_ACTIVE 2940
#define STREAM_STATUS_AUTO_RECOVER_END 2941  //Additional Info. = # scans skipped
#define STREAM_STATUS_SCAN_OVERLAP 2942
#define STREAM_STATUS_AUTO_RECOVER_END_OVERFLOW 2943
#define STREAM_STATUS_BURST_COMPLETE 2944

//Bytes per sample in the stream response
#define STREAM_BYTES_PER_SAMPLE 2

// Stream Constants
#define MAX_ADDRESSES 20

// Ethernet Settings
#define SOCK_TIMEOUT 10
#define CONNECT_TIMEOUT 2000

typedef struct {
    // Network details
    int CR_PORT;
    int SP_PORT;
    IPAddress IP_ADDR;

    // Sockets for connection
    EthernetClient crSock;
    EthernetClient arSock;

    // Type of device. 4 = T4 and 7 = T7
    unsigned int type;

    // Config Settings
    float scanRate;
    unsigned int numAddresses;
    unsigned int samplesPerPacket;
    float settling;
    unsigned int resolutionIndex;
    unsigned int bufferSizeBytes;
    unsigned int autoTarget;
    unsigned int numScans;
    unsigned int printStream;

    // Specific channel information
    unsigned int scanListAddresses[MAX_ADDRESSES];
    unsigned short nChanList[MAX_ADDRESSES];
    float rangeList[MAX_ADDRESSES];
    unsigned int gainList[MAX_ADDRESSES];

    // Read time stream variables
    unsigned short gCurTransID;
    double lastReadTime;

} LJConfig;

// Sets the Labjack up for streaming
// config: LJConfig struct containing config information to read/write from (see configT4/7)
// devCal: device calibration struct to use. This is a void pointer because either a T7 or T4 devCal struct can be passed in, 
// as long as it's type matches that specified in LJConfig.type
int labjackSetup(LJConfig * config, void * devCal, int streamReset);

// Reads Labjack stream, if available. Returns 1 if stream was available and read and 0 if the packet isn't ready yet
// config: LJConfig struct containing configuration information
// devCal: device calibration to convert from output to volts. See labjackSetup. 
// data: array of data to return to. This should have length of the number of samples per packet
int labjackRead(LJConfig * config, void * devCal, float * data);

// Configures T7 with desired options. To change any desired settings (e.g. channels to be read), change the implementation of this function
// config: LJConfig struct to write to
void configT7(LJConfig * config);

// Exact same as configT7 except for the T4 (see above)
void configT4(LJConfig * config);

int configTC(EthernetClient * sock, unsigned int numSensors, const unsigned int * posAINList, const unsigned short * negAINList, const unsigned int * typeList);

// Returns time in milliseconds
double getTimeSec();

//Reads the analog input settings that are going to be streamed.
//Returns -1 on error, 0 on success.
//sock: The T7's socket. The socket needs to be on port 502.
//numAddresses: The number of Modbus addresses in the addressList array.
//scanListAddresses: Array containing the AIN Modbus addresses in the scan.
//                   Needs to have numAddresses elements.
//nChannelList: Array where the negative channels for scanListAddresses are
//              returned. Needs to have numAddresses elements.
//rangeList: Array where ranges for scanListAddresses are returned. Needs to
//           have numAddresses elements.
int readAinConfig(EthernetClient * sock, unsigned int numAddresses,
                  unsigned int *scanListAddresses, unsigned short *nChannelList,
                  float *rangeList);

//Configures the analog input settings that are going to be streamed.
//Returns -1 on error, 0 on success.
//sock: The T7's socket. The socket needs to be on port 502.
//numAddresses: The number of Modbus addresses in the addressList array.
//scanListAddresses: Array containing the AIN Modbus addresses in the scan.
//                   Needs to have numAddresses elements.
//nChannelList: Array containing the negative channels for addressList.
//              Needs to have scanListAddresses elements.
//rangeList: Array containing the ranges for scanListAddresses. Needs to have
//           numAddresses elements.
int ainConfig(EthernetClient * sock, unsigned int numAddresses, 
              const unsigned int *scanListAddresses, const unsigned short *nChannelList,
              const float *rangeList);

//Reads the current stream configuration on a T7. Returns -1 on error, 0 on
//success. 
//Check streamConfig function for parameter information.
int readStreamConfig(EthernetClient * sock, float *scanRate,
                     unsigned int *numAddresses, unsigned int *samplesPerPacket,
                     float *settling, unsigned int *resolutionIndex,
                     unsigned int *bufferSizeBytes, unsigned int *autoTarget,
                     unsigned int *numScans);

//Reads the current stream scan Modbus addresses configured on a T7. Call
//readStreamConfig first and use the returned numAddresses for the size of
//the scanListAddresses. Returns -1 on error, 0 on success.
int readStreamAddressesConfig(EthernetClient * sock, unsigned int numAddresses,
                              unsigned int *scanListAddresses);

//Configures streaming on a T7. Use AINConfig to configure AIN ranges and
//negative channels. Returns -1 on error, 0 on success.
//sock: The T7's socket. The socket needs to be on port 502.
//scanRate: The number of times per second that all channels in the scanlist
//          will be read.
//          Sample Rate (Hz) = scanRate * numAddresses
//numAddresses: The number of entries in the scan list.
//samplesPerPacket: Specifies the number of data points to be sent in the data
//                  packet. Only applies to spontaneous mode.
//                  For better throughput set this to high values. Maximum
//                  values (STREAM_MAX_SAMPLES_PER_PACKET_X) are defined in
//                  this header.
//settling: Time in microseconds to allow signals to settle after switching the
//          mux. Default=0 equates to 10 us if sample rate <80k, and 6 us for
//          higher sample rates. Max is 5000.
//resolutionIndex: Index specifying the resolution of the data. High settings
//                 will have lower max speeds.
//                 0 = default = Res. Index 1
//bufferSizeBytes: Size of the stream data buffer in bytes. A value of 0
//                 equates to the default value. Must be a power of 2, as it
//                 takes 2 bytes to hold 1 sample.
//autoTarget: Controls where data will be sent. Value is a bitmask. These values
//            (STREAM_TARGET_X) are defined in this header file.
//numScans: The number of scans to run before automatically stopping
//          (stream-burst). 0 = run continuously.
//          Max number of samples is bufferSizeBytes/2.
//          Max number of scans is (bufferSizeBytes/2)/numAddresses.
//scanlistAddresses: Array containing the list of addresses to read each scan.
//                   In the case of Stream-Out enabled, the list may also
//                   include something to write each scan. 
int streamConfig(EthernetClient * sock, float scanRate,
                 unsigned int numAddresses, unsigned int samplesPerPacket,
                 float settling, unsigned int resolutionIndex,
                 unsigned int bufferSizeBytes, unsigned int autoTarget,
                 unsigned int numScans, const unsigned int *scanListAddresses);

//Starts streaming on a T7. Call the streamConfig function first. Returns -1 on
//error, 0 on success.
//sock: The device's socket. The socket needs to be on port 502.
int streamStart(EthernetClient * sock, unsigned short * transID);

//Reads stream samples from an spontaneous stream packet from a T7. Samples
//are in raw data form and will need to be converted to a voltage. Use the
//ainBinToVolt function in calibration.h for converting data to voltages.
//Returns -1 on error, 0 on success.
//sock: The T7's socket. The socket needs to be on port 702.
//samplesPerPacket: The number of samples in one stream packet. You configure
//                  this in the streamConfig call.
// transID: Current transaction ID
//backlog: Number of bytes in the T7's stream buffer. Reads clear the buffer.
//         A buffer overflow will occur when the backlog reaches the T7's
//         stream buffer limit (bufferSizeIndex) configured in the streamConfig
//         call.
//status: Status codes about the stream. Codes (STREAM_STATUS_X) defined in
//        this header.
//additionalInfo: Additional status information.
//rawData: Byte array containing the raw sample data. The array needs to have
//         samplesPerPacket * 2 elements (1 sample = 2 bytes).
int spontaneousStreamRead(EthernetClient * sock, unsigned int samplesPerPacket, unsigned short * transID, 
                          unsigned short *backlog, unsigned short *status, 
                          unsigned short *additionalInfo, unsigned char *rawData);

//Stops the currently running stream on a T7. Returns -1 on error, 0 on
//success.
//sock: The T7's socket. The socket needs to be on port 502.
int streamStop(EthernetClient * sock);

int streamStatusCheck(unsigned short Status, int * gQuit);

#endif