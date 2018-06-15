//Sim900A module library by Huynh Hoang Kha
//Using AT command at serial port

#include "Sim900a.h"

#ifdef __MULTI_HARDWARE_SERIAL_PORT__
SIM900A::SIM900A(int serialPort, int baudRate) {
	if (serialPort == 0) {
		this->serialMode = HARDWARE_SERIAL;
		this->baudRate = baudRate;
		this->hardwareSerialPort = 0;
		while (!Serial) Serial.begin(baudRate);
	}
#ifdef __SERIAL1__
	if (serialPort == 1) {
		this->serialMode = HARDWARE_SERIAL;
		this->baudRate = baudRate;
		this->hardwareSerialPort = 1;
		while (!Serial1) Serial1.begin(baudRate);
	}
#endif // __SERIAL1__
#ifdef __SERIAL2__
	if (serialPort == 2) {
		this->serialMode = HARDWARE_SERIAL;
		this->baudRate = baudRate;
		this->hardwareSerialPort = 2;
		while (!Serial2) Serial2.begin(baudRate);
	}
#endif
}
#endif // __MULTI_HARDWARE_SERIAL_PORT__

SIM900A::SIM900A(int baudRate) {
	this->serialMode = HARDWARE_SERIAL;
	this->baudRate = baudRate;
	this->hardwareSerialPort = 0;
	while (!Serial) Serial.begin(baudRate);
}

#ifdef __SOFTWARE_SERIAL_SUPPORT__
SIM900A::SIM900A(int rxPin, int txPin, int baudRate) {
	this->serialMode = SOFTWARE_SERIAL;
	this->baudRate = baudRate;
	this->simSerial = new SoftwareSerial(rxPin, txPin);
	this->simSerial->begin(baudRate);
	this->hardwareSerialPort = -1;
}
#endif

void SIM900A::sendATCommand(char * atCommand) {
#ifdef __SOFTWARE_SERIAL_SUPPORT__
	if (this->serialMode == SOFTWARE_SERIAL)
	this->simSerial->println(atCommand);
#endif
	if (this->serialMode == HARDWARE_SERIAL) {
		if (this->hardwareSerialPort == 0) Serial.println(atCommand);
#ifdef __SERIAL1__
		if (this->hardwareSerialPort == 1) Serial1.println(atCommand);
#endif // __SERIAL1__
#ifdef __SERIAL2__
		if (this->hardwareSerialPort == 2) Serial1.println(atCommand);
#endif // __SERIAL2__
	}
}

bool SIM900A::isValidCharacter(char c, int indexInBuffer) {
	if (indexInBuffer > 1) {
		if ((int)c == 13) return true;
		if ((int)c == 10) return true;
	}
	if ((int)c < 32 || (int)c > 126) return false;
	return true;
}

SIM900A::Sim900AEvent SIM900A::receiveSerialData(char* buffer, int bufferLength) {
	memset((void*)buffer, '\0' , bufferLength);
	int count = 0;
	char c = '\0';
#ifdef __SOFTWARE_SERIAL_SUPPORT__
	if (this->serialMode == SOFTWARE_SERIAL) {
		while (this->simSerial->available()>0) {
			c = this->simSerial->read();
			if (isValidCharacter(c, count)) buffer[count++] = c;
			if (count > bufferLength - 1) return USER_BUFFER_OVERFLOW;
			if (this->simSerial->overflow()) return SYSTEM_BUFFER_OVERFLOW;
			if (this->simSerial->available() < 2) delay((int)(1000.0f / ((float)this->baudRate)));
		}
	}
#endif // __SOFTWARE_SERIAL_SUPPORT__
	if (this->serialMode == HARDWARE_SERIAL) {
		if (this->hardwareSerialPort == 0) {
			while (Serial.available()>0) {
				c = Serial.read();
				if (isValidCharacter(c, count)) buffer[count++] = c;
				if (count > bufferLength - 1) return USER_BUFFER_OVERFLOW;
				if (Serial.available() < 2) delay((int)(1000.0f / ((float)this->baudRate)));
			}
		}
#ifdef __SERIAL1__
		if (this->hardwareSerialPort == 1) {
			while (Serial1.available()>0) {
				c = Serial1.read();
				if (isValidCharacter(c, count)) buffer[count++] = c;
				if (count > bufferLength - 1) return USER_BUFFER_OVERFLOW;
				if (Serial1.available() < 2) delay((int)(1000.0f / ((float)this->baudRate)));
			}
		}
#endif
#ifdef __SERIAL2__
		if (this->hardwareSerialPort == 2) {
			while (Serial2.available()>0) {
				c = Serial2.read();
				if (isValidCharacter(c, count)) buffer[count++] = c;
				if (count > bufferLength - 1) return USER_BUFFER_OVERFLOW;
				if (Serial2.available() < 2) delay((int)(1000.0f / ((float)this->baudRate)));
			}
		}
#endif // __SERIAL2__

	}
	if (strlen(buffer) > 0) return this->parseSerialData(buffer, bufferLength);
	else return NO_DATA;
}

