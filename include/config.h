// **********************************************************************************
// ESP8266 NRJMeter WEB Server configuration Include file
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
#ifndef __CONFIG_H__
#define __CONFIG_H__

// Include main project include file
#include "NRJMeter.h"
#include "MQTT.h"

#define CFG_SSID_SIZE 		  32
#define CFG_PSK_SIZE  		  64
#define CFG_DEFAULT_WMW     10
#define CFG_HOSTNAME_SIZE   16
#define CFG_USER_SIZE       16
#define CFG_PASS_SIZE       16
#define CFG_MQTT_TOPIC_SIZE 32

// ADCO Counter default value
#define CFG_DEFAULT_ADCO "012345678910"
#define CFG_ADCO_SIZE    12

#define CFG_EMON_HOST_SIZE 		32
#define CFG_EMON_APIKEY_SIZE 	32
#define CFG_EMON_URL_SIZE 		32
#define CFG_EMON_DEFAULT_PORT 80
#define CFG_EMON_DEFAULT_HOST "emoncms.org"
#define CFG_EMON_DEFAULT_URL  "/input/post.json"
#define CFG_EMON_DEFAULT_TOPIC  "emon"
#define CFG_EMON_DEFAULT_BMODE 0x01 // HTTP Only by default

// Config JeeDom
#define CFG_JDOM_HOST_SIZE    32
#define CFG_JDOM_APIKEY_SIZE  32
#define CFG_JDOM_URL_SIZE     64
#define CFG_JDOM_DEFAULT_PORT 80
#define CFG_JDOM_DEFAULT_HOST "jeedom.local"
#define CFG_JDOM_DEFAULT_URL  "/jeedom/plugins/teleinfo/core/php/jeeTeleinfo.php"
#define CFG_JDOM_DEFAULT_TOPIC  "sensor"
#define CFG_JDOM_DEFAULT_BMODE 0x01 // HTTP Only by default

// Config DomoticZ
#define CFG_DOMZ_HOST_SIZE    32
//#define CFG_DOMZ_APIKEY_SIZE  32
#define CFG_DOMZ_URL_SIZE     64
#define CFG_DOMZ_DEFAULT_PORT 80
#define CFG_DOMZ_DEFAULT_HOST "domoticz.local"
#define CFG_DOMZ_DEFAULT_URL  "/json.htm" //"/json.htm?type=command&param=udevice&nvalue=0"
#define CFG_DOMZ_DEFAULT_TOPIC "domoticz/in"
#define CFG_DOMZ_DEFAULT_BMODE 0x01 // HTTP Only by default

// Config MQTT
#define CFG_MQTT_HOST_SIZE       16
#define CFG_MQTT_USER_SIZE       16
#define CFG_MQTT_PASS_SIZE       16
#define CFG_MQTT_DEFAULT_HOST    "mqtt.local"
#define CFG_MQTT_DEFAULT_PORT    1883

// Counters default debounce = 10ms
#define CFG_COUNTER_DEFAULT_DELAY 10

#define CFG_SENSORS_TEMP_MIN_WARN (18)
#define CFG_SENSORS_TEMP_MAX_WARN (22)
#define CFG_SENSORS_HUM_MIN_WARN  (30)
#define CFG_SENSORS_HUM_MAX_WARN  (90)
#define CFG_SENSORS_PWR_MIN_WARN  (100)
#define CFG_SENSORS_PWR_MAX_WARN  (400)
#define CFG_SENSORS_DEFAULT_FREQ  (60) // every 60 seconds

// Port pour l'OTA
#define DEFAULT_OTA_PORT     8266

// RGB Led 
#define DEFAULT_LED_BRIGHTNESS  50                // 50%
#define DEFAULT_LED_HEARTBEAT   10                // 1s (1/10s)
#define DEFAULT_LED_TYPE        NeoPixelType_Rgb  // RGB
#define DEFAULT_LED_GPIO        0                 // GPIO 0
#define DEFAULT_LED_NUM         1                 // 1 RGB LED

