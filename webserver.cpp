// **********************************************************************************
// ESP8266 NRJMeter WEB Server, route web function
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
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2015-06-14 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

// Include Arduino header
#include "webserver.h"

// Web server
AsyncWebServer web_server(80);
AsyncWebSocket ws("/ws"); // access at ws://[esp ip]/ws

// State Machine for WebSocket Client;
_ws_client ws_client[MAX_WS_CLIENT]; 


const char* http_username = "admin";
const char* http_password = "admin";



class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {
  }

  bool canHandle(AsyncWebServerRequest *request) {
    // redirect if not in wifi client mode (through filter)
    // and request for different host (due to DNS * response)
    if (request->host() != WiFi.softAPIP().toString())
      return true;
    else
      return false;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    Debugf("Captive request to %s\n", request->url().c_str());
    String location = "http://" + WiFi.softAPIP().toString();
    if (request->host() == WiFi.hostname() + ".local")
      location += request->url();
    request->redirect(location);
  }
};



/* ======================================================================
Function: formatSize 
Purpose : format a asize to human readable format
Input   : size 
Output  : formated string
Comments: -
====================================================================== */
String formatSize(size_t bytes)
{
  if (bytes < 1024){
    return String(bytes) + F(" Byte");
  } else if(bytes < (1024 * 1024)){
    return String(bytes/1024.0) + F(" KB");
  } else if(bytes < (1024 * 1024 * 1024)){
    return String(bytes/1024.0/1024.0) + F(" MB");
  } else {
    return String(bytes/1024.0/1024.0/1024.0) + F(" GB");
  }
}

/* ======================================================================
Function: getValue 
Purpose : return the nth element of a string separated by char
Input   : string
Output  : value
Comments: -
====================================================================== */
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++) {
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}


/* ======================================================================
Function: handleFormConfig 
Purpose : handle main configuration page
Input   : -
Output  : - 
Comments: -
====================================================================== */
void handleFormConfig(AsyncWebServerRequest *request) 
{
  String response="";
  int ret ;

  DebuglnF("Serving handleFormConfig...");
  Debugflush(); 

  // We validated config ?
  // Cant't use PSTR CFG_SAVE here, crash dump hasArg don't have _P
  if ( request->hasArg( "save") ) {
    uint8_t params = request->params();
    int i, l;
    char buff[CFG_SERIAL_BUFFER_SIZE+2]; // Assume Max Len buffer for Arg + = + value 
    Debugf("===== Posted configuration with %d parameters\r\n", params);
    Debugflush(); 


    //resetConfig();
    // enable default configuration, zero all 
    memset(&config, 0, sizeof(_Config));

    
/*
    // Since Checkbox unchecked are not send (we receive only the one checked as cfg_box=on)
    // We clear all related options of them, if checked we will be receive it
    config.config &= ~(CFG_LCD|CFG_DEBUG|CFG_RGB_LED|CFG_AP|CFG_STATIC|CFG_WIFI);

    // same thing for field that can be empty, we do not receive them and want to
    // be able to clear them
    *config.psk='\0';
    *config.ap_psk='\0';
    *config.ota_auth='\0';
    *config.emoncms.host='\0';
    *config.emoncms.apikey='\0';
    *config.emoncms.url='\0';

    *config.jeedom.host='\0';
    *config.jeedom.apikey='\0';
    *config.jeedom.url='\0';
    *config.jeedom.adco='\0';

    *config.domz.host='\0';
    *config.domz.url='\0';
    *config.domz.user='\0';
    *config.domz.pass='\0';

    config.sensors.en_si7021 = false;
    config.sensors.en_sht10 = false;
    config.sensors.en_mcp3421 = false;
    config.ip = 0;
    config.mask = 0;
    config.gw = 0;
    config.dns = 0;
*/

    // Navigate for all args, and simulate as it was typed from command line
    for ( i = 0; i < params; i++ ) {
      AsyncWebParameter* param = request->getParam(i);
      // calc total size + ' ' + '\0'
      l = 2 +  param->name().length() + param->value().length() ;
      //Debugf("[%02d][%02d] %s %s\r\n", i, l, param->name().c_str(), param->value().c_str());
      //Debugflush();

      // fit in buffer and not save command
      if (l<CFG_SERIAL_BUFFER_SIZE && param->name()!="save") {
        sprintf_P(buff, PSTR("%s %s"), param->name().c_str(), param->value().c_str());
        Debugf("[%02d] %s\r\n", i, buff);
        Debugflush();
        execCmd(buff);
      }
    }

    // now we can save the config
    if ( saveConfig() ) {
      ret = 200;
      response = PSTR("OK");
    } else {
      ret = 412;
      response = PSTR("Error while saving configuration");
    }

  } else  {
    ret = 400;
    response = PSTR("Missing Form Field");
  }

  DebugF("Sending "); 
  Debug(ret); 
  Debug(":"); 
  Debugln(response); 
  request->send ( ret, "text/plain", response);
}

