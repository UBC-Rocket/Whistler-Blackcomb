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

float[][] dotProduct(float x[][], float y[][]);

#endif