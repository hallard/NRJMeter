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
#include "config.h"

// Configuration structure for whole program
_Config config;

uint8_t ser_idx = 0;
char    ser_buf[CFG_SERIAL_BUFFER_SIZE + 1];

uint16_t crc16Update(uint16_t crc, uint8_t a)
{
  int i;
  crc ^= a;
  for (i = 0; i < 8; ++i)  {
    if (crc & 1)
      crc = (crc >> 1) ^ 0xA001;
    else
      crc = (crc >> 1);
  }
  return crc;
}

/* ======================================================================
  Function: outputBuffer
  Purpose : dump buffer to Debug/Serial or on websocket
  Input   : buffer
          websocket client ID, 0 if no websoket
  Output  : -
  Comments: -
  ====================================================================== */
void outputBuffer(char * buff, uint32_t clientid)
{
  if (clientid)
    // Send via WebSocket
    ws.text(clientid, buff, strlen(buff));
  else
    // Send to Serial
    DEBUG_SERIAL.print(buff);
}

/* ======================================================================
  Function: eeprom_dump
  Purpose : dump eeprom value to serial
  Input 	: -
  Output	: -
  Comments: -
  ====================================================================== */
void eepromDump(uint8_t bytesPerRow, uint32_t clientid)
{
  uint16_t i, b;
  uint16_t j = 0 ;
  char buff[256];

  // default to 16 bytes per row
  if (bytesPerRow == 0 || bytesPerRow > 64)
    bytesPerRow = 16;

  Debugln();

  // loop thru EEP address
  for (i = 0; i < sizeof(_Config); i++) {
    // First byte of the row ?
    if (j == 0) {
      // Display Address
      sprintf_P(buff, PSTR("%04X ; "), i);
    }

    // add write byte in hex form
    sprintf_P(buff + strlen(buff), PSTR("%02X "), EEPROM.read(i));

    // Last byte of the row ?
    // start a new line and dump row
    if (++j >= bytesPerRow) {
      j = 0;
      outputBuffer(buff, clientid);
    }
  }
}

/* ======================================================================
  Function: readConfig
  Purpose : fill config structure with data located into eeprom
  Input 	: true if we need to clear actual struc in case of error
  Output	: true if config found and crc ok, false otherwise
  Comments: -
  ====================================================================== */
bool readConfig (bool clear_on_error, uint32_t clientid)
{
  uint16_t crc = ~0;
  uint8_t * pconfig = (uint8_t *) &config ;
  uint8_t data ;

  // For whole size of config structure
  for (uint16_t i = 0; i < sizeof(_Config); ++i) {
    // read data
    data = EEPROM.read(i);

    // save into struct
    *pconfig++ = data ;

    // calc CRC
    crc = crc16Update(crc, data);
  }

  // CRC Error ?
  if (crc != 0) {
    // Clear config if wanted
    if (clear_on_error) {
      resetConfig();
    }
    return false;
  }

  // This can do reset by divide by 0
  // Paranonia mode
  if (config.led_num == 0)
    config.led_num = DEFAULT_LED_NUM;

  return true ;
}

/* ======================================================================
  Function: saveConfig
  Purpose : save config structure values into eeprom
  Input 	: -
  Output	: true if saved and readback ok
  Comments: once saved, config is read again to check the CRC
  ====================================================================== */
bool saveConfig (uint32_t clientid)
{
  uint8_t * pconfig ;
  bool ret_code;

  //eepromDump(32, clientid);

  // Init pointer
  pconfig = (uint8_t *) &config ;

  // Init CRC
  config.crc = ~0;

  // For whole size of config structure, pre-calculate CRC
  for (uint16_t i = 0; i < sizeof (_Config) - 2; ++i)
    config.crc = crc16Update(config.crc, *pconfig++);

  // Re init pointer
  pconfig = (uint8_t *) &config ;

  // For whole size of config structure, write to EEP
  for (uint16_t i = 0; i < sizeof(_Config); ++i)
    EEPROM.write(i, *pconfig++);

  // Physically save
  EEPROM.commit();

  // Read Again to see if saved ok, but do
  // not clear if error this avoid clearing
  // default config and breaks OTA
  ret_code = readConfig(false);

  char buff[32];
  strcpy_P(buff, PSTR("Write config "));
  strcat_P(buff, ret_code ? PSTR("OK!") : PSTR("Error!"));

  // Send to correct client output
  outputBuffer(buff, clientid);

  //eepromDump(32, clientid);

  // return result
  return (ret_code);
}

/* ======================================================================
  Function: ResetConfig
  Purpose : Set all configuration to default values
  Input   : -
  Output  : -
  Comments: -
  ====================================================================== */