/* ======================================================================
Function: handleFormCounter 
Purpose : handle counters values
Input   : -
Output  : - 
Comments: -
====================================================================== */
void handleFormCounter(AsyncWebServerRequest *request) 
{
  String response="";
  int ret, l ;

  DebugF("Serving handleFormCounter...");
  Debugflush(); 

  // We validated config ?
  if ( request->hasArg(CFG_COUNT1_VALUE) || request->hasArg(CFG_COUNT2_VALUE)) {
    uint8_t params = request->params();
    char buff[CFG_SERIAL_BUFFER_SIZE+2]; // Assume Max Len buffer for Arg + = + value 

    DebuglnF("===== Posted counter"); 
    Debugflush(); 

    // Navigate for all args, 
    for ( uint8_t i = 0; i < params; i++ ) {
      AsyncWebParameter* param = request->getParam(i);
      // calc total size + ' ' + '\0'
      l = 2 + param->name().length() + param->value().length() ;

      // fit in buffer and not save command
      if (l<CFG_SERIAL_BUFFER_SIZE && (param->name()=="CFG_COUNT1_VALUE" || param->name()=="CFG_COUNT2_VALUE") ) {

        snprintf(buff, l, "%s %s", param->name().c_str(), param->value().c_str());
        Debugf("[%02d] %s\r\n", i, buff);
        Debugflush();

        execCmd(buff);
      }
    }

    // now we can save the config
    if ( saveConfig() ) {
      ret = 200;
      response = PSTR("OK");
    } else {
      ret = 412;
      response = PSTR("Error while saving counter value");
    }

  } else  {
    ret = 400;
    response = PSTR("Missing counter value");
  }

  DebugF("Sending "); 
  Debug(ret); 
  Debug(":"); 
  Debugln(response); 
  request->send ( ret, "text/plain", response);

}


