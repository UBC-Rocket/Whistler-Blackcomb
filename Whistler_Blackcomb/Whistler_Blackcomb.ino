
#include <SD.h>
#include <SPI.h>



File datalog;

const int chipSelect = 3;

void setup()
{

    Serial.begin(9600);
    while (!Serial) {}


    Serial.print("\nInitializing SD card...");
    //SD library is default to SPI1, unsure how pin sharing will work
    if(!SD.begin(chipSelect)){
        Serial.println("initialization failed");
        return;
    }
    else
        Serial.println("Wiring is correct and a card is present.");

    datalog = SD.open("test.csv", FILE_WRITE);

    if(datalog){
        Serial.println("Writing to file");
        datalog.write("one, two, three, four\n");
        datalog.write("1,2,3,4\n");
        datalog.flush();
        datalog.write("hahah, hahaha, ha, aha\n");
        datalog.flush();
    }
    else
        Serial.println("Your fucked son");


}


void loop(void) {

}