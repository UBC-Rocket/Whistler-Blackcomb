/*
Trajectory Prediction header

File for calculating trajectory and predicting impact
*/
#ifndef PREDICTION_H
#define PREDICTION_H

/*Includes--------------------------------------------------------------------*/
#include "SparkFun_BNO080_Arduino_Library.h"
#include "SparkFun_LIS331.h"
#include <math.h>

/*Functions-------------------------------------------------------------------*/
float calculateAngle(float angularVelocityNew, float angularVelocityOld, unsigned long deltaTime, float angle);

float degreeToRadians(float angle);

void addToDeltaTimeSet(unsigned long* average_set, unsigned long data);

float** dotProduct(float x[][3], float y[][3]);

void predictionCalculation(unsigned long *delta_time, unsigned long delta_time_set[], float* alt, float* x, float* y, float* z);

#endif