void resetConfig(uint32_t clientid)
{
  // enable default configuration, zero all
  memset(&config, 0, sizeof(_Config));

  // Set default Hostname
  sprintf_P(config.host, PSTR("NRJMeter-%c%06X"), OTA_PREFIX_ID, ESP.getChipId());

  //Set OTA default port
  config.ota_port = DEFAULT_OTA_PORT ;

  //Set Wifi Max Wait for Connect
  config.wmw = CFG_DEFAULT_WMW;
  // AP SSID
  sprintf_P(config.ap_ssid, PSTR("ch2i-NRJMeter-%c%06X"), OTA_PREFIX_ID, ESP.getChipId());

  // Add other init default config here

  //  strcpy_P(config.ssid, PSTR("YOUR-AP"));
  //  strcpy_P(config.psk, PSTR("Your-PSK"));

  // Emoncms
  strcpy_P(config.emoncms.host, PSTR(CFG_EMON_DEFAULT_HOST));
  config.emoncms.port = CFG_EMON_DEFAULT_PORT;
  strcpy_P(config.emoncms.url, PSTR(CFG_EMON_DEFAULT_URL));
  strcpy_P(config.emoncms.topic, CFG_EMON_DEFAULT_TOPIC);
  config.emoncms.bmode = ( CFG_EMON_DEFAULT_BMODE );

  // Jeedom
  strcpy_P(config.jeedom.host, CFG_JDOM_DEFAULT_HOST);
  config.jeedom.port = CFG_JDOM_DEFAULT_PORT;
  strcpy_P(config.jeedom.url, CFG_JDOM_DEFAULT_URL);
  strcpy_P(config.jeedom.topic, CFG_JDOM_DEFAULT_TOPIC);
  strcpy_P(config.jeedom.adco, CFG_DEFAULT_ADCO);
  config.jeedom.bmode = ( CFG_JDOM_DEFAULT_BMODE );

  // Domoticz
  strcpy_P(config.domz.host, CFG_DOMZ_DEFAULT_HOST);
  config.domz.port = CFG_DOMZ_DEFAULT_PORT;
  strcpy_P(config.domz.url, CFG_DOMZ_DEFAULT_URL);
  strcpy_P(config.domz.topic, CFG_DOMZ_DEFAULT_TOPIC);
  config.domz.bmode = ( CFG_DOMZ_DEFAULT_BMODE );

  // Counters
  config.counter.delay1 = CFG_COUNTER_DEFAULT_DELAY;
  config.counter.delay2 = CFG_COUNTER_DEFAULT_DELAY;
  config.counter.gpio1 = NOT_A_PIN;
  config.counter.gpio2 = NOT_A_PIN;

  // Sensors
  config.sensors.temp_min_warn  = CFG_SENSORS_TEMP_MIN_WARN ;
  config.sensors.temp_max_warn  = CFG_SENSORS_TEMP_MAX_WARN ;
  config.sensors.hum_min_warn   = CFG_SENSORS_HUM_MIN_WARN;
  config.sensors.hum_max_warn   = CFG_SENSORS_HUM_MAX_WARN ;
  config.sensors.pwr_min_warn   = CFG_SENSORS_PWR_MIN_WARN;
  config.sensors.pwr_max_warn   = CFG_SENSORS_PWR_MAX_WARN ;
  config.sensors.freq           = CFG_SENSORS_DEFAULT_FREQ ;

  //MQTT
  strcpy_P(config.mqtt.host, CFG_MQTT_DEFAULT_HOST);
  config.mqtt.port = CFG_MQTT_DEFAULT_PORT;

  //TInfo
  config.tinfo.type = CFG_TI_NONE;

  config.config = (CFG_RGB_LED | CFG_DEBUG | CFG_WIFI ) ;

  config.led_bright = DEFAULT_LED_BRIGHTNESS;
  config.led_hb = DEFAULT_LED_HEARTBEAT;
  config.led_type = DEFAULT_LED_TYPE;
  config.led_num = DEFAULT_LED_NUM;
  config.led_gpio = DEFAULT_LED_GPIO;

  // save back
  saveConfig(clientid);
}

/* ======================================================================
  Function: resetBoard
  Purpose : do a board reset
  Input   : -
  Output  : -
  Comments: -
  ====================================================================== */
void resetBoard(uint32_t clientid) {
  //webSocket.disconnect();
  //delay(100);
  //server.close();
  //DNS.stop();

  // Default boot pin mode, be sure to avoid potential lockup at boot
  pinMode(0, INPUT);
  pinMode(2, INPUT);
  pinMode(15, INPUT);
  ESP.reset();

  // Should never arrive there
  while (true);
}

/* ======================================================================
  Function: showConfig
  Purpose : display configuration
  Input 	: bits flag of section to show
  Output	: -
  Comments: -
  ====================================================================== */
