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

#include "flash_str.h"

// Optimize string space in flash, avoid duplication
const char FP_RESTART[] 		PROGMEM = "OK, restarting\r\n";
const char FP_NL[] 					PROGMEM = "\n";

// Used in various place
const char FP_TEMPERATURE[] PROGMEM = "temperature";
const char FP_HUMIDITY[]    PROGMEM = "humidity";
const char FP_POWER[]       PROGMEM = "power";
const char FP_SEEN[]        PROGMEM = "seen";

const char FP_SSID[]        PROGMEM = "ssid";
const char FP_RSSI[]        PROGMEM = "rssi";
const char FP_ENCRYPTION[]  PROGMEM = "enc";
const char FP_CHANNEL[]     PROGMEM = "chan";
const char FP_STATUS[]      PROGMEM = "status";

// Used in JSON array for bootstrap table
const char FP_NA[] PROGMEM = "na";
const char FP_VA[] PROGMEM = "va";
const char FP_AC[] PROGMEM = "ac";
const char FP_CK[] PROGMEM = "ck";
const char FP_FL[] PROGMEM = "fl";
const char FP_ER[] PROGMEM = "er";
const char FP_EV[] PROGMEM = "ev";

// Used for teleinformation
const char PAPP[]  PROGMEM = "PAPP";
const char IINST[] PROGMEM = "IINST";

// Web Interface/serial configuration Form field/command names
const char CFG_SAVE[] 		PROGMEM = "save";
const char CFG_SSID[] 		PROGMEM = "ssid";
const char CFG_PSK[] 			PROGMEM = "psk";
const char CFG_WMW[]      PROGMEM = "wmw";
const char CFG_HOST[] 		PROGMEM = "host";
const char CFG_IP[] 			PROGMEM = "ip";
const char CFG_GATEWAY[]	PROGMEM = "gw";
const char CFG_MASK[]			PROGMEM = "mask";
const char CFG_DNS[]			PROGMEM = "dns";
const char CFG_AP_PSK[] 	PROGMEM = "appsk";
const char CFG_AP_SSID[] 	PROGMEM = "apssid";
const char CFG_HTTP_USR[] PROGMEM = "http_usr";
const char CFG_HTTP_PWD[] PROGMEM = "http_pwd";
const char CFG_OTA_AUTH[] PROGMEM = "ota_auth";
const char CFG_OTA_PORT[] PROGMEM = "ota_port";
const char CFG_EEP_DUMP[]	PROGMEM = "eep_dump";

const char CFG_MQTT_HOST[] PROGMEM = "mqtt_host";
const char CFG_MQTT_PORT[] PROGMEM = "mqtt_port";
const char CFG_MQTT_USR[]  PROGMEM = "mqtt_usr";
const char CFG_MQTT_PWD[]  PROGMEM = "mqtt_pwd";
const char CFG_MQTT_INT[]  PROGMEM = "mqtt_in";
const char CFG_MQTT_OUT[]  PROGMEM = "mqtt_out";
const char CFG_MQTT_QOS[]  PROGMEM = "mqtt_qos";
const char CFG_MQTT_RET[]  PROGMEM = "mqtt_ret";
const char CFG_MQTT_VER[]  PROGMEM = "mqtt_ver";

const char CFG_EMON_HOST[] 	PROGMEM = "emon_host";
const char CFG_EMON_PORT[] 	PROGMEM = "emon_port";
const char CFG_EMON_URL[] 	PROGMEM = "emon_url";
const char CFG_EMON_TOPIC[]   PROGMEM = "emon_topic";
const char CFG_EMON_KEY[] 	PROGMEM = "emon_apikey";
const char CFG_EMON_NODE[] 	PROGMEM = "emon_node";
const char CFG_EMON_FREQ[] 	PROGMEM = "emon_freq";
const char CFG_EMON_HTTP[]   PROGMEM = "emon_http";
const char CFG_EMON_MQTT[]   PROGMEM = "emon_mqtt";

