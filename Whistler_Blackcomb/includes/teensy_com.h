#ifndef TEENSY_COM_H
#define TEENSY_COM_H






void setupSerialCom();

void sendMessage(unsigned char message, unsigned char system_address);

void readMessage(unsigned char *, unsigned char *);

void generateMessage()


#endif