// Bit definition for different configuration modes
#define CFG_LCD				  0x0001	// Enable display
#define CFG_DEBUG			  0x0002	// Enable serial debug
#define CFG_RGB_LED     0x0004  // Enable RGB LED
#define CFG_AP          0x0008  // Enable Wifi Access Point
#define CFG_SI7021      0x0010  // SI7021 seen
#define CFG_SHT10       0x0020  // SHT10 seen
#define CFG_MCP3421     0x0040  // MCP3421 seen
#define CFG_MCP4725     0x0080  // MCP4725 seen
#define CFG_HASOLED     0x0100  // I2C OLED seen
#define CFG_TINFO       0x0200  // Enable TInfo (any type: edf, dsm, ...)
#define CFG_DEMO_MODE   0x0400  // Demo Mode
#define CFG_LOGGER      0x0800  // Enable Debug to be printed to log.txt on SPIFFS
#define CFG_MQTT        0x1000  // Enable MQTT
#define CFG_STATIC      0x2000  // Enable Static IP
#define CFG_WIFI        0x4000  // Enable Wifi
#define CFG_BAD_CRC     0x8000  // Bad CRC when reading configuration

//Bit definition for different tinfo modes
#define CFG_TI_NONE      0x00 //No TInfo
#define CFG_TI_EDF       0x01 //EDF Teleinfo
#define CFG_TI_FILLER1   0x02 //Just in case we manage other Tinfo types (Deutch Smart Meter?)
#define CFG_TI_FILLER2   0x04 //Just in case we manage other Tinfo types
#define CFG_TI_FILLER3   0x08 //Just in case we manage other Tinfo types

//Bit definition for different broadcast modes
#define CFG_BMODE_NONE      0x00 //No broadcast
#define CFG_BMODE_HTTP      0x01 //HTTP broadcast
#define CFG_BMODE_MQTT      0x02 //MQTT broadcast
#define CFG_BMODE_FILLER2   0x04 //Just in case we manage other Tinfo types
#define CFG_BMODE_FILLER3   0x08 //Just in case we manage other Tinfo types

// Show config and help sections
#define CFG_HLP_ALL     0xFFFF
#define CFG_HLP_HELP    0x0000
#define CFG_HLP_SYS     0x0001
#define CFG_HLP_WIFI    0x0002
#define CFG_HLP_DATA    0x0004
#define CFG_HLP_SENSOR  0x0008
#define CFG_HLP_JEEDOM  0x0010
#define CFG_HLP_DOMZ    0x0020
#define CFG_HLP_COUNTER 0x0040
#define CFG_HLP_TINFO   0x0080
#define CFG_HLP_MQTT    0x0100

#define CFG_SERIAL_BUFFER_SIZE 128

#pragma pack(push)  // push current alignment to stack
#pragma pack(1)     // set alignment to 1 byte boundary

// Config for emoncms
// 256 Bytes
typedef struct 
{
  char  host[CFG_EMON_HOST_SIZE+1]; 		// FQDN 
  char  apikey[CFG_EMON_APIKEY_SIZE+1]; // Secret
  char  url[CFG_EMON_URL_SIZE+1];  			// Post URL
  char  topic[CFG_MQTT_TOPIC_SIZE+1];   // MQTT In Topic (33 Bytes)
  uint32_t port;   									    // Protocol port (HTTP/HTTPS)
  uint32_t node;     									  // optional node
  uint32_t freq;                        // refresh rate
  uint8_t bmode;                        // Bit field register (1 Byte) - Broadcast mode (HTTP, MQTT)
  uint8_t filler[111];									// in case adding data in config avoiding loosing current conf by bad crc*/
} _emoncms;

// Config for sensors / Counter
// 256 Bytes
typedef struct 
{
  int16_t temp_min_warn;  // Temperature offset min warning
  int16_t temp_max_warn;  // Temperature offset max warning
  int16_t hum_min_warn;   // humidity offset min warning 
  int16_t hum_max_warn;   // humidity offset max warning
  int16_t pwr_min_warn;   // power offset min warning 
  int16_t pwr_max_warn;   // pwer offset max warning
  uint32_t freq;          // refresh rate
  int8_t  en_si7021;      // enable SI7021 sensor
  int8_t  en_sht10;       // enable SH10 sensor
  int8_t  en_mcp3421;     // enable ADC MCP3421 sensor
  uint8_t filler01;       // in case adding data in config avoiding loosing current conf by bad crc*/
  uint32_t counter1;      // Counter 1
  uint32_t counter2;      // Counter 2
  uint8_t filler[228];    // in case adding data in config avoiding loosing current conf by bad crc*/
} _sensors;

