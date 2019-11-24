#ifndef TEENSY_COM_H
#define TEENSY_COM_H






void setupSerialCom();

void sendMessage(unsigned char message, int system_address);

void readMessage(char message[]);


#endif