const char CFG_JDOM_HOST[] PROGMEM = "jdom_host";
const char CFG_JDOM_PORT[] PROGMEM = "jdom_port";
const char CFG_JDOM_URL[]  PROGMEM = "jdom_url";
const char CFG_JDOM_TOPIC[]  PROGMEM = "jdom_topic";
const char CFG_JDOM_KEY[]  PROGMEM = "jdom_apikey";
const char CFG_JDOM_ADCO[] PROGMEM = "jdom_adco";
const char CFG_JDOM_FREQ[] PROGMEM = "jdom_freq";
const char CFG_JDOM_HTTP[] PROGMEM = "jdom_http";
const char CFG_JDOM_MQTT[] PROGMEM = "jdom_mqtt";

const char CFG_DOMZ_HOST[] PROGMEM  = "domz_host";
const char CFG_DOMZ_PORT[] PROGMEM  = "domz_port";
const char CFG_DOMZ_URL[]  PROGMEM  = "domz_url";
const char CFG_DOMZ_TOPIC[]  PROGMEM  = "domz_topic";
const char CFG_DOMZ_USER[]  PROGMEM = "domz_user";
const char CFG_DOMZ_PASS[]  PROGMEM = "domz_pass";
const char CFG_DOMZ_IDX_UPT[]  PROGMEM = "domz_idx_upt";
const char CFG_DOMZ_IDX_RST[]  PROGMEM = "domz_idx_rst";
const char CFG_DOMZ_IDX_MCP3421[]  PROGMEM = "domz_idx_mcp3421";
const char CFG_DOMZ_IDX_SI7021[]  PROGMEM = "domz_idx_si7021";
const char CFG_DOMZ_IDX_SHT10[]  PROGMEM = "domz_idx_sht10";
const char CFG_DOMZ_IDX_TXT[]  PROGMEM = "domz_idx_txt";
const char CFG_DOMZ_IDX_P1SM[] PROGMEM = "domz_idx_p1sm";
const char CFG_DOMZ_IDX_CRT[]  PROGMEM = "domz_idx_crt";
const char CFG_DOMZ_IDX_ELEC[] PROGMEM = "domz_idx_elec";
const char CFG_DOMZ_IDX_KWH[]  PROGMEM = "domz_idx_kwh";
const char CFG_DOMZ_IDX_PCT[]  PROGMEM = "domz_idx_pct";
const char CFG_DOMZ_FREQ[]     PROGMEM  = "domz_freq";
const char CFG_DOMZ_HTTP[]     PROGMEM  = "domz_http";
const char CFG_DOMZ_MQTT[]     PROGMEM  = "domz_mqtt";

const char CFG_COUNT1_GPIO[]  PROGMEM = "cnt_io_1";
const char CFG_COUNT2_GPIO[]  PROGMEM = "cnt_io_2";
const char CFG_COUNT1_DELAY[] PROGMEM = "cnt_dly_1";
const char CFG_COUNT2_DELAY[] PROGMEM = "cnt_dly_2";
const char CFG_COUNT1_VALUE[] PROGMEM = "cnt_val_1";
const char CFG_COUNT2_VALUE[] PROGMEM = "cnt_val_2";

const char CFG_TINFO_EDF[]       PROGMEM = "tinfo_edf";
const char CFG_TINFO_PWR_LED[]   PROGMEM = "tinfo_pwr_led";
const char CFG_TINFO_NRJ_LED[]   PROGMEM = "tinfo_nrj_led";

const char CFG_SENS_SI7021[] 		PROGMEM = "sens_si7021";
const char CFG_SENS_SHT10[] 		PROGMEM = "sens_sht10";
const char CFG_SENS_MCP3421[] 	PROGMEM = "sens_mcp3421";
const char CFG_SENS_FREQ[] 		  PROGMEM = "sens_freq";
const char CFG_SENS_HUM_LED[] 	PROGMEM = "sens_hum_led";
const char CFG_SENS_TEMP_LED[] 	PROGMEM = "sens_temp_led";
const char CFG_SENS_PWR_LED[] 	PROGMEM = "sens_pwr_led";

