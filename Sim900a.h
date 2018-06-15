//Sim900A module library by Huynh Hoang Kha
//Using AT command at serial port

#pragma once
#ifndef _SIM900A_H_
#define _SIM900A_H_
#include<Arduino.h>

#ifdef __AVR_ATmega2560__
#define __MULTI_HARDWARE_SERIAL_PORT__
#ifndef __SERIAL1__
#define __SERIAL1__
#endif // !__SERIAL1__
#ifndef __SERIAL2__
#define __SERIAL2__
#endif // !__SERIAL2__
#endif //__AVR_ATmega2560__

#ifdef ARDUINO_ARCH_ESP8266
#define __SOFTWARE_SERIAL_SUPPORT__
#define __MULTI_HARDWARE_SERIAL_PORT__
#ifndef __SERIAL1__
#define __SERIAL1__
#endif // !__SERIAL1__
#endif //ARDUINO_ARCH_ESP8266

#ifdef __AVR_ATmega328P__
#define __SOFTWARE_SERIAL_SUPPORT__
#endif // __AVR_ATmega328P__

#ifdef __SOFTWARE_SERIAL_SUPPORT__
#include<SoftwareSerial.h>
#endif

#ifndef SIM900A_MESSAGE_LENGTH
#define SIM900A_MESSAGE_LENGTH 256
#endif // !SIM900A_MESSAGE_LENGTH

#define MAX_PHONE_NUMBER_LENGTH 16
#define SERIAL_TIMEOUT_MILLISECOND 1000

#define _SS_MAX_RX_BUFF 256
#if (RAMEND < 1000)
#define SERIAL_BUFFER_SIZE 16
#else
#define SERIAL_BUFFER_SIZE 64
#endif

class SIM900A {
public:
	enum SerialMode { 
		HARDWARE_SERIAL,
		SOFTWARE_SERIAL 
	};
	enum Sim900AEvent {
		INCOMING_MESSAGE,
		INCOMING_CALL,
		ACKNOWLEDGEMENT,
		UNKNOWN_EVENT,
		RECEIVING_ERROR,
		NO_DATA,
		USER_BUFFER_OVERFLOW,
		SYSTEM_BUFFER_OVERFLOW
	};
#ifdef __MULTI_HARDWARE_SERIAL_PORT__
	SIM900A(int serialPort, int baudRate);
#endif //__MULTI_HARDWARE_SERIAL_PORT__
	SIM900A(int baudRate);
#ifdef __SOFTWARE_SERIAL_SUPPORT__
	SIM900A(int rxPin, int txPin, int baudRate);
#endif // __SOFTWARE_SERIAL_SUPPORT__
	void sendATCommand(char* atCommand);
	bool isValidCharacter(char c, int indexInBuffer);
	Sim900AEvent receiveSerialData(char* buffer, int bufferLength);
	SerialMode getSerialMode();
#ifdef __SOFTWARE_SERIAL_SUPPORT__
	SoftwareSerial* getSoftwareSerialReference();
#endif // !__SOFTWARE_SERIAL_SUPPORT__
	bool sendMessage(char* phoneNumber, char* message);
	bool getLastMessage(char* phoneNumberContainer, int phoneNumberContainerSize, char* messageBuffer, int messageBufferSize);
	int getBaudRate();
private:
#ifdef __SOFTWARE_SERIAL_SUPPORT__
	SoftwareSerial* simSerial;
#endif // __SOFTWARE_SERIAL_SUPPORT__
	SerialMode serialMode;
	Sim900AEvent parseSerialData(char* buffer, int bufferLength);
	char lastMessageSender[MAX_PHONE_NUMBER_LENGTH];
	char lastMessageContent[SIM900A_MESSAGE_LENGTH];
	char lastCaller[MAX_PHONE_NUMBER_LENGTH];
	int baudRate;
	int hardwareSerialPort;
};

#endif // !_SIM900A_H_
