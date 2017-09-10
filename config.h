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

#define CFG_SSID_SIZE 		32
#define CFG_PSK_SIZE  		64
#define CFG_HOSTNAME_SIZE 16
#define CFG_USER_SIZE     16
#define CFG_PASS_SIZE     16

// ADCO Counter default value
#define CFG_DEFAULT_ADCO "012345678910"
#define CFG_ADCO_SIZE    12

#define CFG_EMON_HOST_SIZE 		32
#define CFG_EMON_APIKEY_SIZE 	32
#define CFG_EMON_URL_SIZE 		32
#define CFG_EMON_DEFAULT_PORT 80
#define CFG_EMON_DEFAULT_HOST "emoncms.org"
#define CFG_EMON_DEFAULT_URL  "/input/post.json"

// Config JeeDom
#define CFG_JDOM_HOST_SIZE    32
#define CFG_JDOM_APIKEY_SIZE  32
#define CFG_JDOM_URL_SIZE     64
#define CFG_JDOM_DEFAULT_PORT 80
#define CFG_JDOM_DEFAULT_HOST "jeedom.local"
#define CFG_JDOM_DEFAULT_URL  "/jeedom/plugins/teleinfo/core/php/jeeTeleinfo.php"

// Config DomoticZ
#define CFG_DOMZ_HOST_SIZE    32
//#define CFG_DOMZ_APIKEY_SIZE  32
#define CFG_DOMZ_URL_SIZE     64
#define CFG_DOMZ_DEFAULT_PORT 80
#define CFG_DOMZ_DEFAULT_HOST "domoticz.local"
#define CFG_DOMZ_DEFAULT_URL  "/json.htm?type=command&param=udevice&nvalue=0"

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
#define CFG_STATIC      0x2000  // Enable Static IP
#define CFG_WIFI        0x4000  // Enable Wifi
#define CFG_BAD_CRC     0x8000  // Bad CRC when reading configuration

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

#define CFG_SERIAL_BUFFER_SIZE 128

#pragma pack(push)  // push current alignment to stack
#pragma pack(1)     // set alignment to 1 byte boundary

// Config for emoncms
// 128 Bytes
typedef struct 
{
  char  host[CFG_EMON_HOST_SIZE+1]; 		// FQDN 
  char  apikey[CFG_EMON_APIKEY_SIZE+1]; // Secret
  char  url[CFG_EMON_URL_SIZE+1];  			// Post URL
  uint32_t port;   									    // Protocol port (HTTP/HTTPS)
  uint32_t node;     									  // optional node
  uint32_t freq;                        // refresh rate
  uint8_t filler[17];									  // in case adding data in config avoiding loosing current conf by bad crc*/
} _emoncms;

// Config for sensors / Counter
// 128 Bytes
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
  uint8_t filler[100];    // in case adding data in config avoiding loosing current conf by bad crc*/
} _sensors;

// Config for jeedom
// 160 Bytes
typedef struct 
{
  char  host[CFG_JDOM_HOST_SIZE+1];     // FQDN 
  char  apikey[CFG_JDOM_APIKEY_SIZE+1]; // Secret
  char  url[CFG_JDOM_URL_SIZE+1];       // Post URL
  char  adco[CFG_ADCO_SIZE+1];          // Identifiant compteur
  uint16_t port;                        // Protocol port (HTTP/HTTPS)
  uint32_t freq;                        // refresh rate
  uint8_t filler[10];                   // in case adding data in config avoiding loosing current conf by bad crc*/
} _jeedom;

// Config for domtoticz
// 160 Bytes
typedef struct 
{
  char  host[CFG_DOMZ_HOST_SIZE+1];     // FQDN 
  char  user[CFG_USER_SIZE+1];          // user name
  char  pass[CFG_PASS_SIZE+1];          // password
  char  url[CFG_DOMZ_URL_SIZE+1];       // Post URL
  uint32_t freq;                        // refresh rate
  uint16_t index;                       // DomoticZ device index
  uint16_t port;                        // Protocol port (HTTP/HTTPS)
  uint8_t filler[20];                   // in case adding data in config avoiding loosing current conf by bad crc*/
} _domoticz;


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
  uint8_t filler[18];                   // in case adding data in config avoiding loosing current conf by bad crc*/
} _counter;

// Config saved into eeprom
// 2048 bytes total including CRC
typedef struct 
{
  char  ssid[CFG_SSID_SIZE+1]; 		 // SSID     
  char  psk[CFG_PSK_SIZE+1]; 		   // Pre shared key
  char  host[CFG_HOSTNAME_SIZE+1]; // Hostname 
  char  ap_psk[CFG_PSK_SIZE+1];    // Access Point Pre shared key
  char  ap_ssid[CFG_SSID_SIZE+1];  // Access Point SSID name
  char  ota_auth[CFG_PSK_SIZE+1];  // OTA Authentication password
  uint32_t config;           		   // Bit field register 
  uint16_t ota_port;         		   // OTA port 
  uint8_t led_bright;              // RGB Led brigthness
  uint8_t led_hb;                  // RGB Led HeartBeat
  uint8_t led_type;                // RGB Led type
  uint8_t led_num;                 // # of RGB LED
  uint8_t led_gpio;                // GPIO driving RGBLED
  uint32_t ip;                     // Static Wifi IP Address
  uint32_t mask;                   // Static Wifi NetMask
  uint32_t gw;                     // Static Wifi Gateway Address
  uint32_t dns;                    // Static Wifi DNS server Address
  uint8_t  led_panel;              // LED Panel brigthness
  uint8_t  filler[76];      		   // in case adding data in config avoiding loosing current conf by bad crc
  _emoncms emoncms;                // Emoncms configuration
  _sensors sensors;                // Sensors configuration
  _jeedom  jeedom;                 // JeeDom configuration
  _domoticz domz;                  // Domoticz configuration
  _counter counter;                // Counter configuration
  uint8_t  filler1[1056];          // Another filler in case we need more
  uint16_t crc;
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

