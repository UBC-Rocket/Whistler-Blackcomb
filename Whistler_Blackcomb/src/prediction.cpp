/*
Prediction Source

Contains implementation of calculations
*/

/*Includes-----------------------------------------------------------------*/
#include "../includes/prediction.h"
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

float[][] dotProduct(float x[][3], float y[][3]){
    float ret[3][3];
    // Initializing elements of matrix mult to 0.
	for(i = 0; i < 3; ++i)
	{
		for(j = 0; j < 3; ++j)
		{
			ret[i][j] = 0;
		}
	}

    // Dot product 
    for(i = 0; i < 3; ++i)
        for(j = 0; j < 3; ++j)
            for(k = 0; k < 3; ++k)
            {
                ret[i][j] += x[i][k] * y[k][j];
            }

    return ret;
}
