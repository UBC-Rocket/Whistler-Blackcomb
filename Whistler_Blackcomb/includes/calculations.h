/*
Calculations header

File for calculations to be done during fligh
*/
#ifndef CALCULATION_H
#define CALCULATION_H

/*Includes--------------------------------------------------------------------*/
#include "SparkFun_LIS331.h"

/*Constants-------------------------------------------------------------------*/
const int LIS331DataLength = 15;
const int BNO080DataLength = 15;
const int MCP9808DataLength = 15;

/*Functions-------------------------------------------------------------------*/
void updateAverageLIS331(int16_t pastData[][15], int16_t newData[]);

void updateAverageBNO080(float pastData[][15], float newData[]);

void updateAverageMCP9808(float pastData[], float newData);

#endif