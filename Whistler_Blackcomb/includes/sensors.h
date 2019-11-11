/*
Sensors header

File for functions of initializations and data retrieval from sensors
*/
#ifndef SENSORS_H
#define SENSORS_H

/*Includes--------------------------------------------------------------------*/
#include "Adafruit_MCP9808.h"
#include "SparkFun_BNO080_Arduino_Library.h"
#include "SparkFun_LIS331.h"

/*Functions-------------------------------------------------------------------*/
void initLIS331(LIS331 *sensor, uint8_t address);

void initBNO080(BNO080 *sensor, uint8_t address, uint8_t period);

void readAxesBNO080(BNO080 *sensor, float &x, float &y, float &z);

void initMCP9808(Adafruit_MCP9808 *sensor, uint8_t address, uint8_t res);

void pollSensors(LIS331 *accel1, BNO080 *accel2, Adafruit_MCP9808 *temp, int16_t accel1Data[], float accel2Data[], float &tempData);

#endif