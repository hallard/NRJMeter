// **********************************************************************************
// ESP8266 NRJMeter WEB Server global Include file
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// For any explanation about teleinfo ou use , see my blog
// http://hallard.me/category/tinfo
//
// This program works with the Wifinfo board
// see schematic here https://github.com/hallard/teleinfo/tree/master/Wifinfo
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2015-06-14 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************
#ifndef NRJMETER_H
#define NRJMETER_H

// Include Arduino header
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <Hash.h>
#include <ESP8266mDNS.h>
#include <WifiUdp.h>
#include <EEPROM.h>
#include <Wire.h>
//#include <NeoPixelBus.h>
#include <LibTeleinfo.h>
#include <FS.h>
//#include <GDBStub.h>

extern "C" {
#include "user_interface.h"
}

#include "debug.h"
#include "flash_str.h"
#include "webserver.h"
#include "webclient.h"
#include "config.h"
#include "sensors.h"
#include "NeoPixelWrapper.h"
#include "EmonLib.h"


// OTA Prexif Name 
#if defined (ARDUINO_ESP8266_WEMOS_D1MINI) || defined (ARDUINO_ESP8266_WEMOS_D1)
  #define OTA_PREFIX_ID   'W'  
#elif defined (ARDUINO_ESP8266_NODEMCU)
  #define OTA_PREFIX_ID   'N'
#else
  #define OTA_PREFIX_ID   'E'
#endif

#define NRJMETER_VERSION_MAJOR 2
#define NRJMETER_VERSION_MINOR 0

// Don't use MQTT
#define USE_MQTT

// Maximum time when we fire a reset with no refresh (in sec)
#define WDT_RESET_TIME 30

// Maximum time when wifi is allowed to setup config (in sec)
#define MAX_WIFI_SEC	300

// Maximum number of simultaneous clients connected (WebSocket)
#define MAX_WS_CLIENT	5

#define BLINK_LED_MS   100 // ms blink
#define RGB_LED_PIN    0
//#define RGB_LED_COUNT  1
//#define RGB_LED_COUNT  17


#define RGB_ANIM_FADEINOUT	1
#define RGB_ANIM_CYCLON			2

// value for HSL color
// see http://www.workwithcolor.com/blue-color-hue-range-01.htm
#define COLOR_RED             0
#define COLOR_ORANGE         30
#define COLOR_ORANGE_YELLOW  45
#define COLOR_YELLOW         60
#define COLOR_YELLOW_GREEN   90
#define COLOR_GREEN         120
#define COLOR_GREEN_CYAN    165
#define COLOR_CYAN          180
#define COLOR_CYAN_BLUE     210
#define COLOR_BLUE          240
#define COLOR_BLUE_MAGENTA  275
#define COLOR_MAGENTA	      300
#define COLOR_PINK		      350



#define CLIENT_NONE			0
#define CLIENT_SENSORS	1
#define CLIENT_SYSTEM		2
#define CLIENT_CONFIG		3
#define CLIENT_SPIFFS		4
#define CLIENT_LOG			5
#define CLIENT_TINFO    6
#define CLIENT_LOGGER   7

// Light off the RGB LED
#ifndef RGB_LED_PIN
#define LedRGBOFF()  {}
#define LedRGBON(x)  {}
#else
//#define LedRGBOFF() { rgb_led.SetPixelColor(0,0,0,0); rgb_led.Show(); }
void LedRGBOFF(void);
void LedRGBON(uint16_t hue );
//void LedRGBON (uint8_t r, uint8_t g, uint8_t b);
#endif


// output  functions
/* Done un debug.h
#ifdef DEBUG_SERIAL
#define Debug(x)     	{ if (config.config & CFG_DEBUG) DEBUG_SERIAL.print(x);}
#define Debug2(x,y)  	{ if (config.config & CFG_DEBUG) DEBUG_SERIAL.print(x,y);}
#define Debugln(x)		{ if (config.config & CFG_DEBUG) DEBUG_SERIAL.println(x);}
#define Debugln2(x,y)	{ if (config.config & CFG_DEBUG) DEBUG_SERIAL.println(x,y);}
#define DebugF(x)   	{ if (config.config & CFG_DEBUG) DEBUG_SERIAL.print(F(x));}
#define DebuglnF(x) 	{ if (config.config & CFG_DEBUG) DEBUG_SERIAL.println(F(x));}
#define Debugf(...) 	{ if (config.config & CFG_DEBUG) DEBUG_SERIAL.printf(__VA_ARGS__);}
#else
#define Debug(x)       
#define Debug2(x,y)   
#define Debugln(x)    
#define Debugln2(x,y) 
#define DebugF(x)     
#define DebuglnF(x)   
#define Debugf(...)   
#endif
*/

// Exported variables/object instancied in main sketch
// ===================================================
extern DNSServer * pdnsServer ;
extern WiFiUDP OTA;
#ifdef RGB_LED_PIN
  extern long rgb_led_timer ;
#endif

extern unsigned long seconds;
extern unsigned long wifi_connect_time;
extern uint8_t heartbeat;
extern boolean task_emoncms ;
extern boolean task_jeedom ;
extern boolean task_domoticz ;
extern boolean task_sensors ;
extern boolean task_reconf ;
extern uint16_t power_samples;


// Exported function located in main sketch
// ===================================================
void dummy(void);
void handle_net(void);
#ifdef RGB_LED_PIN
void LedRGBSetup(void);
#else
#define LedRGBSetup(){}
#endif

#endif

