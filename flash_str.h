// **********************************************************************************
// ESP8266 NRJMeter Strings stored in flash for size Optimization
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

#ifndef FLASH_STR_H
#define FLASH_STR_H

// Include main project include file
#include "NRJMeter.h"

// Optimize string space in flash, avoid duplication
extern const char FP_RESTART[];
extern const char FP_NL[];

// Used in various place
extern const char FP_TEMPERATURE[];
extern const char FP_HUMIDITY[];
extern const char FP_SEEN[];

extern const char FP_SSID[];
extern const char FP_RSSI[];
extern const char FP_ENCRYPTION[];
extern const char FP_CHANNEL[];

// Used in JSON array for bootstrap table
extern const char FP_NA[] ;
extern const char FP_VA[] ;

// Web Interface/serial configuration Form field/command names
extern const char CFG_SAVE[]		;
extern const char CFG_SSID[] 		;
extern const char CFG_PSK[] 		;
extern const char CFG_HOST[] 		;
extern const char CFG_IP[] 			;
extern const char CFG_GATEWAY[]	;
extern const char CFG_MASK[]		;
extern const char CFG_DNS[]			;
extern const char CFG_EEP_DUMP[];


extern const char CFG_AP_PSK[] 		 ;
extern const char CFG_AP_SSID[] 	 ;
extern const char CFG_OTA_AUTH[] 	 ;
extern const char CFG_OTA_PORT[] 	 ;

extern const char CFG_EMON_HOST[]  ;
extern const char CFG_EMON_PORT[]  ;
extern const char CFG_EMON_URL[] 	 ;
extern const char CFG_EMON_KEY[] 	 ;
extern const char CFG_EMON_NODE[]  ;
extern const char CFG_EMON_FREQ[]  ;

extern const char CFG_JDOM_HOST[] ;
extern const char CFG_JDOM_PORT[] ;
extern const char CFG_JDOM_URL[]  ;
extern const char CFG_JDOM_KEY[]  ;
extern const char CFG_JDOM_ADCO[] ;
extern const char CFG_JDOM_FREQ[] ;

extern const char CFG_DOMZ_HOST[]  ; 
extern const char CFG_DOMZ_PORT[]  ;
extern const char CFG_DOMZ_URL[]   ;
extern const char CFG_DOMZ_USER[]  ;
extern const char CFG_DOMZ_PASS[]  ;
extern const char CFG_DOMZ_INDEX[] ;
extern const char CFG_DOMZ_FREQ[]  ;

extern const char CFG_COUNT1_GPIO[]  ;
extern const char CFG_COUNT2_GPIO[]  ;
extern const char CFG_COUNT1_DELAY[] ;
extern const char CFG_COUNT2_DELAY[] ;
extern const char CFG_COUNT1_VALUE[] ;
extern const char CFG_COUNT2_VALUE[] ;

extern const char CFG_SENS_SI7021[] 	;
extern const char CFG_SENS_SHT10[] 		;
extern const char CFG_SENS_FREQ[] 		;
extern const char CFG_SENS_HUM_LED[] 	;
extern const char CFG_SENS_TEMP_LED[] ;

extern const char CFG_LED_BRIGHTNESS[] ;
extern const char CFG_LED_HEARTBEAT[]  ;
extern const char CFG_LED_GPIO[]			 ;
extern const char CFG_LED_NUM[] 			 ;
extern const char CFG_LED_TYPE[]			 ;


extern const char CFG_CFG_AP[] 			;
extern const char CFG_CFG_WIFI[] 		;
extern const char CFG_CFG_RGBLED[] 	;
extern const char CFG_CFG_DEBUG[] 	;
extern const char CFG_CFG_OLED[] 		;
extern const char CFG_CFG_STATIC[]	;

extern const char HELP_HELP[];
extern const char HELP_SYS[];
extern const char HELP_WIFI[];
extern const char HELP_DATA[];
extern const char HELP_JEEDOM[];
extern const char HELP_DOMZ[];
extern const char HELP_COUNTER[];
extern const char HELP_SENSOR[];


#endif 