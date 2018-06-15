#pragma once
#ifndef _CONFIGURATIONS_H_
#define _CONFIGURATIONS_H_

#define MAGNETIC_SWITCH_PIN D1
#define PIR_SENSOR_PIN D2
#define SPEAKER_PIN D5
#define LAMP_PIN D6
#define SIM_RX_PIN D8
#define SIM_TX_PIN D7
#define BAUDRATE 9600
#define BUFFER_SIZE 128

#define ADMIN_PHONE_NUMBER			"+841645862895"
#define INFO_REQUEST_MSG			"Info"
#define ACTIVE_REQUEST_MSG			"On"
#define DEACTIVE_REQUEST_MSG		"Off"
#define ALARM_ACTIVE_REQUEST_MSG	"Alarm on"
#define ALARM_DEACTIVE_REQUEST_MSG	"Alarm off"
#define PIR_ENABLE_REQUEST_MSG		"Pir on"
#define PIR_DISABLE_REQUEST_MSG		"Pir off"
#define MAG_ENABLE_REQUEST_MSG		"Mag on"
#define MAG_DISABLE_REQUEST_MSG		"Mag off"

#define ACTIVE_RESPONSE_MSG			"System has been activated."
#define DEACTIVE_RESPONSE_MSG		"System has been deactivated."
#define ALARM_ACTIVE_RESPONSE_MSG	"Alarming."
#define ALARM_DEACTIVE_RESPONSE_MSG	"Alarm stopped."
#define PIR_ENABLE_RESPONSE_MSG		"PIR sensor has been enabled."
#define PIR_DISABLE_RESPONSE_MSG	"PIR sensor has been disabled."
#define MAG_ENABLE_RESPONSE_MSG		"Door-open sensor has been enabled."
#define MAG_DISABLE_RESPONSE_MSG	"Door-open sensor has been disabled."
#define BROKE_IN_ALARM_MSG			"Alarm: There has been an inrush!"

enum REQUEST {
	INFO,
	ACTIVE,
	DEACTIVE,
	ALARM_ACTIVE,
	ALARM_DEACTIVE,
	PIR_ENABLE,
	PIR_DISABLE,
	MAG_ENABLE,
	MAG_DISABLE,
	INVALID_REQUEST,
	UNKNOWN_USER
};
#endif // !_CONFIGURATIONS_H_
