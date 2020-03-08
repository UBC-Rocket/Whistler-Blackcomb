/*
Prediction Source

Contains implementation of prediction
*/

/*Includes-----------------------------------------------------------------*/
#include "../includes/prediction.h"
#include "../includes/sensors.h"
#include "../includes/SparkFun_LIS331.h"

/*Constants----------------------------------------------------------------*/
const static float PI = 3.141592f;
const static float GRAVITY = 9.8f;

/*Functions----------------------------------------------------------------*/
float calculateAngle(float angularVelocityNew, float angularVelocityOld, unsigned long deltaTime, float angle){
    float slope, angleIntegration = 0;
    angularVelocityOld = degreeToRadians(angularVelocityOld);
	angularVelocityNew = degreeToRadians(angularVelocityNew);
	slope = (angularVelocityNew - angularVelocityOld)/(deltaTime);
	angleIntegration = (slope* (float)pow((double)angularVelocityNew,2) - slope*(float)pow((double)angularVelocityOld,2))/2;
	
	return angle + angleIntegration;
}

float degreeToRadians(float angle){
    float radians = 0;
    radians = (angle * PI) / 180;
    
    return radians;
}

void addToDeltaTimeSet(unsigned long* average_set, unsigned long data){
    static int i = 0;
    average_set[i] = data;
    if(i >= 15 - 1)
        i = 0;
    else
        i++;
}

float** dotProduct(float x[][3], float y[][3]){
    float ret[3][3];
    // Initializing elements of matrix mult to 0.
	for(int i = 0; i < 3; ++i)
	{
		for(int j = 0; j < 3; ++j)
		{
			ret[i][j] = 0;
		}
	}

    // Dot product 
    for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j)
            for(int k = 0; k < 3; ++k)
            {
                ret[i][j] += x[i][k] * y[k][j];
            }

    return ret;
}

void predictionCalculation(unsigned long *delta_time, unsigned long delta_time_set[], float* alt, float* x, float* y, float* z){
    
}
