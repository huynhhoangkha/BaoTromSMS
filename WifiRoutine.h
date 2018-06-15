#pragma once
#ifndef _WIFI_ROUTINE_H_
#define _WIFI_ROUTINE_H_
#include "Configurations.h"
#include "ESP8266Wifi.h"
#define APPLICATION_SIGNATURE	"NodeMCU-signature" //17 bytes

bool connectToServer(WiFiClient* nodeMCU);
REQUEST receiveRequestFromServer(WiFiClient* nodeMCU);
void updateStatusToServer(WiFiClient* nodeMCU);

#endif // !_WIFI_ROUTINE_H_