/* ======================================================================
Function: sysJSONTable 
Purpose : dump all sysinfo values in JSON table format for browser
Input   : request pointer if comming from web request
Output  : JsonStr filled if request is null 
Comments: -
====================================================================== */
String sysJSONTable(AsyncWebServerRequest * request)
{
  String JsonStr="";

  // If Web request or just string asking, we'll do JSon stuff 
  // in async response,
  AsyncJsonResponse * response = new AsyncJsonResponse(true);
  JsonArray& arr = response->getRoot(); 
    
  // Web request ?
  if (request) {
    DebugF("Serving /system page...");
  } else {
    DebugF("Getting system JSON table...");
  }

  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "Uptime";
    item[FPSTR(FP_VA)] = seconds; }

  // Free mem should be last one but not really readable on bottom table
  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "Free Ram";
    item[FPSTR(FP_VA)] = formatSize(system_get_free_heap_size());  }

  char version[7];
  sprintf_P(version, PSTR("%d.%d"), NRJMETER_VERSION_MAJOR, NRJMETER_VERSION_MINOR);
  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "Firmware Version";
    item[FPSTR(FP_VA)] = version; }

  String compiled =  __DATE__ " " __TIME__;
  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "compiled";
    item[FPSTR(FP_VA)] = compiled; }

  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "SDK Version";
    item[FPSTR(FP_VA)] = system_get_sdk_version(); }

  char analog[8]; 
  sprintf_P( analog, PSTR("%d mV"), ((1000*analogRead(A0))/1024) );  
  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "Analog";
    item[FPSTR(FP_VA)] = analog; }

  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "Reset cause";
    item[FPSTR(FP_VA)] = ESP.getResetReason(); }

  char chipid[9];
  sprintf_P(chipid, PSTR("0x%06X"), system_get_chip_id() );
  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "Chip ID";
    item[FPSTR(FP_VA)] = chipid; }

  char boot_version[7]; 
  sprintf_P(boot_version, PSTR("0x%0X"), system_get_boot_version() );
  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "Boot Version";
    item[FPSTR(FP_VA)] = boot_version ; }

  // WiFi Informations
  // =================
  const char* modes[] = { "NULL", "STA", "AP", "STA+AP" };
  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "Wifi Mode";
    item[FPSTR(FP_VA)] = modes[wifi_get_opmode()]; }

  const char* phymodes[] = { "", "B", "G", "N" };
  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "Wifi 802.11 Type";
    item[FPSTR(FP_VA)] = phymodes[(int) wifi_get_phy_mode()]; }

  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "Wifi Channel";
    item[FPSTR(FP_VA)] = wifi_get_channel(); }

  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "Wifi AP ID";
    item[FPSTR(FP_VA)] = wifi_station_get_current_ap_id(); }

  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "Wifi Status";
    item[FPSTR(FP_VA)] = (int) wifi_station_get_connect_status(); }

  { JsonObject& item = arr.createNestedObject();
    item["na"] = "Wifi established in (ms)";
    item["va"] = wifi_connect_time; }

  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "Wifi Autoconnect";
    item[FPSTR(FP_VA)] = wifi_station_get_auto_connect(); }



  // Sensors Stuff
  // =============
  char sensors[32]="";
  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "I2C devices";
    if (config.config&CFG_SI7021) strcat(sensors, "SI7021 ");
    if (config.config&CFG_SHT10) strcat(sensors, "SHT1x ");
    if (config.config&CFG_MCP3421) strcat(sensors, "MCP3421 ");
    if (config.config&CFG_MCP4725) strcat(sensors, "MCP4725 ");
    if (config.config&CFG_HASOLED) strcat(sensors, "OLED ");
    item[FPSTR(FP_VA)] = sensors ; }

  // Flash Stuff
  // ===========
  String FlashChipRealSize = formatSize(ESP.getFlashChipRealSize());
  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "Flash Real Size";
    item[FPSTR(FP_VA)] = FlashChipRealSize ; }

  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "Flash IDE Speed";
    item[FPSTR(FP_VA)] = ESP.getFlashChipSpeed()/1000000 ; }

  char ide_mode[8];
  FlashMode_t im = ESP.getFlashChipMode();
  sprintf_P(ide_mode, PSTR("%s"), im==FM_QIO?"QIO":im==FM_QOUT?"QOUT":im==FM_DIO?"DIO":im==FM_DOUT?"DOUT":"UNKNOWN" );
  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "Flash IDE Mode";
    item[FPSTR(FP_VA)] = ide_mode; }

  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "Firmware Size";
    item[FPSTR(FP_VA)] = formatSize(ESP.getSketchSize()); }

  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "Free Size";
    item[FPSTR(FP_VA)] = formatSize(ESP.getFreeSketchSpace()); }

  // SPIFFS Informations
  // ===================
  FSInfo info;
  SPIFFS.info(info);

  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "SPIFFS Total";
    item[FPSTR(FP_VA)] =  formatSize(info.totalBytes); }

  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "SPIFFS Used";
    item[FPSTR(FP_VA)] = formatSize(info.totalBytes); }

  { JsonObject& item = arr.createNestedObject();
    item[FPSTR(FP_NA)] = "SPIFFS Occupation (%)";
    item[FPSTR(FP_VA)] = 100*info.usedBytes/info.totalBytes; }

  // Web request send response to client
  size_t jsonlen ;
  if (request) {
    jsonlen = response->setLength();
    request->send(response); 
  } else {
    // Send JSon to our string
    arr.printTo(JsonStr); 
    jsonlen =  arr.measureLength();
    // Since it's nor a WEB request, we need to manually delete
    // response object so ArduinJSon object is freed
    delete response;
  }
  //Debugf("Json size %lu bytes\r\n", jsonlen);

  // Will be empty for web request
  return JsonStr;
}