const char CFG_LED_BRIGHTNESS[] PROGMEM = "cfg_led_bright";
const char CFG_LED_HEARTBEAT[] 	PROGMEM = "cfg_led_hb";
const char CFG_LED_GPIO[] 			PROGMEM = "cfg_led_gpio";
const char CFG_LED_NUM[] 				PROGMEM = "cfg_led_num";
const char CFG_LED_TYPE[] 			PROGMEM = "cfg_led_type";

const char CFG_CFG_AP[] 		PROGMEM = "cfg_ap";
const char CFG_CFG_WIFI[] 	PROGMEM = "cfg_wifi";
const char CFG_CFG_RGBLED[] PROGMEM = "cfg_rgb";
const char CFG_CFG_DEBUG[] 	PROGMEM = "cfg_debug";
const char CFG_CFG_LOGGER[] PROGMEM = "cfg_logger";
const char CFG_CFG_OLED[] 	PROGMEM = "cfg_oled";
const char CFG_CFG_TINFO[]  PROGMEM = "cfg_tinfo";
const char CFG_CFG_DEMO[]   PROGMEM = "cfg_demo";
const char CFG_CFG_STATIC[]	PROGMEM = "cfg_static";
const char CFG_CFG_MQTT[]   PROGMEM = "cfg_mqtt";
const char CFG_CFG_SHT10[]	PROGMEM = "has_sht10";
const char CFG_CFG_SI7021[]	PROGMEM = "has_si7021";
const char CFG_CFG_MCP3421[]PROGMEM = "has_mcp3421";
const char CFG_CFG_HASOLED[]PROGMEM = "has_oled";


const char HELP_HELP[] PROGMEM = "\r\n===== Help\r\n"
	    "show help [sys|wifi|mqtt|data|tinfo|sens|jdom|domz|cnt]   : show full help or section\r\n"
	 		"show config [sys|wifi|mqtt|data|tinfo|sens|jdom|domz|cnt] : show full config or section\r\n";

const char HELP_SYS[] PROGMEM  = "\r\n===== Config\r\n" 
	    "cfg ap  on|off    : Enable/Disable to be access point mode on or off\r\n"
	    "cfg wifi on|off   : Enable/Disable Wifi on or off\r\n"
      "cfg mqtt on|off   : Enable/Disable MQTT on or off\r\n"
	    "cfg rgb on|off    : Enable/Disable rgb led\r\n"
	    "cfg debug on|off  : Enable/Disable debug\r\n"
      "cfg demo on|off   : Enable/Disable demo data mode\r\n"
	    "cfg oled on|off   : Enable/Disable display\r\n"

	    "cfg led bright p  : Set RGB led brightness to p percent (0 to 100)\r\n"
	    "cfg led hb n      : Set RGB led heartbeat to n sec (0 to 60)\r\n"
	    "cfg led num n     : Set n # of RGB led\r\n"
	    "cfg led gpio n    : Set gpio port used to drive RGB LED\r\n"
	    "cfg led type x    : Set RGB LED type (0=None, 1=RGB, 2=GRB, 3=RGBW, 4=GRBW\r\n"

      "http usr login    : Set HTTP Authentication User to login\r\n" 
      "http pwd password : Set HTTP Authentication Password to password\r\n" 
	    "ota auth authkey  : Set OTA Authentication to authkey\r\n" 
	    "ota port          : Set OTA TCP Port\r\n" 
	    "save config       : save current configuration\r\n"
	    "eep dump <n>      : dump current EEP data with n bytes per line\r\n"

	    "-- Examples\r\n" 
	    "  cfg rgb off       => disable RGB led\r\n" 
	    "  cfg wifi on       => enable Wifi\r\n" 
	    "  cfg led bright 50 => Set RGB led brightness to 50%\r\n"
	    "  cfg led type 2    => Set RGB led type to GRB\r\n"
	    "  cfg led num 16    => Set 16 RGB led type\r\n"
	    "  cfg led gpio 0    => Set GPIO0 port to drive RGB led\r\n";
  
