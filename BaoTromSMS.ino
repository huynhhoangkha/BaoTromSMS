#include "Sim900a.h"
#include "AccessPoint.h"
#include "Configurations.h"
#include "WifiRoutine.h"
#include "ESP8266Wifi.h"
#define _DEBUG_

SIM900A sim(SIM_RX_PIN, SIM_TX_PIN, 9600);
WiFiClient nodeMCU;
char buffer[BUFFER_SIZE];
volatile bool alarmActivated;
bool systemActivated, magneticSwitchEnabled, pirSensorEnabled;

void activateAlarm();
void deactivateAlarm();
void alarmViaSMS();
void magneticSwitchInterruptRoutine();
void pirSensorInterruptRoutine();
void onReceivingMessageRoutine();
REQUEST getRequestFromMessage();
void sendSystemInfo();

void setup() {
	Serial.begin(9600);
	sim.sendATCommand("AT+IPR=9600");
	pinMode(MAGNETIC_SWITCH_PIN, INPUT_PULLUP);
	digitalWrite(MAGNETIC_SWITCH_PIN, HIGH);
	pinMode(PIR_SENSOR_PIN, INPUT);
	pinMode(SPEAKER_PIN, OUTPUT);
	pinMode(LAMP_PIN, OUTPUT);
	systemActivated = false; //should be false
	magneticSwitchEnabled = true;
	pirSensorEnabled = true;
	alarmActivated = false;
	delay(1000);
	attachInterrupt(MAGNETIC_SWITCH_PIN, magneticSwitchInterruptRoutine, RISING);
	attachInterrupt(PIR_SENSOR_PIN, pirSensorInterruptRoutine, RISING);
}

void loop() {
	if (alarmActivated) {
		activateAlarm();
		alarmViaSMS();
		alarmActivated = false;
	}
	if (sim.receiveSerialData(buffer, BUFFER_SIZE) == SIM900A::INCOMING_MESSAGE) {
		onReceivingMessageRoutine();
	}
	if (strlen(buffer) > 0) {
		Serial.print("Number of byte(s): ");
		Serial.println(strlen(buffer));
		Serial.println(buffer);
	}
	delay(1000);
}

void activateAlarm() {
	digitalWrite(SPEAKER_PIN, HIGH);
	digitalWrite(LAMP_PIN, HIGH);
	Serial.println("Alarm activated.");
}

void deactivateAlarm() {
	digitalWrite(SPEAKER_PIN, LOW);
	digitalWrite(LAMP_PIN, LOW);
	alarmActivated = false;
	Serial.println("Alarm deactivated.");
}

void alarmViaSMS() {
	sim.sendMessage(ADMIN_PHONE_NUMBER, BROKE_IN_ALARM_MSG);
}

void magneticSwitchInterruptRoutine() {
	if (magneticSwitchEnabled && systemActivated) alarmActivated = true;
}

void pirSensorInterruptRoutine() {
	if (pirSensorEnabled && systemActivated) alarmActivated = true;
}

void onReceivingMessageRoutine() {
	REQUEST request = getRequestFromMessage();
	Serial.print("Request received: ");
	Serial.println(request);
	switch (request) {
	case INFO:
		sendSystemInfo();
		break;
	case ACTIVE:
		systemActivated = true;
		updateStatusToServer(&nodeMCU);
		sim.sendMessage(ADMIN_PHONE_NUMBER, ACTIVE_RESPONSE_MSG);
		break;
	case DEACTIVE:
		deactivateAlarm();
		systemActivated = false;
		updateStatusToServer(&nodeMCU);
		sim.sendMessage(ADMIN_PHONE_NUMBER, DEACTIVE_RESPONSE_MSG);
		break;
	case ALARM_ACTIVE:
		alarmActivated = true;
		updateStatusToServer(&nodeMCU);
		sim.sendMessage(ADMIN_PHONE_NUMBER, ALARM_ACTIVE_RESPONSE_MSG);
		break;
	case ALARM_DEACTIVE:
		deactivateAlarm();
		updateStatusToServer(&nodeMCU);
		sim.sendMessage(ADMIN_PHONE_NUMBER, ALARM_DEACTIVE_RESPONSE_MSG);
		break;
	case PIR_ENABLE:
		pirSensorEnabled = true;
		updateStatusToServer(&nodeMCU);
		sim.sendMessage(ADMIN_PHONE_NUMBER, PIR_ENABLE_RESPONSE_MSG);
		break;
	case PIR_DISABLE:
		pirSensorEnabled = false;
		updateStatusToServer(&nodeMCU);
		sim.sendMessage(ADMIN_PHONE_NUMBER, PIR_DISABLE_RESPONSE_MSG);
		break;
	case MAG_ENABLE:
		magneticSwitchEnabled = true;
		updateStatusToServer(&nodeMCU);
		sim.sendMessage(ADMIN_PHONE_NUMBER, MAG_ENABLE_RESPONSE_MSG);
		break;
	case MAG_DISABLE:
		magneticSwitchEnabled = false;
		updateStatusToServer(&nodeMCU);
		sim.sendMessage(ADMIN_PHONE_NUMBER, MAG_DISABLE_RESPONSE_MSG);
		break;
	case INVALID_REQUEST:

		break;
	case UNKNOWN_USER:

		break;
	default:
		
		break;
	}
}