// Config for jeedom
// 256 Bytes
typedef struct 
{
  char  host[CFG_JDOM_HOST_SIZE+1];     // FQDN 
  char  apikey[CFG_JDOM_APIKEY_SIZE+1]; // Secret
  char  url[CFG_JDOM_URL_SIZE+1];       // Post URL
  char  topic[CFG_MQTT_TOPIC_SIZE+1];   // MQTT In Topic (33 Bytes)
  char  adco[CFG_ADCO_SIZE+1];          // Identifiant compteur
  uint16_t port;                        // Protocol port (HTTP/HTTPS)
  uint32_t freq;                        // refresh rate
  uint8_t bmode;                        // Bit field register (1 Byte) - Broadcast mode (HTTP, MQTT)
  uint8_t filler[72];                  // in case adding data in config avoiding loosing current conf by bad crc*/
} _jeedom;

// Config for domoticz
// 256 Bytes
typedef struct 
{
  char  host[CFG_DOMZ_HOST_SIZE+1];     // FQDN 
  char  user[CFG_USER_SIZE+1];          // user name
  char  pass[CFG_PASS_SIZE+1];          // password
  char  url[CFG_DOMZ_URL_SIZE+1];       // Post URL
  char  topic[CFG_MQTT_TOPIC_SIZE+1];   // MQTT In Topic (33 Bytes)
  uint32_t freq;                        // refresh rate
  uint16_t idx_upt;                     // Index device domoticz Uptime (2 Byte)
  uint16_t idx_rst;                     // Index device domoticz Reset Cause (2 Byte)
  uint16_t idx_mcp3421;                 // Index device domoticz Electric (2 Byte)
  uint16_t idx_si7021;                  // Index device domoticz Temp + Humidity (2 Byte)
  uint16_t idx_sht10;                   // Index device domoticz Temp + Humidity (2 Byte)
  uint16_t idx_txt;                     // Index device domoticz Text (2 Byte)
  uint16_t idx_p1sm;                    // Index device domoticz P1 Smart Meter (2 Bytes)
  uint16_t idx_crt;                     // Index device domoticz Current (2 Bytes)
  uint16_t idx_elec;                    // Index device domoticz Eletric (2 Byte)
  uint16_t idx_kwh;                     // Index device domoticz Kwh (2 Byte)
  uint16_t idx_pct;                     // Index device domoticz Percentage (2 Byte)
  uint16_t port;                        // Protocol port (HTTP/HTTPS)
  uint8_t bmode;                        // Bit field register (1 Byte) - Broadcast mode (HTTP, MQTT)
  uint8_t filler[62];                   // in case adding data in config avoiding loosing current conf by bad crc*/
} _domoticz;


// Config for MQTT
// 256 Bytes
typedef struct 
{
  char  host[CFG_MQTT_HOST_SIZE+1];     // Broker Hostname (17 Bytes)
  uint16_t port;                  // MQTT port  (2 Bytes)
  char  usr[CFG_MQTT_USER_SIZE+1];     // HTTP Authentication user (17 Bytes)
  char  pwd[CFG_MQTT_PASS_SIZE+1];     // HTTP Authentication user (17 Bytes)
  uint8_t filler[203];                 // in case adding data in config avoiding loosing current conf by bad crc*/
} _mqtt;


// Config for counters
// 32 Bytes
typedef struct 
{
  unsigned long value1;   // Counter 1 value
  unsigned long value2;   // Counter 2 value
  uint16_t delay1;        // Counter 1 delay (debounce time) in ms
  uint16_t delay2;        // Counter 2 delay (debounce time) in ms
  uint8_t  gpio1;         // Counter 1 gpio pin number
  uint8_t  gpio2;         // Counter 2 gpio pin number
  uint8_t filler[18];     // in case adding data in config avoiding loosing current conf by bad crc*/
} _counter;

