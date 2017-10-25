// **********************************************************************************
// ESP8266 NRJMeter MQTT Publisher
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// For any explanation about teleinfo ou use, see my blog
// http://hallard.me/category/tinfo
//
// This program works with the Wifinfo board
// see schematic here https://github.com/hallard/teleinfo/tree/master/Wifinfo
//
// Written by Sylvain REMY
//
// History : V1.00 2017-09-05 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#ifndef MQTT_h
#define MQTT_h


#include "NRJMeter.h"
#include "Debug.h"
#include <PubSubClient.h>

#define MQTT_MAX_PACKET_SIZE 128
#define MQTT_KEEPALIVE 15
#define MQTT_VERSION MQTT_VERSION_3_1_1
#define MQTT_VER_STRING "3.1.1" //Library version format conversion to human readable
#define MQTT_QOS_STRING "0 - Almost Once" //Only mode supported by the library
#define MQTT_RET_STRING "0" //Only mode supporte by the library

// Exported variables/object instancied in main sketch
// ===================================================
extern String outTopic;
extern String inTopic;

// Exported function located in main sketch
// ===================================================
void MQTT_setup();
void MQTT_close();
void handle_MQTT();
bool mqttPost(const char * topic, const char* message);

#endif