REQUEST getRequestFromMessage() {
	char sender[MAX_PHONE_NUMBER_LENGTH];
	char message[SIM900A_MESSAGE_LENGTH];
	sim.getLastMessage(sender, MAX_PHONE_NUMBER_LENGTH, message, SIM900A_MESSAGE_LENGTH);
	Serial.print("Message received: ");
	Serial.println(message);
	Serial.print("From: ");
	Serial.println(sender);
	if (strcmp(sender, ADMIN_PHONE_NUMBER) != 0) return UNKNOWN_USER;
	if (strcmp(message, INFO_REQUEST_MSG) == 0) return INFO;
	if (strcmp(message, ACTIVE_REQUEST_MSG) == 0) return ACTIVE;
	if (strcmp(message, DEACTIVE_REQUEST_MSG) == 0) return DEACTIVE;
	if (strcmp(message, ALARM_ACTIVE_REQUEST_MSG) == 0) return ALARM_ACTIVE;
	if (strcmp(message, ALARM_DEACTIVE_REQUEST_MSG) == 0) return ALARM_DEACTIVE;
	if (strcmp(message, PIR_ENABLE_REQUEST_MSG) == 0) return PIR_ENABLE;
	if (strcmp(message, PIR_DISABLE_REQUEST_MSG) == 0) return PIR_DISABLE;
	if (strcmp(message, MAG_ENABLE_REQUEST_MSG) == 0) return MAG_ENABLE;
	if (strcmp(message, MAG_DISABLE_REQUEST_MSG) == 0) return MAG_DISABLE;
	return INVALID_REQUEST;
}

void sendSystemInfo() {
	int textPtr = 0;
	char infoMessage[64];
	char* systemStr = "System: ";
	char* alarmStr = "Alarm: ";
	char* lampStr = "Lamp: ";
	char* buzzerStr = "Buzzer: ";
	char* pirStr = "PIR sensor: ";
	char* magStr = "Door-open sensor: ";
	char* onStr = "on.\n";
	char* offStr = "off.\n";
	char* activeStr = "active.\n";
	char* inActiveStr = "inactive.\n";

	strcpy(infoMessage, systemStr);
	textPtr += strlen(systemStr);
	if (systemActivated) {
		strcpy(infoMessage + textPtr, activeStr);
		textPtr += strlen(activeStr);
	}
	else {
		strcpy(infoMessage + textPtr, inActiveStr);
		textPtr += strlen(inActiveStr);
	}

	strcpy(infoMessage + textPtr, pirStr);
	textPtr += strlen(pirStr);
	if (pirSensorEnabled) {
		strcpy(infoMessage + textPtr, onStr);
		textPtr += strlen(onStr);
	}
	else {
		strcpy(infoMessage + textPtr, offStr);
		textPtr += strlen(offStr);
	}

	strcpy(infoMessage + textPtr, magStr);
	textPtr += strlen(magStr);
	if (magneticSwitchEnabled) {
		strcpy(infoMessage + textPtr, onStr);
		textPtr += strlen(onStr);
	}
	else {
		strcpy(infoMessage + textPtr, offStr);
		textPtr += strlen(offStr);
	}
	sim.sendMessage(ADMIN_PHONE_NUMBER, infoMessage);
	Serial.println(infoMessage);
}