#include "WifiRoutine.h"
#include <Arduino.h>
#include "ESP8266WiFi.h"
#include "WiFiClient.h"
#include "Port.h"
#include "Configurations.h"
#include <string>

extern volatile bool alarmActivated;
extern bool systemActivated, magneticSwitchEnabled, pirSensorEnabled;

bool connectToServer(WiFiClient* nodeMCU) {
	//Section: Connect to the server via TCP/IP
	
	Serial.print("Connecting to server socket: ");
	Serial.print(SERVER_IPV4);
	Serial.print(":");
	Serial.println(DESTINATION_TCP_PORT);
	int stt = nodeMCU->connect(SERVER_IPV4, DESTINATION_TCP_PORT);
	if (stt == 0) {
		Serial.println("Can not establish connection.");
		return false;
	}
	else if (stt == 1) {
		Serial.println("Connection established.");
		return true;
	}
	//Endsection: Connect to the server via TCP/IP
}

REQUEST receiveRequestFromServer(WiFiClient* nodeMCU) {
	return INVALID_REQUEST;
}

void updateStatusToServer(WiFiClient* nodeMCU) {
	String infoMsg = APPLICATION_SIGNATURE;			//Signature
	if (systemActivated) infoMsg.concat('1');		//System status
	else infoMsg.concat('0');
	if (magneticSwitchEnabled) infoMsg.concat('1');	//Magnetic status
	else infoMsg.concat('0');
	if (pirSensorEnabled) infoMsg.concat('1');		//Pir status
	else infoMsg.concat('0');
	if (alarmActivated) infoMsg.concat('1');		//Alarm status
	else infoMsg.concat('0');
	nodeMCU->print(infoMsg);
}