/* ======================================================================
Function: sensorsJSONTable 
Purpose : dump all sensors values in JSON table format for browser
Input   : request pointer if comming from web request
Output  : JsonStr filled if request is null 
Comments: -
====================================================================== */
String sensorsJSONTable(AsyncWebServerRequest * request)
{
  String JsonStr="";

  // If Web request or just string asking, we'll do JSon stuff 
  // in async response,
  AsyncJsonResponse * response = new AsyncJsonResponse();
  JsonObject& root = response->getRoot(); 

  if (request) {
    DebugF("Serving /sensors page...");
  } else {
    DebugF("Getting sensors JSON table...");
  }

  // Do a measurment
  sensors_measure();

  JsonArray& a_si7021 = root.createNestedArray("si7021");
  JsonObject& o_si7021 = a_si7021.createNestedObject();
  o_si7021[FPSTR(FP_TEMPERATURE)] = si7021_temperature/100.0;
  o_si7021[FPSTR(FP_HUMIDITY)]    = si7021_humidity/100.0 ;   
  o_si7021[FPSTR(FP_SEEN)]        = (int)((config.config&CFG_SI7021)?si7021_last_seen:-1);  

  JsonArray& a_sht10 = root.createNestedArray("sht10");
  JsonObject& o_sht10 = a_sht10.createNestedObject();
  o_sht10[FPSTR(FP_TEMPERATURE)] = sht1x_temperature/100.0;
  o_sht10[FPSTR(FP_HUMIDITY)]    = sht1x_humidity/100.0 ;   
  o_sht10[FPSTR(FP_SEEN)]        = (int)((config.config&CFG_SHT10)?sht1x_last_seen:-1);  

  JsonArray& a_mcp3421 = root.createNestedArray("mcp3421");
  JsonObject& o_mcp3421 = a_mcp3421.createNestedObject();
  o_mcp3421[FPSTR(FP_POWER)] = mcp3421_power;
  o_mcp3421[FPSTR(FP_SEEN)]  = (int)((config.config&CFG_MCP3421)?mcp3421_last_seen:-1);  

  // Web request send response to client
  size_t jsonlen ;
  if (request) {
    jsonlen = response->setLength();
    request->send(response); 
  } else {
    // Send JSon to our string
    root.printTo(JsonStr); 
    jsonlen =  root.measureLength();
    // Since it's nor a WEB request, we need to manually delete
    // response object so ArduinJSon object is freed
    delete response;
  }
  //Debugf("Json size %lu bytes\r\n", jsonlen);

  // Will be empty for web request
  return JsonStr;
}

