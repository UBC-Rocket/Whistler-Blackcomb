//make sure to select teensy 3.6 for boards

/*Includes---------------------------------------------------------*/
#include "./includes/sensors.h"
#include "./includes/calculations.h"
#include "./includes/prediction.h"

#include <SoftwareSerial.h>
#include "./includes/modbus.h"
#include "./includes/stream.h"

#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>

BNO080 myIMU;
unsigned long lastTime;
quaternion orientation;
quaternion acceleration;

float gravityAccel[3];
float position[] = {0, 0, 0};
float velocity[] = {0, 0, 0};
float accel[] = {0, 0, 0};
// Dummy covariance matrices for now since these are yet to be determined
float stateCovariance[][2][2] = {{{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}};
float processCovariance[2][2] = {{0, 0}, {0, 0}};

void setup() { 
    Serial.begin(9600);
    Wire.begin();
    Wire.setClock(400000);
    delay(500);

    myIMU.begin();
    myIMU.enableGameRotationVector(50);
    myIMU.enableGyro(50);
    myIMU.enableAccelerometer(50);

    orientation.q0 = 1;
    orientation.q1 = 0;
    orientation.q2 = 0;
    orientation.q3 = 0;

    // Get initial gravity acceleration
    while(!myIMU.dataAvailable() || myIMU.getAccelZ() == 0){
        delay(10);
    }

    gravityAccel[0] = myIMU.getAccelX();
    gravityAccel[1] = myIMU.getAccelY();
    gravityAccel[2] = myIMU.getAccelZ();
    Serial.println(gravityAccel[2]);
}

void loop()
{
    if(myIMU.dataAvailable()){
        // Quaternion from sensor library
        float quatI = myIMU.getQuatI();
        float quatJ = myIMU.getQuatJ();
        float quatK = myIMU.getQuatK();
        float quatReal = myIMU.getQuatReal();
        float quatRadianAccuracy = myIMU.getQuatRadianAccuracy();
        Serial.print(quatI, 2);
        Serial.print(", ");
        Serial.print(quatJ, 2);
        Serial.print(", ");
        Serial.print(quatK, 2);
        Serial.print(", ");
        Serial.print(quatReal, 2);
        Serial.println();

        float x = myIMU.getGyroX();
        float y = myIMU.getGyroY();
        float z = myIMU.getGyroZ();
        acceleration.q0 = 0;
        acceleration.q1 = myIMU.getAccelX();
        acceleration.q2 = myIMU.getAccelY();
        acceleration.q3 = myIMU.getAccelZ();

        // Get calculated orientation quaternion
        orientation = getOrientation((millis() - lastTime) / 1000.0, orientation, x, y, z);
        // Convert relative acceleration to world reference acceleration
        acceleration = qMult(qMult(orientation, acceleration), qConjugate(orientation));

        accel[0] = acceleration.q1 - gravityAccel[0];
        accel[1] = acceleration.q2 - gravityAccel[1];
        accel[2] = acceleration.q3 - gravityAccel[2];

        // Predict phase of Kalman filter (without GPS, all we can do)
        predictFilter((millis() - lastTime) / 1000.0, position, velocity, accel, stateCovariance, processCovariance);
        
        // float mat[][2] = {{1, 2}, {3, 4}};
        // transpose(mat);
        Serial.print(orientation.q1, 2);
        Serial.print(", ");
        Serial.print(orientation.q2, 2);
        Serial.print(", ");
        Serial.print(orientation.q3, 2);
        Serial.print(", ");
        Serial.print(orientation.q0, 2);
        Serial.println();
        Serial.print(accel[0], 2);
        Serial.print(", ");
        Serial.print(accel[0], 2);
        Serial.print(", ");
        Serial.print(accel[1], 2);
        Serial.print(", ");
        Serial.print(accel[2], 2);
        Serial.println();

        Serial.print(position[0], 2);
        Serial.print(", ");
        Serial.print(position[1], 2);
        Serial.print(", ");
        Serial.print(position[2], 2);
        Serial.println();
        Serial.println();

        // For first 2 seconds continuously update the average acceleration due to gravity (you have to leave IMU completely still for this time)
        if(millis() < 2000){
            int n = millis() / 50;
            gravityAccel[0] = (gravityAccel[0] * n + myIMU.getAccelX()) / (n+1);
            gravityAccel[1] = (gravityAccel[1] * n + myIMU.getAccelY()) / (n+1);
            gravityAccel[2] = (gravityAccel[2] * n + myIMU.getAccelZ()) / (n+1);
        }

        lastTime = millis();
    }
}
