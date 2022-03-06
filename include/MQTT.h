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
// Written by Sylvain REMY / Charles-Henri Hallard
//
// History : V1.00 2017-09-05 - First release
//           V2.00 2019-06-02 - Use AsyncMQTT library
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#ifndef MQTT_h
#define MQTT_h

#include "NRJMeter.h"

#define MQTT_MAX_TOPIC_LEN 1024
#define MQTT_QOS 		0  		
#define MQTT_RETAIN false 

// Exported variables/object instancied in main sketch
// ===================================================
extern String outTopic;
extern String inTopic;
extern AsyncMqttClient mqttClient;

// Exported function located in main sketch
// ===================================================
void MQTT_setup();
void MQTT_close();
void handle_MQTT();
bool mqttPost(const char * topic, const char* message);

#endif