void showConfig(uint16_t section, uint32_t clientid )
{
  char buff[256] = "";
#define OUT(x,y) { sprintf_P(buff+strlen(buff),PSTR(x),y); }

  if (section & CFG_HLP_SYS) {
    strcat_P(buff, PSTR(""));
    OUT("Config (0x%04X)  : ", config.config);
    if (config.config & CFG_AP)          strcat_P(buff, PSTR("ACCESS_POINT "));
    if (config.config & CFG_WIFI)        strcat_P(buff, PSTR("WIFI "));
    if (config.config & CFG_MQTT)        strcat_P(buff, PSTR("MQTT "));
    if (config.config & CFG_STATIC)      strcat_P(buff, PSTR("STATIC "));
    if (config.config & CFG_RGB_LED)     strcat_P(buff, PSTR("RGBLED "));
    if (config.config & CFG_DEBUG)       strcat_P(buff, PSTR("DEBUG "));
    if (config.config & CFG_LOGGER)      strcat_P(buff, PSTR("LOGGER "));
    if (config.config & CFG_LCD)         strcat_P(buff, PSTR("OLED "));
    if (config.config & CFG_DEMO_MODE)   strcat_P(buff, PSTR("DEMO "));
    if (config.config & CFG_TINFO)       strcat_P(buff, PSTR("TINFO "));

    if (config.config & CFG_SI7021)      strcat_P(buff, PSTR("SI7021 "));
    if (config.config & CFG_SHT10)       strcat_P(buff, PSTR("SHT10 "));
    if (config.config & CFG_MCP3421)     strcat_P(buff, PSTR("MCP3421 "));

    const char* ledtype[] = { "None", "RGB", "GRB", "RGBW", "GRBW" };
    sprintf_P(buff + strlen(buff), PSTR("\nRGB LED  : #%d GPIO%d %s  Brigth %d %%  Heartbeat %d sec\n"),
              config.led_num, config.led_gpio,
              ledtype[(uint8_t)config.led_type],
              config.led_bright, config.led_hb / 10);

    if (*config.http_usr) {
      OUT("HTTP User: %s\n", config.http_usr);
    } else {
      OUT("HTTP User: %s\n", "none");
    }
    if (*config.http_pwd) {
      OUT("HTTP Password: %s\n", config.http_pwd);
    } else {
      OUT("HTTP Password: %s\n", "none");
    }

    OUT("OTA Port : %d\nOTA Auth : ", config.ota_port);
    if (*config.ota_auth) {
      OUT("%s\n", config.ota_auth);
    } else {
      OUT("%s\n", "none");
    }

    // Send to correct client output
    outputBuffer(buff, clientid);
  }

  if (section & CFG_HLP_WIFI) {
    strcpy_P(buff, PSTR("\r\n===== Wifi\r\n"));
    OUT("SSID     : %s\n", config.ssid);
    OUT("psk      : %s\n", config.psk);
    OUT("wmw      : %d\n", config.wmw);
    OUT("host     : %s\n", config.host);
    OUT("ap SSID  : %s\n", config.ap_ssid);
    OUT("ap psk   : %s\n", config.ap_psk);
    OUT("Static IP: %s",  ((IPAddress) config.ip).toString().c_str());
    OUT("/%s  ",          ((IPAddress) config.mask).toString().c_str());
    OUT("GW:%s  ",        ((IPAddress) config.gw).toString().c_str());
    OUT("DNS:%s\n",       ((IPAddress) config.dns).toString().c_str());

    // Send to correct client output
    outputBuffer(buff, clientid);
  }

  #ifdef USE_MQTT
  if (section & CFG_HLP_MQTT) {
    strcpy_P(buff, PSTR("\r\n===== MQTT\r\n"));
    OUT("Broker     : %s\n", config.mqtt.host);
    OUT("Port       : %d\n", config.mqtt.port);
    OUT("User       : %s\n", config.mqtt.usr);
    OUT("Password   : %s\n", config.mqtt.pwd);
    OUT("In Topic   : %s\n", inTopic.c_str());
    OUT("Out Topic  : %s\n", outTopic.c_str());
    OUT("QoS        : %s\n", MQTT_QOS_STRING);
    OUT("Retain     : %s\n", MQTT_RET_STRING);
    OUT("Protocol   : %s\n", MQTT_VER_STRING);

    // Send to correct client output
    outputBuffer(buff, clientid);
  }
  #endif

  if (section & CFG_HLP_DATA) {
    strcpy_P(buff, PSTR("\r\n===== Data Server\n"));
    OUT("host  : %s\n", config.emoncms.host);
    OUT("port  : %d\n", config.emoncms.port);
    OUT("url   : %s\n", config.emoncms.url);
    OUT("key   : %s\n", config.emoncms.apikey);
    OUT("node  : %d\n", config.emoncms.node);
    OUT("freq  : %d\n", config.emoncms.freq);
    OUT("topic : %s\n", config.emoncms.topic);
    OUT("http  : %s\n", (config.emoncms.bmode & CFG_BMODE_HTTP ? "on" : "off"));
    OUT("mqtt  : %s\n", (config.emoncms.bmode & CFG_BMODE_MQTT ? "on" : "off"));

    // Send to correct client output
    outputBuffer(buff, clientid);
  }

  if (section & CFG_HLP_TINFO) {
    strcpy_P(buff, PSTR("\r\n===== TeleInfo\n"));
    OUT("Type (0x%02X)  : \n", config.tinfo.type);
    if (config.tinfo.type == CFG_TI_NONE)       strcat_P(buff, PSTR("Disabled"));
    if (config.tinfo.type & CFG_TI_EDF)   strcat_P(buff, PSTR("EDF Enabled"));

    // Send to correct client output
    outputBuffer(buff, clientid);
  }

  if (section & CFG_HLP_JEEDOM) {
    strcpy_P(buff, PSTR("\r\n===== Jeedom Server\n"));
    OUT("host : %s\n", config.jeedom.host);
    OUT("port : %d\n", config.jeedom.port);
    OUT("url  : %s\n", config.jeedom.url);
    OUT("key  : %s\n", config.jeedom.apikey);
    OUT("adco : %d\n", config.jeedom.adco);
    OUT("freq : %d\n", config.jeedom.freq);
    OUT("topic : %s\n", config.jeedom.topic);
    OUT("http  : %s\n", (config.jeedom.bmode & CFG_BMODE_HTTP ? "on" : "off"));
    OUT("mqtt  : %s\n", (config.jeedom.bmode & CFG_BMODE_MQTT ? "on" : "off"));

    // Send to correct client output
    outputBuffer(buff, clientid);
  }

  if (section & CFG_HLP_DOMZ) {
    strcpy_P(buff, PSTR("\r\n===== Domoticz Server\n"));
    OUT("host : %s\n", config.domz.host);
    OUT("port : %d\n", config.domz.port);
    OUT("url  : %s\n", config.domz.url);
    OUT("user : %s\n", config.domz.user);
    OUT("pass : %s\n", config.domz.pass);
    OUT("idx upt : %d\n", config.domz.idx_upt);
    OUT("idx rst : %d\n", config.domz.idx_rst);
    OUT("idx mcp3421 : %d\n", config.domz.idx_mcp3421);
    OUT("idx si7021 : %d\n", config.domz.idx_si7021);
    OUT("idx sht10 : %d\n", config.domz.idx_sht10);
    OUT("idx txt : %d\n", config.domz.idx_txt);
    OUT("idx p1sm : %d\n", config.domz.idx_p1sm);
    OUT("idx crt : %d\n", config.domz.idx_crt);
    OUT("idx elec : %d\n", config.domz.idx_elec);
    OUT("idx kwh : %d\n", config.domz.idx_kwh);
    OUT("idx pct : %d\n", config.domz.idx_pct);
    OUT("freq : %d\n", config.domz.freq);
    OUT("topic : %s\n", config.domz.topic);
    OUT("http  : %s\n", (config.domz.bmode & CFG_BMODE_HTTP ? "on" : "off"));
    OUT("mqtt  : %s\n", (config.domz.bmode & CFG_BMODE_MQTT ? "on" : "off"));

    // Send to correct client output
    outputBuffer(buff, clientid);
  }

  if (section & CFG_HLP_COUNTER) {
    strcpy_P(buff, PSTR("\r\n===== Counters\n"));
    sprintf_P(buff + strlen(buff), PSTR("1 io : %d\n1 dly: %d ms\n1 val: %lu\n"),
              config.counter.gpio1, config.counter.delay1, config.counter.value1);
    sprintf_P(buff + strlen(buff), PSTR("2 io : %d\n2 dly: %d ms\n2 val: %lu\n"),
              config.counter.gpio2, config.counter.delay2, config.counter.value2);

    // Send to correct client output
    outputBuffer(buff, clientid);
  }

  if (section & CFG_HLP_SENSOR) {
    strcpy_P(buff, PSTR("\r\n===== Sensors"));
    strcat_P(buff, PSTR("\nSI7021      : "));
    strcat_P(buff, config.sensors.en_si7021 ? PSTR("Enabled") : PSTR("Disabled"));
    strcat_P(buff, config.config & CFG_SI7021 ? PSTR(" (seen)") : PSTR(" (not found)"));

    strcat_P(buff, PSTR("\nSHT10       : "));
    strcat_P(buff, config.sensors.en_sht10 ? PSTR("Enabled") : PSTR("Disabled"));
    strcat_P(buff, config.config & CFG_SHT10 ? PSTR(" (seen)") : PSTR(" (not found)"));
    OUT(" CLK=%d",   SHT1x_CLOCK_PIN);
    OUT(" DAT=%d\n", SHT1x_DATA_PIN);

    strcat_P(buff, PSTR("\nMCP3421     : "));
    strcat_P(buff, config.sensors.en_mcp3421 ? PSTR("Enabled") : PSTR("Disabled"));
    strcat_P(buff, config.config & CFG_MCP3421 ? PSTR(" (seen)") : PSTR(" (not found)"));

    // Send to correct client output
    outputBuffer(buff, clientid);

    strcpy(buff, "");
    OUT("reading freq: %d\n", config.sensors.freq);
    OUT("temperature : %d",   config.sensors.temp_min_warn);
    OUT(" <---OK---> %d\n",   config.sensors.temp_max_warn);
    OUT("temperature : %d",   config.sensors.hum_min_warn);
    OUT(" <---OK---> %d\n",   config.sensors.hum_max_warn);
    OUT("Power       : %d",   config.sensors.pwr_min_warn);
    OUT(" <--<-->--> %d\n",   config.sensors.pwr_max_warn);

    // Send to correct client output
    outputBuffer(buff, clientid);
  }

}