/* ======================================================================
Function: confJSONTable 
Purpose : dump all config values in JSON table format for browser
Input   : -
Output  : - 
Comments: -
====================================================================== */
void confJSONTable(AsyncWebServerRequest * request)
{
  IPAddress ip_addr;
  size_t l;
  String str;

  AsyncJsonResponse * response = new AsyncJsonResponse();
  JsonObject& root = response->getRoot(); 

  DebuglnF("Serving /config page...");
  // Debugf("Stage 1 : JSONBufferSize:%d  FreeRam:%d\r\n", response->getSize(), ESP.getFreeHeap());

  root[FPSTR(CFG_SSID)]    = config.ssid;
  root[FPSTR(CFG_PSK)]     = config.psk;
  root[FPSTR(CFG_HOST)]    = config.host ;
  root[FPSTR(CFG_AP_PSK)]  = config.ap_psk ;
  root[FPSTR(CFG_AP_SSID)] = config.ap_ssid ;

  ip_addr=config.ip;   root[FPSTR(CFG_IP)]      = ip_addr.toString();
  ip_addr=config.mask; root[FPSTR(CFG_MASK)]    = ip_addr.toString();
  ip_addr=config.gw;   root[FPSTR(CFG_GATEWAY)] = ip_addr.toString();
  ip_addr=config.dns;  root[FPSTR(CFG_DNS)]     = ip_addr.toString();
  root[FPSTR(CFG_EMON_HOST)] = config.emoncms.host;
  root[FPSTR(CFG_EMON_PORT)] = (unsigned int) config.emoncms.port;
  root[FPSTR(CFG_EMON_URL)]  = config.emoncms.url;
  root[FPSTR(CFG_EMON_KEY)]  = config.emoncms.apikey;
  root[FPSTR(CFG_EMON_NODE)] = (unsigned int) config.emoncms.node;
  root[FPSTR(CFG_EMON_FREQ)] = (unsigned int) config.emoncms.freq;

  root[FPSTR(CFG_JDOM_HOST)] = config.jeedom.host;
  root[FPSTR(CFG_JDOM_PORT)] = (unsigned int) config.jeedom.port;
  root[FPSTR(CFG_JDOM_URL)]  = config.jeedom.url;
  root[FPSTR(CFG_JDOM_KEY)]  = config.jeedom.apikey;
  root[FPSTR(CFG_JDOM_ADCO)] = config.jeedom.adco;
  root[FPSTR(CFG_JDOM_FREQ)] = (unsigned int) config.jeedom.freq;

  root[FPSTR(CFG_DOMZ_HOST)] = config.domz.host;
  root[FPSTR(CFG_DOMZ_PORT)] = (unsigned int) config.domz.port;
  root[FPSTR(CFG_DOMZ_URL)]  = config.domz.url;
  root[FPSTR(CFG_DOMZ_USER)] = config.domz.user;
  root[FPSTR(CFG_DOMZ_PASS)] = config.domz.pass;
  root[FPSTR(CFG_DOMZ_INDEX)]= (unsigned int) config.domz.index;
  root[FPSTR(CFG_DOMZ_FREQ)] = (unsigned int) config.domz.freq;

  root[FPSTR(CFG_COUNT1_GPIO)]  = (unsigned int) config.counter.gpio1;
  root[FPSTR(CFG_COUNT1_DELAY)] = (unsigned int) config.counter.delay1;
  root[FPSTR(CFG_COUNT1_VALUE)] = (unsigned long) config.counter.value1;
  root[FPSTR(CFG_COUNT2_GPIO)]  = (unsigned int) config.counter.gpio2;
  root[FPSTR(CFG_COUNT2_DELAY)] = (unsigned int) config.counter.delay2;
  root[FPSTR(CFG_COUNT2_VALUE)] = (unsigned long) config.counter.value2;

  root[FPSTR(CFG_OTA_AUTH)] = config.ota_auth;
  root[FPSTR(CFG_OTA_PORT)] = (unsigned int) config.ota_port;
  root[FPSTR(CFG_SENS_SI7021)]= (unsigned int) config.sensors.en_si7021;
  root[FPSTR(CFG_SENS_SHT10)] = (unsigned int) config.sensors.en_sht10;
  root[FPSTR(CFG_SENS_MCP3421)] = (unsigned int) config.sensors.en_mcp3421;

  root[FPSTR(CFG_CFG_RGBLED)] = (config.config&CFG_RGB_LED)?1:0;
  root[FPSTR(CFG_CFG_DEBUG)]  = (config.config&CFG_DEBUG)?1:0;
  root[FPSTR(CFG_CFG_OLED)]   = (config.config&CFG_LCD)?1:0;
  root[FPSTR(CFG_CFG_AP)]     = (config.config&CFG_AP)?1:0;
  root[FPSTR(CFG_CFG_WIFI)]   = (config.config&CFG_WIFI)?1:0;
  root[FPSTR(CFG_CFG_STATIC)] = (config.config&CFG_STATIC)?1:0;
  root[FPSTR(CFG_CFG_SI7021)] = (config.config&CFG_SI7021)?1:0;
  root[FPSTR(CFG_CFG_SHT10)]  = (config.config&CFG_SHT10)?1:0;
  root[FPSTR(CFG_CFG_MCP3421)]= (config.config&CFG_MCP3421)?1:0;
  root[FPSTR(CFG_CFG_HASOLED)]= (config.config&CFG_HASOLED)?1:0;

  root[FPSTR(CFG_LED_BRIGHTNESS)] = (unsigned int) config.led_bright;
  root[FPSTR(CFG_LED_HEARTBEAT)]  = (unsigned int) config.led_hb;
  root[FPSTR(CFG_LED_NUM)]        = (unsigned int) config.led_num;
  root[FPSTR(CFG_LED_GPIO)]       = (unsigned int) config.led_gpio;
  root[FPSTR(CFG_LED_TYPE)]       = (unsigned int) config.led_type;

  root[FPSTR(CFG_SENS_FREQ)] = (unsigned int) config.sensors.freq;
  str = config.sensors.hum_min_warn; str += "," ; str += config.sensors.hum_max_warn;
  root[FPSTR(CFG_SENS_HUM_LED)]   = str;
  str = config.sensors.temp_min_warn; str += "," ; str += config.sensors.temp_max_warn;
  root[FPSTR(CFG_SENS_TEMP_LED)]  = str;
  str = config.sensors.pwr_min_warn; str += "," ; str += config.sensors.pwr_max_warn;
  root[FPSTR(CFG_SENS_PWR_LED)]  = str;

  size_t jsonlen ;
  jsonlen = response->setLength();
  request->send(response);
  //Debugf("Json size %lu bytes\r\n", jsonlen);
}

/* ======================================================================
Function: spiffsJSONTable 
Purpose : dump all spiffs system in JSON table format for browser
Input   : -
Output  : - 
Comments: -
====================================================================== */
void spiffsJSONTable(AsyncWebServerRequest * request)
{
  AsyncJsonResponse * response = new AsyncJsonResponse();
  JsonObject& root = response->getRoot(); 

  DebugF("Serving /spiffs page...");

  // Loop trough all files
  JsonArray& a_files = root.createNestedArray("files");
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {    
    JsonObject& o_item = a_files.createNestedObject();
    o_item[FPSTR(FP_NA)] = dir.fileName();
    o_item[FPSTR(FP_VA)] = dir.fileSize();
  }

  // Get SPIFFS File system informations
  FSInfo info;
  SPIFFS.info(info);
  JsonArray& a_spiffs = root.createNestedArray("spiffs");
  JsonObject& o_item = a_spiffs.createNestedObject();
  o_item["Total"] = info.totalBytes;
  o_item["Used"]  = info.usedBytes ;
  o_item["ram"]   = system_get_free_heap_size();

  size_t jsonlen ;
  jsonlen = response->setLength();
  request->send(response);
  //Debugf("Json size %lu bytes\r\n", jsonlen);
}

