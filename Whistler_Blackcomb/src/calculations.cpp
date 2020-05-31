/*
Sensors Source

Contains implementation of calculations
*/

/*Includes-----------------------------------------------------------------*/
#include "../includes/calculations.h"
#include "../includes/SparkFun_LIS331.h"

/*Constants----------------------------------------------------------------*/


/*Functions----------------------------------------------------------------*/
void updateAverageLIS331(int16_t pastData[][LIS331DataLength], int16_t newData[]){
    for(int j=0; j<3; j++){
        int16_t sum=0; 
        for(int i=LIS331DataLength-1; i>0; i--){
            pastData[j][i]=pastData[j][i-1];
            sum+=pastData[j][i];
        }
        pastData[j][0]=newData[j];
        sum+=pastData[j][0];
        newData[j]=sum/LIS331DataLength;
    }
}

void updateAverageBNO080(float pastData[][BNO080DataLength], float newData[]){
    for(int j=0; j<3; j++){
        float sum=0; 
        for(int i=BNO080DataLength-1; i>0; i--){
            pastData[j][i]=pastData[j][i-1];
            sum+=pastData[j][i];
        }
        pastData[j][0]=newData[j];
        sum+=pastData[j][0];
        newData[j]=sum/BNO080DataLength;
    }
}

void updateAverageMCP9808(float pastData[], float *newData){
    float sum=0;
    for(int i=MCP9808DataLength-1; i>0; i--){
        pastData[i]=pastData[i-1];
        sum+=pastData[i];
    }
    pastData[0]=*newData;
    sum+=pastData[0];
    *newData=sum/BNO080DataLength;
}

// int test(){
//     return 3;
// }

