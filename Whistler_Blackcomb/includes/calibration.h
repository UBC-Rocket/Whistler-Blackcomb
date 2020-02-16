/**
 * Name: calibration.h
 * Desc: Provides functions for getting and applying T7 calibration.
 *       General calibration constants documentation can be found here:
 *       http://labjack.com/support/datasheets/t7/internal-flash
 * Auth: LabJack Corp.
**/

#ifndef CALIBRATION_H_
#define CALIBRATION_H_

#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>


typedef struct
{
	float PSlope;
	float NSlope;
	float Center;
	float Offset;
} CalSet;

//Stores T7 calibration constants
typedef struct
{
	CalSet HS[4];
	CalSet HR[4];

	struct
	{
		float Slope;
		float Offset;
	} DAC[2];

	float Temp_Slope;
	float Temp_Offset;

	float ISource_10u;
	float ISource_200u;

	float I_Bias;
} DeviceCalibration;

typedef struct{
    struct {
        float Slope;
        float Offset;
    } HV[4];
    struct {
        float Slope;
        float Offset;
    } LV;
    struct {
        float Slope;
        float Offset;
    } SpecV;
    struct {
        float Slope;
        float Offset;
    } DAC[2];

    float Temp_Slope;
    float Temp_Offset;

    float I_Bias;
} DeviceCalibrationT4;

//Gets the nominal calibration constants. No T7 communication is performed.
//devCal: The returned nominal calibration constants.
void getNominalCalibration(DeviceCalibration *devCal);

//Gets the calibration constants from a T7. Returns -1 on error, 0 on success.
//sock: The T7's socket.
//devCal: The returned calibration constants from the T7.
int getCalibration(EthernetClient * sock, DeviceCalibration *devCal);

//Converts AIN bytes to a calibrated voltage. Streaming only supports
//high speed resolutions. Returns -1 on error, 0 on success.
//devCal: The calibration constants to use.
//ainBytes: 2 byte, big endian byte array. This is the uncalibrated, binary
//          representation of the AIN reading and will be converted.
//gainIndex: The gain index (0 to 3) of the channel's reading.
//           Gain Indexes: 0 = +/-10 V, 1 = +/-1.0 V, 2 = +/-0.1 V,
//                         3 = +/-0.01 V
//volt: The returned calibrated voltage (V).
int ainBinToVolts(const DeviceCalibration *devCal, const unsigned char *ainBytes,
                  unsigned int gainIndex, float *volt);


/*-------------------T4 Calibration---------------------------------*/

//Gets the nominal calibration constants. No T7 communication is performed.
//devCal: The returned nominal calibration constants.
void getNominalCalibrationT4(DeviceCalibrationT4 *devCal);

//Gets the calibration constants from a T7. Returns -1 on error, 0 on success.
//sock: The T7's socket.
//devCal: The returned calibration constants from the T7.
int getCalibrationT4(EthernetClient * sock, DeviceCalibrationT4 *devCal);

//Converts AIN bytes to a calibrated voltage. Streaming only supports
//high speed resolutions. Returns -1 on error, 0 on success.
//devCal: The calibration constants to use.
//ainBytes: 2 byte, big endian byte array. This is the uncalibrated, binary
//          representation of the AIN reading and will be converted.
//gainIndex: The gain index (0 to 3) of the channel's reading.
//           Gain Indexes: 0 = +/-10 V, 1 = +/-1.0 V, 2 = +/-0.1 V,
//                         3 = +/-0.01 V
//volt: The returned calibrated voltage (V).
int ainBinToVoltsT4(const DeviceCalibrationT4 *devCal, const unsigned char *ainBytes,
                  unsigned int gainIndex, float *volt);

#endif