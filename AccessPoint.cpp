#include "AccessPoint.h"
#include <Arduino.h>
#include "ESP8266WiFi.h"
#include "Port.h"
extern HardwareSerial Serial;

void connectToAccessPoint() {
	Serial.println();
	Serial.printf("Connecting to %s ", WIFI_NAME);
	WiFi.begin(WIFI_NAME, PASSWORD);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("\nConnected.");
}