const char HELP_WIFI[] PROGMEM  = "\r\n===== Wifi\r\n" 
	    "ssid name         : Set SSID name\r\n"
	    "psk pass          : Set Key/Password\r\n"
      "wmw 10            : Set Wifi Max Wait Connect Time in seconds\r\n"
	    "host name         : Set device Hostname\r\n" 
	    "apssid name       : Set device Access Point SSID name\r\n" 
	    "appsk psk         : Set device Access Point Key/password\r\n" 
	    "cfg static on|off : Enable Wifi Static mode (set also folowing values)\r\n"
	    "ip w.x.y.z        : Set IP address\r\n" 
	    "mask w.x.y.z      : Set network Mask\r\n" 
	    "gw w.x.y.z        : Set Gateway IP\r\n" 
	    "dns w.x.y.z       : Set DNS server\r\n" 
	    "-- Examples\r\n" 
	    "  ssid CH2I-HOTSPOT => Set Wifi to connect to CH2I-HOTSPOT\r\n" 
	    "  psk ch2i_passw    => connect to hotspot with ch2i_passw key\r\n" 
	    "  cfg static on     => Set Wifi static address need ip/mask/gw/dns also set\r\n"
	    "  dns 192.168.1.254 => Set Wifi static DNS address need ip/mask/gw also set\r\n";

const char HELP_MQTT[] PROGMEM  = "\r\n===== MQTT\r\n" 
      "mqtt host name         : Set Broker host name\r\n"
      "mqtt port tcp          : Set Broker port\r\n"
      "mqtt usr user          : Set Broker User\r\n"
      "mqtt pwd password      : Set Broker Password\r\n"
      "-- Examples\r\n" 
      "  mqtt host broker.local => Set Broker to connect to broker.local\r\n" 
      "  mqtt port 1883         => Set Port to connect to broker on port 1883\r\n" 
      "  mqtt usr admin         => Set User to connect to broker\r\n"
      "  mqtt pwd password      => Set Password to connect to broker\r\n";
      
const char HELP_DATA[] PROGMEM  = "\r\n===== Data Server\r\n" 
	    "emon host name     : Set server name/IP\r\n" 
	    "emon port tcp      : Set server port [80|443]\r\n" 
	    "emon url query     : Set post URL\r\n" 
      "emon topic name    : Set MQTT In Topic\r\n" 
	    "emon apikey key    : Set apikey\r\n" 
	    "emon freq f        : Set sending interval to f sec\r\n"
      "emon http on|off   : Set sending data using HTTP\r\n"
      "emon mqtt on|off   : Set sending data using MQTT\r\n"
	    "emon node n        : Set emoncms node number n\r\n" 
	    "-- Examples\r\n" 
	    "  emon host data.ch2i.eu    => Set data server \r\n" 
	    "  emon freq 300             => Send data to server every 5 minutes\r\n" 
	    "  emon url /input/post.json => set server url\r\n" ;

const char HELP_JEEDOM[] PROGMEM  = "\r\n===== Jeedom Server\r\n" 
	    "jdom host name     : Set server name/IP\r\n" 
	    "jdom port tcp      : Set server port [80|443]\r\n" 
	    "jdom url query     : Set post URL\r\n" 
      "jdom topic name    : Set MQTT In Topic\r\n" 
	    "jdom apikey key    : Set apikey\r\n" 
	    "jdom freq f        : Set sending interval to f sec\r\n" 
      "jdom http on|off   : Set sending data using HTTP\r\n"
      "jdom mqtt on|off   : Set sending data using MQTT\r\n"
	    "jdom adco serial   : Set ADCO serial number\r\n" 
	    "-- Examples\r\n" 
	    "  jdom host jeedom.local    => Set data server \r\n" 
	    "  jdom freq 300             => Send data to server every 5 minutes\r\n" 
	    "  jdom url /Teleinfo.php    => set server url\r\n" ;