SIM900A::SerialMode SIM900A::getSerialMode() {
	return this->serialMode;
}

#ifdef __SOFTWARE_SERIAL_SUPPORT__
SoftwareSerial * SIM900A::getSoftwareSerialReference() {
	if (this->serialMode == HARDWARE_SERIAL) return nullptr;
	else return this->simSerial;
}
#endif // __SOFTWARE_SERIAL_SUPPORT__

bool SIM900A::sendMessage(char * phoneNumber, char * message) {
	this->sendATCommand("AT+CMGF=1");
	delay(1000);
	char setPhoneNumberATCommand[32];
	memset((void*)setPhoneNumberATCommand, '\0', 32);
	strcpy(setPhoneNumberATCommand, "AT+CMGS=");
	setPhoneNumberATCommand[8] = '\"';
	strcpy(setPhoneNumberATCommand + 9, phoneNumber);
	setPhoneNumberATCommand[9 + strlen(phoneNumber)] = '\"';
	setPhoneNumberATCommand[10 + strlen(phoneNumber)] = '\r';
	this->sendATCommand(setPhoneNumberATCommand);
	delay(1000);
	this->sendATCommand(message);
	delay(100);
	this->sendATCommand("\x1A");
	delay(1000);
	return false;
}

bool SIM900A::getLastMessage(char * phoneNumberContainer, int phoneNumberContainerSize, char * messageBuffer, int messageBufferSize) {
	strcpy(phoneNumberContainer, this->lastMessageSender);
	strcpy(messageBuffer, this->lastMessageContent);
	return ((phoneNumberContainerSize > strlen(this->lastMessageSender))&&(messageBufferSize > strlen(this->lastMessageContent)));
}

int SIM900A::getBaudRate() {
	return this->baudRate;
}

SIM900A::Sim900AEvent SIM900A::parseSerialData(char* buffer, int bufferLength) {
	char dataTag[7];
	memset((void*)dataTag, '\0', 7);
	for (int i = 0; i < 7; i++) dataTag[i] = buffer[i];
	dataTag[6] = '\0';
	if (strcmp(dataTag, "+CMT: ") == 0) {
		memset((void*)lastMessageSender, '\0', MAX_PHONE_NUMBER_LENGTH);
		int i;
		for (i = 7; buffer[i] != '\"' && i < MAX_PHONE_NUMBER_LENGTH + 7; i++) this->lastMessageSender[i - 7] = buffer[i];
		this->lastMessageSender[i] = '\0';
		while (((int)buffer[i] != 10) && i < bufferLength - 1) i++;
		if ((int)buffer[i] == 10) strcpy(this->lastMessageContent, buffer + i + 1);
		int msgLength = strlen(this->lastMessageContent);
		while (!this->isValidCharacter(this->lastMessageContent[msgLength - 1], 0)) {
			this->lastMessageContent[msgLength - 1] = '\0';
			msgLength--;
		}
		return INCOMING_MESSAGE;
	}
	if (strcmp(dataTag, "RING  ")) return INCOMING_CALL;
	return UNKNOWN_EVENT;
}