/* ======================================================================
Function: wifiScanJSON 
Purpose : scan Wifi Access Point and return JSON code
Input   : -
Output  : - 
Comments: 
====================================================================== */
void wifiScanJSON(AsyncWebServerRequest * request)
{
  AsyncJsonResponse * response = new AsyncJsonResponse();
  JsonObject& root = response->getRoot(); 

  // Just to debug where we are
  Debug(F("Serving /wifiscan page..."));

  // Return WIFI_SCAN_RUNNING -1 or WIFI_SCAN_FAILED -2
  int n = WiFi.scanComplete();

  root[FPSTR(FP_STATUS)] = n;

  if(n == WIFI_SCAN_FAILED) {
    DebugF("No results, starting scan...");
    WiFi.scanNetworks(true);
  } else if (n) {
    char buff[] = "????";
    int qual;

    JsonArray& a_net = root.createNestedArray("networks");
    // Just to debug where we are
    Debugf("Scan done:%d networks!\r\n",n);

    for (int i = 0; i < n; ++i) {
      switch(WiFi.encryptionType(i)) {
        case ENC_TYPE_NONE: strcpy_P(buff, PSTR("Open"));  break;
        case ENC_TYPE_WEP:  strcpy_P(buff, PSTR("WEP")) ;  break;
        case ENC_TYPE_TKIP: strcpy_P(buff, PSTR("WPA")) ;  break;
        case ENC_TYPE_CCMP: strcpy_P(buff, PSTR("WPA2"));  break;
        case ENC_TYPE_AUTO: strcpy_P(buff, PSTR("Auto"));  break;
      }

      Debugf("[%d] '%s' Encryption=%s Channel=%d\r\n", i, WiFi.SSID(i).c_str(), buff, WiFi.channel(i));

      JsonObject& item = a_net.createNestedObject();
      item[FPSTR(FP_SSID)]       = WiFi.SSID(i);
      item[FPSTR(FP_RSSI)]       = WiFi.RSSI(i); 
      item[FPSTR(FP_ENCRYPTION)] = buff; 
      item[FPSTR(FP_CHANNEL)]    = WiFi.channel(i); 
    }

    size_t jsonlen ;
    jsonlen = response->setLength();
    request->send(response); 
    //Debugf("%d bytes\r\n", jsonlen);

    WiFi.scanDelete();
    if(WiFi.scanComplete() == WIFI_SCAN_FAILED) {
      WiFi.scanNetworks(true);
    }
  }
}

/* ======================================================================
Function: handleFactoryReset 
Purpose : reset the module to factory settingd
Input   : -
Output  : - 
Comments: -
====================================================================== */
void handleFactoryReset(AsyncWebServerRequest * request)
{
  // Just to debug where we are
  Debug(F("Serving /factory_reset page..."));
  request->send ( 200, "text/plain", FPSTR(FP_RESTART) );
  Debugln(F("Ok!"));
  ESP.eraseConfig(); // Delete SDK Config (Wifi Credentials)
  resetConfig();
  resetBoard();
}

/* ======================================================================
Function: handleReset 
Purpose : reset the module
Input   : -
Output  : - 
Comments: -
====================================================================== */
void handleReset(AsyncWebServerRequest * request)
{
  // Just to debug where we are
  Debug(F("Serving /reset page..."));
  request->send ( 200, "text/plain", FPSTR(FP_RESTART) );
  Debugln(F("Ok!"));
  resetBoard();
}