/* ======================================================================
  Function: showHelp
  Purpose : display help
  Input   : bits flag of section to show
          WebSocket client ID if want to send to WS
  Output  : -
  Comments: -
  ====================================================================== */
void showHelp(uint16_t section, uint32_t clientid)
{
  if (clientid) {
    ws.text(clientid, FPSTR(HELP_HELP));
    if (section & CFG_HLP_SYS)      ws.text(clientid, FPSTR(HELP_SYS));
    if (section & CFG_HLP_WIFI)     ws.text(clientid, FPSTR(HELP_WIFI));
    if (section & CFG_HLP_MQTT)     ws.text(clientid, FPSTR(HELP_MQTT));
    if (section & CFG_HLP_DATA)     ws.text(clientid, FPSTR(HELP_DATA));
    if (section & CFG_HLP_TINFO)    ws.text(clientid, FPSTR(HELP_TINFO));
    if (section & CFG_HLP_JEEDOM)   ws.text(clientid, FPSTR(HELP_JEEDOM));
    if (section & CFG_HLP_DOMZ)     ws.text(clientid, FPSTR(HELP_DOMZ));
    if (section & CFG_HLP_COUNTER)  ws.text(clientid, FPSTR(HELP_COUNTER));
    if (section & CFG_HLP_SENSOR)   ws.text(clientid, FPSTR(HELP_SENSOR));
  } else {
    DEBUG_SERIAL.print(FPSTR(HELP_HELP));
    if (section & CFG_HLP_SYS)      DEBUG_SERIAL.print(FPSTR(HELP_SYS));
    if (section & CFG_HLP_WIFI)     DEBUG_SERIAL.print(FPSTR(HELP_WIFI));
    if (section & CFG_HLP_MQTT)     DEBUG_SERIAL.print(FPSTR(HELP_MQTT));
    if (section & CFG_HLP_DATA)     DEBUG_SERIAL.print(FPSTR(HELP_DATA));
    if (section & CFG_HLP_TINFO)    DEBUG_SERIAL.print(FPSTR(HELP_TINFO));
    if (section & CFG_HLP_JEEDOM)   DEBUG_SERIAL.print(FPSTR(HELP_JEEDOM));
    if (section & CFG_HLP_DOMZ)     DEBUG_SERIAL.print(FPSTR(HELP_DOMZ));
    if (section & CFG_HLP_COUNTER)  DEBUG_SERIAL.print(FPSTR(HELP_COUNTER));
    if (section & CFG_HLP_SENSOR)   DEBUG_SERIAL.print(FPSTR(HELP_SENSOR));
  }
}

void catParam(char * dest, uint8_t maxsize, char * par1, char * par2 , char * par3 )
{
  if (par1) {
    strncpy(dest, par1, maxsize);

    if (par2) {
      maxsize = maxsize - strlen(dest) - 1;
      strcat(dest, "_");
      strncat(dest, par2, maxsize);

      if (par3) {
        maxsize = maxsize - strlen(par2) - 1;
        strcat(dest, "_");
        strncat(dest, par3, maxsize);
      }
    }
    // No param, clear field
  } else {
    *dest = '\0';
  }

}

/* ======================================================================
  Function: execCmd
  Purpose : execute a configuration command
  Input   : command line
  Output  : -
  Comments: command can be sent by serial, websocket or by form post
  ====================================================================== */