const char HELP_DOMZ[] PROGMEM  = 	"\r\n===== DomoticZ Server\r\n" 
	    "domz host name           : Set server name/IP\r\n" 
	    "domz port tcp            : Set server port [80|443]\r\n" 
	    "domz url query           : Set post URL\r\n" 
      "domz topic name          : Set MQTT In Topic\r\n" 
	    "domz user username       : Set server username\r\n" 
	    "domz pass password       : Set server password\r\n" 
	    "domz freq f              : Set sending interval to f sec\r\n" 
      "domz http on|off         : Set sending data using HTTP\r\n"
      "domz mqtt on|off         : Set sending data using MQTT\r\n"
	    "domz idx mcp3421 number  : Set device Index number\r\n" 
      "domz idx si7021 number   : Set device Index number\r\n" 
      "domz idx sht10 number    : Set device Index number\r\n" 
      "domz idx txt number      : Set device Index number\r\n" 
      "domz idx p1sm number     : Set device Index number\r\n" 
      "domz idx crt number      : Set device Index number\r\n" 
      "domz idx elec number     : Set device Index number\r\n" 
      "domz idx kwh number      : Set device Index number\r\n" 
      "domz idx pct number      : Set device Index number\r\n" 
	    "-- Examples\r\n" 
	    "  domz host domoticz   => Set data server \r\n" 
	    "  domz freq 300        => Send data to server every 5 minutes\r\n" 
	    "  domz url /json.htm   => set server url\r\n" 
	    "  domz user foo        => set server username to foo\r\n" ;

const char HELP_COUNTER[] PROGMEM  = "\r\n===== Counter Index (1 or 2 only)\r\n" 
	    "cnt io n gpio      : Set counter n gpio number\r\n" 
	    "cnt dly n delay    : Set counter n debounce value\r\n" 
	    "cnt val n value    : Set counter n to value\r\n" 
	    "-- Examples\r\n" 
	    "  cnt io 1 14     => Associate counter 1 to gpio14\r\n" 
	    "  cnt dly 2 50    => Set counter 2 debounce time to 50ms\r\n" 
	    "  cnt val 1 1234  => Set counter 1 to value 1234\r\n" ;

const char HELP_TINFO[] PROGMEM  = "\r\n===== TeleInfo\r\n" 
      "tinfo edf on|off   : Set EDF TeleInfo\r\n" 
      "-- Examples\r\n" 
      "  tinfo edf on    => Enable EDF TeleInfo\r\n";

const char HELP_SENSOR[] PROGMEM = 	"\r\n===== Sensors\r\n" 
	    "sens si7021 on|off : Enable/Disable SI7021 internal sensor\r\n" 
	    "sens sht10 on|off  : Enable/Disable SHT10 external sensor\r\n" 
	    "sens mcp3421 on|off: Enable/Disable ADC MCP3421 Power external sensor\r\n" 
	    "sens freq f        : Set sensor measure interval each f sec\r\n" 
     
	    "sens temp led l,h   : Set RGB Led temperature warning low/High Range\r\n" 
	    "sens hum led l,h    : Set RGB Led humidity warning low/High Range\r\n" 
	    "-- Examples\r\n" 
	    "  sens temp led 15,25 => Set green led if temp is between 15 and 25 (check also humidity)\r\n" 
	    "  sens hum led 30,70  => Set green led if humidity is between 30%RH and 70%RH\r\n" 
	    "    Both above commands set green led IF temp is between 15 and 25\r\n" 
	    "      AND if humidity is between 30%RH and 70%RH\r\n" ;