/* ======================================================================
Function: handleNotFound 
Purpose : default WEB routing when URI is not found
Input   : -
Output  : - 
Comments: -
====================================================================== */
void handleNotFound(AsyncWebServerRequest * request) 
{

  // Handle captive portal
  if ( request->url().startsWith("/hotspot-detect.html") 
       || request->url().startsWith("/fwlink") 
       || request->url().startsWith("/generate_204")) {

    // Redirect to main mage
    request->redirect("/");

  } else {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    //response->addHeader("Server","ESP Async Web Server");
    response->printf("<!DOCTYPE html><html><head><title>Webpage at %s</title></head><body>", request->url().c_str());
    response->print("<h2>Sorry ");
    response->print(request->client()->remoteIP());
    response->print("We're unable to process this page</h2>");

    response->print("<h3>Information</h3>");
    response->print("<ul>");
    response->printf("<li>Version: HTTP/1.%u</li>", request->version());
    response->printf("<li>Method: %s</li>", request->methodToString());
    response->printf("<li>URL: %s</li>", request->url().c_str());
    response->printf("<li>Host: %s</li>", request->host().c_str());
    response->printf("<li>ContentType: %s</li>", request->contentType().c_str());
    response->printf("<li>ContentLength: %u</li>", request->contentLength());
    response->printf("<li>Multipart: %s</li>", request->multipart()?"true":"false");
    response->print("</ul>");

    response->print("<h3>Headers</h3>");
    response->print("<ul>");
    int headers = request->headers();
    for (int i=0;i<headers;i++) {
      AsyncWebHeader* h = request->getHeader(i);
      response->printf("<li>%s: %s</li>", h->name().c_str(), h->value().c_str());
    }
    response->print("</ul>");

    response->print("<h3>Parameters</h3>");
    response->print("<ul>");
    int params = request->params();
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if( p->isFile() ) {
        response->printf("<li>FILE[%s]: %s, size: %u</li>", p->name().c_str(), p->value().c_str(), p->size());
      } else if ( p->isPost() ) {
        response->printf("<li>POST[%s]: %s</li>", p->name().c_str(), p->value().c_str());
      } else {
        response->printf("<li>GET[%s]: %s</li>", p->name().c_str(), p->value().c_str());
      }
    }
    response->print("</ul>");

    response->print("</body></html>");
    //send the response last
    request->send(response);
  }
}

/* ======================================================================
Function: webSocketEvent
Purpose : Manage routing of websocket events
Input   : -
Output  : - 
Comments: -
====================================================================== */
void webSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len)
{
  //Handle WebSocket event
  switch(type) {
    
    case WS_EVT_DISCONNECT:
      //client disconnected
      Debugf("ws[%s][%u] Disconnect ", server->url(), client->id());

      for (uint8_t i=0; i<MAX_WS_CLIENT ; i++) {
        if (ws_client[i].id == client->id() ) {
          ws_client[i].id = 0;
          ws_client[i].state = CLIENT_NONE;
          ws_client[i].refresh = 0;
          ws_client[i].tick = 0;
          Debugf("freed[%d]\n", i);
          i=MAX_WS_CLIENT; // Exit for loop
        }
      }
    break;

    case WS_EVT_CONNECT:
      uint8_t i;
      //client connected
      Debugf("ws[%s][%u] connect ", server->url(), client->id() );
      // Search a free location to store client information
      for (i=0; i<MAX_WS_CLIENT ; i++) {
        if (ws_client[i].id == 0 ) {
          ws_client[i].id = client->id();
          ws_client[i].state = CLIENT_NONE;
          ws_client[i].refresh = 0;
          ws_client[i].tick = 0;
          Debugf("added[%d]\n", i);

          String response = "{message:\"config\", \"data\":[{\"ledpanel\":";
          response += config.led_panel;
          response += "}]}";
          Debugf("ws[%u][%u] sending %s\n", ws_client[i].id, i, response.c_str());    

          // send message to this connected client
          ws.text(ws_client[i].id, response);
          break; // Exit for loop
        }
      }
      if (i>MAX_WS_CLIENT) {
        DebuglnF("not added, table is full");
      }
    break;

    case WS_EVT_ERROR: 
      //error was received from the other end
      Debugf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);    
    break;

    case WS_EVT_PONG: 
      //pong message was received (in response to a ping request maybe)
      Debugf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");  
    break;

    case WS_EVT_DATA: {
      //data packet
      AwsFrameInfo * info = (AwsFrameInfo*) arg;

      if (info->final && info->index == 0 && info->len == len) {
        //the whole message is in a single frame and we got all of it's data
        Debugf("ws[%s][#%u] %s-msg[%lu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", (uint32_t) info->len);

        // Search if it's a known client
        for (uint8_t index=0; index<MAX_WS_CLIENT ; index++) {
          if (ws_client[index].id == client->id() ) {
            String msg = "";
            char buff[3];
            int val;

            // Grab all data
            for(size_t i=0; i < info->len; i++) {
              if (info->opcode == WS_TEXT ) {
                msg += (char) data[i];
              } else {
                sprintf(buff, "%02x ", (uint8_t) data[i]);
                msg += buff ;
              }
            }
            Debugf("known[%d] '%s'\n", index, msg.c_str());
            Debugf("client #%d info state=%d, tick=%u, refresh=%u\n", 
                    client->id(), ws_client[index].state, ws_client[index].tick, ws_client[index].refresh);

            // Received text message
            if (info->opcode == WS_TEXT) {
              // Is it a command ?
              if (msg.startsWith("$")) {
                msg = msg.substring(1);
                if (msg=="system") {
                 ws_client[index].state = CLIENT_SYSTEM;
                } else if (msg=="config") {
                 DebuglnF("config");
                 ws_client[index].state = CLIENT_CONFIG;
                } else if (msg=="log") {
                 DebuglnF("log");
                 ws_client[index].state = CLIENT_LOG;
                } else if (msg=="spiffs") {
                 DebuglnF("spiffs");
                 ws_client[index].state = CLIENT_SPIFFS;
                } else if (msg.startsWith("sensors:") &&  info->len >= 9 ) {
                  val = msg.substring(8).toInt();
                  Debugf("sensor=%d",val);
                  ws_client[index].state = CLIENT_SENSORS;
                  if (val>=2 && val <=300)
                    ws_client[index].refresh = val;
                } else if (msg.startsWith("rgbb:") &&  info->len >= 6 ) {
                  val = msg.substring(5).toInt();
                  Debugf("RGB brightness=%d",val);
                  ws_client[index].state = CLIENT_SENSORS;
                  if (val>=0 && val <=100)
                    config.led_bright = val;
                } else if (msg.startsWith("lpnl:") &&  info->len >= 6 ) {
                  val = msg.substring(5).toInt();
                  Debugf("Panel Brightness=%d",val);
                  ws_client[index].state = CLIENT_SENSORS;
                  if (val>=0 && val <=100)
                    config.led_panel = val;
                }
              // It's just text, pass to command interpreter
              } else {
                if (msg.length()>0) {
                  handle_serial((char*) msg.c_str(), client->id() );
                }
              }
            } else {
              //client->binary("I got your binary message");
            }
          } // if known client

          // Exit for loop
          index = MAX_WS_CLIENT;

        } // for all clients
      }
    }
    break;
  }
}