void execCmd(char *line, uint32_t clientid)
{
  char sep[4] = " _,"; // Parse with space, underscore and coma
  char * cmd = NULL;
  char * par1 = NULL;
  char * par2 = NULL;
  char * par3 = NULL;
  char * par4 = NULL;

  // get the command and params
  if ( (cmd = strtok(line, sep)) != NULL ) {
    if ( (par1 = strtok(NULL, sep)) != NULL ) {
      if ( (par2 = strtok(NULL, sep)) != NULL ) {
        if ( (par3 = strtok(NULL, sep)) != NULL ) {
          if ( (par4 = strtok(NULL, sep)) != NULL ) {
          }
        }
      }
    }
  }

  Debugf(">'%s', 1:'%s', 2:'%s', 3:'%s', 4:'%s'\r\n", cmd, par1 ? par1 : "null", par2 ? par2 : "null", par3 ? par3 : "null", par4 ? par4 : "null");

  // Error ? show help
  if (!cmd || !par1) {
    showHelp(CFG_HLP_HELP, clientid);
  } else {
    IPAddress ip_addr;

    // Show command
    if (!strcasecmp_P(cmd, PSTR("show"))) {

      // Show config or show help
      if ( !strcasecmp_P(par1, PSTR("config")) || !strcasecmp_P(par1, PSTR("help")) ) {
        uint16_t cfg_hlp = CFG_HLP_ALL;
        if (par2) {
          if (!strcasecmp_P(par2, PSTR("sys"))) {
            cfg_hlp = CFG_HLP_SYS ;
          } else if (!strcasecmp_P(par2, PSTR("wifi"))) {
            cfg_hlp = CFG_HLP_WIFI;
          } else if (!strcasecmp_P(par2, PSTR("mqtt"))) {
            cfg_hlp = CFG_HLP_MQTT;
          } else if (!strcasecmp_P(par2, PSTR("data"))) {
            cfg_hlp = CFG_HLP_DATA;
          } else if (!strcasecmp_P(par2, PSTR("tinfo"))) {
            cfg_hlp = CFG_HLP_TINFO;
          } else if (!strcasecmp_P(par2, PSTR("sens"))) {
            cfg_hlp = CFG_HLP_SENSOR;
          } else if (!strcasecmp_P(par2, PSTR("jdom"))) {
            cfg_hlp = CFG_HLP_JEEDOM;
          } else if (!strcasecmp_P(par2, PSTR("domz"))) {
            cfg_hlp = CFG_HLP_DOMZ;
          } else if (!strcasecmp_P(par2, PSTR("cnt"))) {
            cfg_hlp = CFG_HLP_COUNTER;
          }
        } else {

        }

        if (!strcasecmp_P(par1, PSTR("config"))) {
          showConfig(cfg_hlp, clientid);
        } else {
          showHelp(cfg_hlp, clientid);
        }
      }

      // Save command
      //} else if(!strcasecmp(cmd, CFG_SAVE) ) {
    } else if (!strcasecmp_P(cmd, CFG_SAVE) ) {
      saveConfig();

      // Reset command
    } else if (!strcasecmp_P(cmd, PSTR("reset")) ) {

      if (!strcasecmp_P(par1, PSTR("sdk"))) {
        ESP.eraseConfig(); // Delete SDK Config (Wifi Credentials)
      } else if (!strcasecmp_P(par1, PSTR("config"))) {
        resetConfig();
      } else if (!strcasecmp_P(par1, PSTR("board"))) {
        resetBoard();
      }

      // Wifi command
    } else if (!strcasecmp_P(cmd, CFG_SSID)) {
      catParam(config.ssid, CFG_SSID_SIZE, par1, par2, par3);
    } else if (!strcasecmp_P(cmd, CFG_PSK )) {
      catParam(config.psk, CFG_PSK_SIZE, par1, par2, par3);
    } else if (!strcasecmp_P(cmd, CFG_WMW )) {
      config.wmw = ( atoi(par1) > 5 && atoi(par1) < 60 ? atoi(par1) : CFG_DEFAULT_WMW);
    } else if (!strcasecmp_P(cmd, CFG_HOST)) {
      catParam(config.host, CFG_HOSTNAME_SIZE, par1, par2, par3);
    } else if (!strcasecmp_P(cmd, CFG_AP_PSK)) {
      catParam(config.ap_psk, CFG_PSK_SIZE, par1, par2, par3);

    } else if (!strcasecmp_P(cmd, CFG_AP_SSID)) {
      catParam(config.ap_ssid, CFG_SSID_SIZE, par1, par2, par3);
    } else if (!strcasecmp_P(cmd, CFG_IP) && ip_addr.fromString(par1) ) {
      config.ip = ip_addr;
    } else if (!strcasecmp_P(cmd, CFG_MASK) && ip_addr.fromString(par1) ) {
      config.mask = ip_addr;
    } else if (!strcasecmp_P(cmd, CFG_GATEWAY) && ip_addr.fromString(par1) ) {
      config.gw = ip_addr;
    } else if (!strcasecmp_P(cmd, CFG_DNS) && ip_addr.fromString(par1) ) {
      config.dns = ip_addr;
    }

    // We had a 2nd parameters?
    if ( par2 ) {
      unsigned long v = atol(par2);

      // tinfo command
      if (!strcasecmp_P(cmd, PSTR("tinfo")) ) {
        Debugf("Cmd='tinfo_','%s','%s'\r\n", par1, par2);
        if (!strcasecmp_P(par1, &CFG_TINFO_EDF[6])) {
          uint8_t o1_msk = 0x00; // Future bits to set
          uint8_t a1_msk = 0xFF; // Future bits to clear

          if (!strcasecmp(par2, "on") ) {
            o1_msk |= CFG_TI_EDF;
          } else if (!strcasecmp(par2, "off") ) {
            a1_msk &= ~CFG_TI_EDF;
          }

          config.tinfo.type |= o1_msk; // Set needed bits
          config.tinfo.type &= a1_msk; // clear needed bits

          uint32_t o_msk = 0x0000; // Future bits to set
          uint32_t a_msk = 0xFFFF; // Future bits to clear

          if (!(config.tinfo.type == CFG_TI_NONE))
          {
            o_msk |= CFG_TINFO;
          }
          else
          {
            a_msk &= ~CFG_TINFO;
          }

          config.config |= o_msk; // Set needed bits
          config.config &= a_msk; // clear needed bits
        }
      }

      // ota command
      if (!strcasecmp_P(cmd, PSTR("ota")) ) {
        Debugf("Cmd='ota_','%s','%s'\r\n", par1, par2);
        if (!strcasecmp_P(par1, &CFG_OTA_AUTH[4])) {
          catParam(config.ota_auth, CFG_PSK_SIZE, par2, par3, par4);
        } else if (!strcasecmp_P(par1, &CFG_OTA_PORT[4] )) {
          config.ota_port = (v >= 0 && v <= 65535) ? v : DEFAULT_OTA_PORT;
        }

        // http command
      } else if (!strcasecmp_P(cmd, PSTR("http")) ) {
        Debugf("Cmd='http_','%s','%s'\r\n", par1, par2);
        if (!strcasecmp_P(par1, &CFG_HTTP_USR[5])) {
          catParam(config.http_usr, CFG_USER_SIZE, par2, par3, par4);
        } else if (!strcasecmp_P(par1, &CFG_HTTP_PWD[5] )) {
          catParam(config.http_pwd, CFG_PASS_SIZE, par2, par3, par4);
        }


        // mqtt command
      } else if (!strcasecmp_P(cmd, PSTR("mqtt")) ) {
        Debugf("Cmd='mqtt_','%s','%s'\r\n", par1, par2);
        if (!strcasecmp_P(par1, &CFG_MQTT_HOST[5])) {
          catParam(config.mqtt.host, CFG_MQTT_HOST_SIZE, par2, par3, par4);
        } else if (!strcasecmp_P(par1, &CFG_MQTT_PORT[5] )) {
          config.mqtt.port = (v >= 0 && v <= 65535) ? v : CFG_MQTT_DEFAULT_PORT ;
        } else if (!strcasecmp_P(par1, &CFG_MQTT_USR[5])) {
          catParam(config.mqtt.usr, CFG_MQTT_USER_SIZE, par2, par3, par4);
        } else if (!strcasecmp_P(par1, &CFG_MQTT_PWD[5])) {
          catParam(config.mqtt.pwd, CFG_MQTT_PASS_SIZE, par2, par3, par4);
        }

        // emon command
      } else if (!strcasecmp_P(cmd, PSTR("emon")) ) {
        Debugf("Cmd='emon_','%s','%s'\r\n", par1, par2);
        if (!strcasecmp_P(par1, &CFG_EMON_HOST[5])) {
          catParam(config.emoncms.host, CFG_EMON_HOST_SIZE, par2, par3, par4);
        } else if (!strcasecmp_P(par1, &CFG_EMON_PORT[5] )) {
          config.emoncms.port = (v >= 0 && v <= 65535) ? v : CFG_EMON_DEFAULT_PORT ;
        } else if (!strcasecmp_P(par1, &CFG_EMON_URL[5])) {
          catParam(config.emoncms.url, CFG_EMON_URL_SIZE, par2, par3, par4);
        } else if (!strcasecmp_P(par1, &CFG_EMON_KEY[5])) {
          catParam(config.emoncms.apikey, CFG_EMON_APIKEY_SIZE, par2, par3, par4);
        } else if (!strcasecmp_P(par1, &CFG_EMON_TOPIC[5])) {
          catParam(config.emoncms.topic, CFG_MQTT_TOPIC_SIZE, par2, par3, par4);
        } else if (!strcasecmp_P(par1, &CFG_EMON_HTTP[5])) {
          uint8_t o1_msk = 0x00; // Future bits to set
          uint8_t a1_msk = 0xFF; // Future bits to clear

          if (!strcasecmp(par2, "on") ) {
            o1_msk |= CFG_BMODE_HTTP;
          } else if (!strcasecmp(par2, "off") ) {
            a1_msk &= ~CFG_BMODE_HTTP;
          }

          config.emoncms.bmode |= o1_msk; // Set needed bits
          config.emoncms.bmode &= a1_msk; // clear needed bits
        } else if (!strcasecmp_P(par1, &CFG_EMON_MQTT[5])) {
          uint8_t o1_msk = 0x00; // Future bits to set
          uint8_t a1_msk = 0xFF; // Future bits to clear

          if (!strcasecmp(par2, "on") ) {
            o1_msk |= CFG_BMODE_MQTT;
          } else if (!strcasecmp(par2, "off") ) {
            a1_msk &= ~CFG_BMODE_MQTT;
          }

          config.emoncms.bmode |= o1_msk; // Set needed bits
          config.emoncms.bmode &= a1_msk; // clear needed bits
        } else if (!strcasecmp_P(par1, &CFG_EMON_NODE[5])) {
          config.emoncms.node = (v >= 0 && v <= 255) ? v : 0 ;
        } else if (!strcasecmp_P(par1, &CFG_EMON_FREQ[5])) {
          config.emoncms.freq = (v > 0 && v <= 86400) ? v : 0;
        }

        // jeedom command
      } else if (!strcasecmp_P(cmd, PSTR("jdom")) ) {
        Debugf("Cmd='jdom_','%s','%s'\r\n", par1, par2);
        if (!strcasecmp_P(par1, &CFG_JDOM_HOST[5])) {
          catParam(config.jeedom.host, CFG_JDOM_HOST_SIZE, par2, par3, par4);
        } else if (!strcasecmp_P(par1, &CFG_JDOM_PORT[5] )) {
          config.jeedom.port = (v >= 0 && v <= 65535) ? v : CFG_JDOM_DEFAULT_PORT ;
        } else if (!strcasecmp_P(par1, &CFG_JDOM_URL[5])) {
          catParam(config.jeedom.url, CFG_JDOM_URL_SIZE, par2, par3, par4);
        } else if (!strcasecmp_P(par1, &CFG_JDOM_TOPIC[5])) {
          catParam(config.jeedom.topic, CFG_MQTT_TOPIC_SIZE, par2, par3, par4);
        } else if (!strcasecmp_P(par1, &CFG_JDOM_HTTP[5])) {
          uint8_t o1_msk = 0x00; // Future bits to set
          uint8_t a1_msk = 0xFF; // Future bits to clear

          if (!strcasecmp(par2, "on") ) {
            o1_msk |= CFG_BMODE_HTTP;
          } else if (!strcasecmp(par2, "off") ) {
            a1_msk &= ~CFG_BMODE_HTTP;
          }

          config.jeedom.bmode |= o1_msk; // Set needed bits
          config.jeedom.bmode &= a1_msk; // clear needed bits
        } else if (!strcasecmp_P(par1, &CFG_JDOM_MQTT[5])) {
          uint8_t o1_msk = 0x00; // Future bits to set
          uint8_t a1_msk = 0xFF; // Future bits to clear

          if (!strcasecmp(par2, "on") ) {
            o1_msk |= CFG_BMODE_MQTT;
          } else if (!strcasecmp(par2, "off") ) {
            a1_msk &= ~CFG_BMODE_MQTT;
          }

          config.jeedom.bmode |= o1_msk; // Set needed bits
          config.jeedom.bmode &= a1_msk; // clear needed bits
        } else if (!strcasecmp_P(par1, &CFG_JDOM_KEY[5])) {
          catParam(config.jeedom.apikey, CFG_JDOM_APIKEY_SIZE, par2, par3, par4);
        } else if (!strcasecmp_P(par1, &CFG_JDOM_ADCO[5])) {
          catParam(config.jeedom.adco, CFG_ADCO_SIZE, par2, par3, par4);
        } else if (!strcasecmp_P(par1, &CFG_JDOM_FREQ[5])) {
          config.jeedom.freq = (v > 0 && v <= 86400) ? v : 0;
        }

        // domoticz command
      } else if (!strcasecmp_P(cmd, PSTR("domz")) ) {
        Debugf("Cmd='domz_','%s','%s'\r\n", par1, par2);
        if (!strcasecmp_P(par1, &CFG_DOMZ_HOST[5])) {
          catParam(config.domz.host, CFG_DOMZ_HOST_SIZE, par2, par3, par4);
        } else if (!strcasecmp_P(par1, &CFG_DOMZ_PORT[5] )) {
          config.domz.port = (v >= 0 && v <= 65535) ? v : CFG_DOMZ_DEFAULT_PORT ;
          /*        } else if (!strcasecmp_P(par1, &CFG_DOMZ_INDEX[5] )) {
                    config.domz.index = (v>=0 && v<=65535) ? v : 0 ;*/
        } else if (!strcasecmp_P(par1, PSTR("idx") )) {
          if (par3)
          {
            unsigned long i = atol(par3);

            if (!strcasecmp_P(par2, &CFG_DOMZ_IDX_TXT[9] )) {
              config.domz.idx_txt = (i >= 0 && i <= 65535) ? i : 0 ;
            }
            else if (!strcasecmp_P(par2, &CFG_DOMZ_IDX_UPT[9] ))
            {
              config.domz.idx_upt = (i >= 0 && i <= 65535) ? i : 0 ;
            }
            else if (!strcasecmp_P(par2, &CFG_DOMZ_IDX_RST[9] ))
            {
              config.domz.idx_rst = (i >= 0 && i <= 65535) ? i : 0 ;
            }
            else if (!strcasecmp_P(par2, &CFG_DOMZ_IDX_MCP3421[9] ))
            {
              config.domz.idx_mcp3421 = (i >= 0 && i <= 65535) ? i : 0 ;
            }
            else if (!strcasecmp_P(par2, &CFG_DOMZ_IDX_SI7021[9] ))
            {
              config.domz.idx_si7021 = (i >= 0 && i <= 65535) ? i : 0 ;
            }
            else if (!strcasecmp_P(par2, &CFG_DOMZ_IDX_SHT10[9] ))
            {
              config.domz.idx_sht10 = (i >= 0 && i <= 65535) ? i : 0 ;
            }
            else if (!strcasecmp_P(par2, &CFG_DOMZ_IDX_P1SM[9] ))
            {
              config.domz.idx_p1sm = (i >= 0 && i <= 65535) ? i : 0 ;
            }
            else if (!strcasecmp_P(par2, &CFG_DOMZ_IDX_CRT[9] ))
            {
              config.domz.idx_crt = (i >= 0 && i <= 65535) ? i : 0 ;
            }
            else if (!strcasecmp_P(par2, &CFG_DOMZ_IDX_ELEC[9] ))
            {
              config.domz.idx_elec = (i >= 0 && i <= 65535) ? i : 0 ;
            }
            else if (!strcasecmp_P(par2, &CFG_DOMZ_IDX_KWH[9] ))
            {
              config.domz.idx_kwh = (i >= 0 && i <= 65535) ? i : 0 ;
            }
            else if (!strcasecmp_P(par2, &CFG_DOMZ_IDX_PCT[9] ))
            {
              config.domz.idx_pct = (i >= 0 && i <= 65535) ? i : 0 ;
            }
          }
        } else if (!strcasecmp_P(par1, &CFG_DOMZ_URL[5])) {
          catParam(config.domz.url, CFG_DOMZ_URL_SIZE, par2, par3, par4);
        } else if (!strcasecmp_P(par1, &CFG_DOMZ_TOPIC[5])) {
          catParam(config.domz.topic, CFG_MQTT_TOPIC_SIZE, par2, par3, par4);
        } else if (!strcasecmp_P(par1, &CFG_DOMZ_HTTP[5])) {
          uint8_t o1_msk = 0x00; // Future bits to set
          uint8_t a1_msk = 0xFF; // Future bits to clear

          if (!strcasecmp(par2, "on") ) {
            o1_msk |= CFG_BMODE_HTTP;
          } else if (!strcasecmp(par2, "off") ) {
            a1_msk &= ~CFG_BMODE_HTTP;
          }

          config.domz.bmode |= o1_msk; // Set needed bits
          config.domz.bmode &= a1_msk; // clear needed bits
        } else if (!strcasecmp_P(par1, &CFG_DOMZ_MQTT[5])) {
          uint8_t o1_msk = 0x00; // Future bits to set
          uint8_t a1_msk = 0xFF; // Future bits to clear

          if (!strcasecmp(par2, "on") ) {
            o1_msk |= CFG_BMODE_MQTT;
          } else if (!strcasecmp(par2, "off") ) {
            a1_msk &= ~CFG_BMODE_MQTT;
          }

          config.domz.bmode |= o1_msk; // Set needed bits
          config.domz.bmode &= a1_msk; // clear needed bits
        } else if (!strcasecmp_P(par1, &CFG_DOMZ_USER[5])) {
          catParam(config.domz.user, CFG_USER_SIZE, par2, par3, par4);
        } else if (!strcasecmp_P(par1, &CFG_DOMZ_PASS[5])) {
          catParam(config.domz.pass, CFG_PASS_SIZE, par2, par3, par4);
        } else if (!strcasecmp_P(par1, &CFG_DOMZ_FREQ[5])) {
          config.domz.freq = (v > 0 && v <= 86400) ? v : 0;
        }

        // cfg command
      } else  if (!strcasecmp_P(cmd, PSTR("cfg")) ) {
        uint32_t o_msk = 0x0000; // Future bits to set
        uint32_t a_msk = 0xFFFF; // Future bits to clear

        Debugf("Cmd='cfg_','%s','%s','%s'\r\n", par1 ? par1 : "null", par2 ? par2 : "null", par3 ? par3 : "null");

        if (par2) {
          uint8_t val = !strcasecmp(par2, "on") ? 1 : 0;

          if (!strcasecmp_P(par1, &CFG_CFG_AP[4])) {
            if (val) o_msk |= CFG_AP; else a_msk &= ~CFG_AP;
          } else if (!strcasecmp_P(par1, &CFG_CFG_WIFI[4] )) {
            if (val) o_msk |= CFG_WIFI; else a_msk &= ~CFG_WIFI;
          } else if (!strcasecmp_P(par1, &CFG_CFG_RGBLED[4] )) {
            if (val) o_msk |= CFG_RGB_LED; else a_msk &= ~CFG_RGB_LED;
          } else if (!strcasecmp_P(par1, &CFG_CFG_DEBUG[4] )) {
            if (val) o_msk |= CFG_DEBUG; else a_msk &= ~CFG_DEBUG;
          } else if (!strcasecmp_P(par1, &CFG_CFG_MQTT[4] )) {
            if (val) o_msk |= CFG_MQTT; else a_msk &= ~CFG_MQTT;
          } else if (!strcasecmp_P(par1, &CFG_CFG_LOGGER[4] )) {
            if (val) o_msk |= CFG_LOGGER; else a_msk &= ~CFG_LOGGER;
          } else if (!strcasecmp_P(par1, &CFG_CFG_DEMO[4] )) {
            if (val) o_msk |= CFG_DEMO_MODE; else a_msk &= ~CFG_DEMO_MODE;
          } else if (!strcasecmp_P(par1, &CFG_CFG_OLED[4] )) {
            if (val) o_msk |= CFG_LCD; else a_msk &= ~CFG_LCD;
          } else if (!strcasecmp_P(par1, &CFG_CFG_STATIC[4] )) {
            if (val) o_msk |= CFG_STATIC; else a_msk &= ~CFG_STATIC;
          }

          if (!(config.tinfo.type == CFG_TI_NONE))
          {
            o_msk |= CFG_TINFO;
          }
          else
          {
            a_msk &= ~CFG_TINFO;
          }

          config.config |= o_msk; // Set needed bits
          config.config &= a_msk; // clear needed bits

          if ( !strcasecmp(par1, "led") && par3) {
            val = atoi(par3);

            if (!strcasecmp_P(par2, &CFG_LED_BRIGHTNESS[8])) {
              config.led_bright = (val >= 0 && val <= 100) ? val : DEFAULT_LED_BRIGHTNESS;
            } else if (!strcasecmp_P(par2, &CFG_LED_HEARTBEAT[8] )) {
              config.led_hb = (val >= 0 && val <= 50) ? val : DEFAULT_LED_HEARTBEAT;
            } else if (!strcasecmp_P(par2, &CFG_LED_GPIO[8] )) {
              config.led_gpio = (val >= 0 && val <= 16) ? val : DEFAULT_LED_GPIO;
              task_reconf = true;
            } else if (!strcasecmp_P(par2, &CFG_LED_NUM[8] )) {
              config.led_num = (val >= 0 && val <= 128) ? val : DEFAULT_LED_NUM;
              task_reconf = true;
            } else if (!strcasecmp_P(par2, &CFG_LED_TYPE[8] )) {
              config.led_type = (val >= NeoPixelType_None && val < NeoPixelType_End) ? val : DEFAULT_LED_TYPE;
              task_reconf = true;
            }

          }
        } // par2

        // counter command
      } else if (!strcasecmp_P(cmd, PSTR("cnt")) ) {
        uint16_t val;

        Debugf("Cmd='cnt_','%s','%s','%s'\r\n", par1 ? par1 : "null", par2 ? par2 : "null", par3 ? par3 : "null");

        if (par2 && par3) {
          uint8_t cnt = '0' - *par2;
          unsigned long val = atol(par3);

          if (!strcasecmp_P(par1, &CFG_COUNT1_GPIO[4] )) {
            if (val >= 1 && val <= 16) {
              if (cnt == 1) config.counter.gpio1 = val;
              if (cnt == 2) config.counter.gpio2 = val;
            }
          } else if (!strcasecmp_P(par1, &CFG_COUNT1_DELAY[4] )) {
            if (val >= 0 && val <= 100000) {
              if (cnt == 1) config.counter.delay1 = val;
              if (cnt == 2) config.counter.delay2 = val;
            }
          } else if (!strcasecmp_P(par1, &CFG_COUNT1_VALUE[4] )) {
            if (cnt == 1) config.counter.value1 = val;
            if (cnt == 2) config.counter.value2 = val;
          }
        } // par2 && par3

        // sensor command
      } else if (!strcasecmp_P(cmd, PSTR("sens")) ) {
        uint16_t val;

        Debugf("Cmd='sens_','%s','%s','%s','%s'\r\n", par1 ? par1 : "null", par2 ? par2 : "null", par3 ? par3 : "null", par4 ? par4 : "null");

        if (par2) {
          if (!strcasecmp(par2, "on") ) {
            val = 1;
          } else if (!strcasecmp(par2, "off") ) {
            val = 0;
          } else {
            val = atoi(par2);
          }

          if (!strcasecmp_P(par1, &CFG_SENS_SI7021[5] )) {
            config.sensors.en_si7021 = val;
          } else if (!strcasecmp_P(par1, &CFG_SENS_SHT10[5] )) {
            config.sensors.en_sht10 = val;
          } else if (!strcasecmp_P(par1, &CFG_SENS_MCP3421[5] )) {
            config.sensors.en_mcp3421 = val;
          } else if (!strcasecmp_P(par1, &CFG_SENS_FREQ[5] )) {
            config.sensors.freq = val;
            if ( val > 0 && val <= 86400 ) {
              task_sensors = true;
            }
          }
        } // par2

        if (par3 && par4) {
          int16_t min = atoi(par3);
          int16_t max = atoi(par4);

          if (!strcasecmp(par1, "temp" )) {
            if (!strcasecmp_P(par2, &CFG_SENS_TEMP_LED[10] )) {
              config.sensors.temp_min_warn = min;
              config.sensors.temp_max_warn = max;
            }
          } else if (!strcasecmp(par1, "hum" )) {
            if (!strcasecmp_P(par2, &CFG_SENS_HUM_LED[9] )) {
              config.sensors.hum_min_warn = min;
              config.sensors.hum_max_warn = max;
            }
          } else if (!strcasecmp(par1, "pwr" )) {
            if (!strcasecmp_P(par2, &CFG_SENS_PWR_LED[9] )) {
              config.sensors.pwr_min_warn = min;
              config.sensors.pwr_max_warn = max;
            }
          }
        } // par3 && par4

      } // sensor command

    } // par2
  } // Cmd && Par1
}