// Config for tinfo
// 16 Bytes
typedef struct
{
  uint8_t type;                   // TInfo type (EDF, ... maybe others to come) eg: Deutch Smart Meter (https://www.wijhebbenzon.nl/media/kunena/attachments/3055/DSMRv5.0FinalP1.pdf + https://www.domoticz.com/forum/viewtopic.php?t=4970)
  uint8_t filler[15];             // in case adding data in config avoiding loosing current conf by bad crc*/
} _tinfo;

// Config saved into eeprom
// 4096 bytes total including CRC
typedef struct 
{
  char  ssid[CFG_SSID_SIZE+1]; 		 // SSID (33 Bytes)
  char  psk[CFG_PSK_SIZE+1]; 		   // Pre shared key (65 Bytes) 
  char  host[CFG_HOSTNAME_SIZE+1]; // Hostname (17 Bytes)
  char  ap_psk[CFG_PSK_SIZE+1];    // Access Point Pre shared key (65 Bytes)
  char  ap_ssid[CFG_SSID_SIZE+1];  // Access Point SSID name (33 Bytes)
  char  http_usr[CFG_USER_SIZE+1]; // HTTP Authentication user (17 Bytes)
  char  http_pwd[CFG_PASS_SIZE+1]; // HTTP Authentication password (17 Bytes)
  char  ota_auth[CFG_PSK_SIZE+1];  // OTA Authentication password (65 Bytes)
  uint32_t config;           		   // Bit field register  (4 Bytes)
  uint16_t ota_port;         		   // OTA port  (2 Bytes)
  uint8_t led_bright;              // RGB Led brigthness (1 Bytes)
  uint8_t led_hb;                  // RGB Led HeartBeat (1 Bytes)
  uint8_t led_type;                // RGB Led type (1 Bytes)
  uint8_t led_num;                 // # of RGB LED (1 Bytes)
  uint8_t led_gpio;                // GPIO driving RGBLED (1 Bytes)
  uint32_t ip;                     // Static Wifi IP Address (4 Bytes)
  uint32_t mask;                   // Static Wifi NetMask (4 Bytes)
  uint32_t gw;                     // Static Wifi Gateway Address (4 Bytes)
  uint32_t dns;                    // Static Wifi DNS server Address (4 Bytes)
  uint8_t  led_panel;              // LED Panel brigthness (1 Bytes)
  uint8_t wmw;                     // Wifi connect max wait in seconds (1 Bytes)
  uint8_t  filler[512];      		   // in case adding data in config avoiding loosing current conf by bad crc (41 Bytes)
  _mqtt mqtt;                      // MQTT configuration (256 Bytes)
  _emoncms emoncms;                // Emoncms configuration (256 Bytes)
  _sensors sensors;                // Sensors configuration (256 Bytes)
  _jeedom  jeedom;                 // JeeDom configuration (256 Bytes)
  _domoticz domz;                  // Domoticz configuration (256 Bytes)
  _counter counter;                // Counter configuration (32 Bytes)
  _tinfo tinfo;                    // TInfo configuration (16 Bytes)
  uint8_t  filler1[1913];          // Another filler in case we need more
  uint16_t crc;                    // CRC (2 Bytes)
} _Config;


// Exported variables/object instancied in main sketch
// ===================================================
extern _Config config;

#pragma pack(pop)
 
// Declared exported function from route.cpp
// ===================================================
bool readConfig(bool clear_on_error=true, uint32_t clientid = 0);
bool saveConfig(uint32_t clientid = 0);
void showConfig(uint16_t section = CFG_HLP_ALL, uint32_t clientid = 0 ) ;
void showHelp(uint16_t section = CFG_HLP_ALL, uint32_t clientid = 0);
void resetConfig(uint32_t clientid = 0);
void resetBoard(uint32_t clientid = 0);
void handle_serial(char * line = NULL, uint32_t clientid = 0 );
void execCmd(char * line, uint32_t clientid = 0);



#endif 