/* ======================================================================
Function: WS_setup
Purpose : Do the async webserver setup
Input   : -
Output  : -
Comments: -
====================================================================== */
void WS_setup(void)
{


  web_server.onFileUpload([](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index) {
      Debugf("UploadStart: %s\n", filename.c_str());
    }
    Debugf("%s", (const char*)data);
    if(final) {
      Debugf("UploadEnd: %s (%u)\n", filename.c_str(), index+len);
    }
  });

  web_server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if(!index) {
      Debugf("BodyStart: %u\n", total);
    }
    Debugf("%s", (const char*) data);
    if(index + len == total) {
      Debugf("BodyEnd: %u\n", total);
    }
  });


  // Create SPIFFS Editor
  web_server.addHandler(new SPIFFSEditor(http_username,http_password));

  // handle captive portal only for access point
  web_server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);

  //server.on("/", handleRoot);
  web_server.on("/config_form",  handleFormConfig);
  web_server.on("/counter_form", handleFormCounter);
  web_server.on("/sensors",      sensorsJSONTable);
  web_server.on("/system",       sysJSONTable);
  web_server.on("/config",       confJSONTable);
  web_server.on("/spiffs",       spiffsJSONTable);
  web_server.on("/wifiscan",     wifiScanJSON);
  web_server.on("/factory_reset",handleFactoryReset);
  web_server.on("/reset",        handleReset);

  //web_server.rewrite("/index.htm", "dpi.htm").setFilter(ON_AP_FILTER);
  web_server.serveStatic("/", SPIFFS, "/index.htm").setCacheControl("max-age:86400");
  web_server.serveStatic("/", SPIFFS, "/"         ).setCacheControl("max-age:86400");
  
  // All other not known 
  // redirect all captive portal to default page
  web_server.onNotFound(handleNotFound);

  // Init client state machine
  for (uint8_t i=0; i<MAX_WS_CLIENT ; i++) {
    ws_client[i].id = 0;
    ws_client[i].state = CLIENT_NONE;
    ws_client[i].refresh = 0;
    ws_client[i].tick = 0;
  }

  // attach AsyncWebSocket
  ws.onEvent(webSocketEvent);
  web_server.addHandler(&ws);

  // Start server
  web_server.begin();
  Debug(F("HTTP server started on http://"));
  if (pdnsServer) {
    Debugln(WiFi.softAPIP());
  } else {
    Debugln(WiFi.localIP());
  }

}