/* ======================================================================
  Function: handle_serial
  Purpose : manage serial command typed on keyboard
  Input   : command line if coming from web socket else NULL
          client ID if coming from web socket else 0
  Output  : -
  Comments: -
  ====================================================================== */
void handle_serial(char * line, uint32_t clientid)
{
  // received full line from Web Sockets
  if (clientid) {
    execCmd(line, clientid);
  } else {
#ifdef SERIAL_CMD
    // Real Serial
    char c;
    uint8_t nb_char = 0;
    boolean reset_buf = false;

    // We take only 1 char per loop
    if (DEBUG_SERIAL.available()) {
      c = DEBUG_SERIAL.read();
      nb_char++;
      // Space in Buffer
      if ( ser_idx < CFG_SERIAL_BUFFER_SIZE && c != '\r') {
        if ( c == '\n') { // End of command
          ser_buf[ser_idx] = '\0'; // end of buffer
          execCmd(ser_buf);
          reset_buf = true;
        } else { // Discard unknown char
          ser_buf[ser_idx++] = c ;
        }
      } else {
        // Too long discard and reset buffer
        reset_buf = true;
      }

      // discard and reset buffer ?
      if (reset_buf) {
        ser_idx = 0;
        memset(ser_buf, 0, sizeof(ser_buf)); // clear buffer
      }
    } // While char
#endif
  }
}
