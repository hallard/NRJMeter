// **********************************************************************************
// ESP8266 NRJMeter WEB Client, web server function
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
// History : V1.00 2015-12-04 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#include "NRJMeter.h"

/* ======================================================================
  Function: httpPost
  Purpose : Do a http post
  Input   : hostname
          port
          url
  Output  : true if received 200 OK
  Comments: -
  ====================================================================== */
boolean httpPost(char * host, uint16_t port, char * url)
{
  boolean ret = false;

  if ( WiFi.status() == WL_CONNECTED && *host) {

    WiFiClient client;
    HTTPClient http;

    unsigned long start = millis();

    String _url ="";

    if (port == 443) {
      _url ="https://" + String(host) ;
    } else {
      _url ="http://" + String(host) + ":" + String(port);
    }
    _url += "/" + String(url);

    // Post should not take more than 2.5 sec
    http.setTimeout(2500);

    // configure targed server and url
    http.begin(client, _url);

    Debugf("http%s://%s:%d%s => ", port == 443 ? "s" : "", host, port, url);

    // start connection and send HTTP header
    int httpCode = http.GET();
    if ( httpCode ) {
      // HTTP header has been send and Server response header has been handled
      Debug(httpCode);
      Debug(" ");

      // Request OK (emoncms returns 200)
      if (httpCode == HTTP_CODE_OK ) {
        Debug(http.getString());
        ret = true;
      }
    } else {
      DebugF("failed!");
    }

    http.end();
    Debugf(" in %ld ms\r\n", millis() - start);
  } else {
    DebuglnF("Wifi not connected!");
  }

  return ret;
}

/* ======================================================================
  Function: jeedomPost
  Purpose : Do a http post to jeedom server
  Input   :
  Output  : true if post returned 200 OK
  Comments: -
  ====================================================================== */
boolean jeedomPost(void)
{
  boolean ret = false;
  char buff[128];
  String json = "";

  String url ;

  url = *config.jeedom.url ? config.jeedom.url : "/";
  url += "?";

  // Config identifiant forcée ?
  if (*config.jeedom.adco) {
    url += F("ADCO=");
    url += config.jeedom.adco;
    url += "&";
  }

  url += F("api=") ;
  url += config.jeedom.apikey;
  url += F("&") ;

  sprintf_P(buff, PSTR("uptime=%ld&"), seconds) ;
  url += buff;

  //sprintf_P(buff, PSTR("reset_cause=%d&"), ESP_getResetReason()) ;
  //url += buff;

  if (config.config & CFG_TINFO || config.config & CFG_DEMO_MODE)
  {
    //Ensure we get last TInfo values
    updateTInfo();

    std::map<std::string, std::string>::iterator iterator;

    for (iterator = tinfo_values.begin(); iterator != tinfo_values.end(); iterator++) {
      std::string k = iterator->first;
      std::string v = iterator->second;
      if (strcmp(k.c_str(), "ADCO") == 0 && *config.jeedom.adco) {
        //ADCO overrided in config
      }
      else
      {
        sprintf_P(buff, PSTR("%s=%s&"), k.c_str(), v.c_str());
        url += buff;
      }
    }
  }

  // Does MCP3421 is enabled and seen
  if (config.sensors.en_mcp3421 && (config.config & CFG_MCP3421 || config.config & CFG_DEMO_MODE) ) {
    sprintf_P(buff, PSTR("mcp3421=%s&"), String(mcp3421_power).c_str());
    url += buff;
  }

  // Does SI7021 is enabled and seen
  if (config.sensors.en_si7021 && (config.config & CFG_SI7021 || config.config & CFG_DEMO_MODE) ) {
    sprintf_P(buff, PSTR("si7021_temp=%s&si7021_hum=%s&"), String(si7021_temperature / 100.0).c_str(), String(si7021_humidity / 100.0).c_str());
    url += buff;
  }

  // Does SHT10 is enabled and seen
  if (config.sensors.en_sht10 && (config.config & CFG_SHT10 || config.config & CFG_DEMO_MODE) ) {
    sprintf_P(buff, PSTR("sht10_temp=%s&sht10_hum=%s&sht10_hum_nc=%s&"),
              String(sht1x_temperature / 100.0).c_str(),
              String(sht1x_humidity / 100.0).c_str(),
              String(sht1x_humidity_nc / 100.0).c_str()
             );
    url += buff;
  }

  if (config.jeedom.bmode & CFG_BMODE_HTTP)
  {
    ret = httpPost( config.jeedom.host, config.jeedom.port, (char *) url.c_str()) ;
  }

  if (config.jeedom.bmode & CFG_BMODE_MQTT)
  {
    //TODO: To be checked if it's correct MQTT frame for jeedom
    json += "not yet implemented";
    ret = mqttPost(config.jeedom.topic, json.c_str());
  }

  return ret;
}

/* ======================================================================
  Function: emoncmsPost
  Purpose : Do a http post to emoncms
  Input   :
  Output  : true if post returned 200 OK
  Comments: -
  ====================================================================== */
boolean emoncmsPost(void)
{
  boolean ret = false;
  char buff[128];

  String url ;
  String json = "";

  url = *config.emoncms.url ? config.emoncms.url : "/";

  sprintf_P(buff, PSTR("?node=%d&apikey=%s&json="), config.emoncms.node, config.emoncms.apikey) ;
  url += buff;
  sprintf_P(buff, PSTR("{uptime:%ld"), seconds) ;
  url += buff;
  json += buff;

  if (config.config & CFG_TINFO || config.config & CFG_DEMO_MODE)
  {
    //Ensure we get last TInfo values
    updateTInfo();

    std::map<std::string, std::string>::iterator iterator;

    for (iterator = tinfo_values.begin(); iterator != tinfo_values.end(); iterator++) {
      std::string k = iterator->first;
      std::string v = iterator->second;
      sprintf_P(buff, PSTR(",%s:%s"), k.c_str(), v.c_str());
      url += buff;
      json += buff;
    }
  }

  if ((config.sensors.en_mcp3421 && config.config & CFG_MCP3421) || (config.sensors.en_si7021 && config.config & CFG_SI7021) || (config.sensors.en_sht10 && config.config & CFG_SHT10)  || config.config & CFG_DEMO_MODE)
  {
    sensors_measure();
  }

  // Does MCP3421 is enabled and seen
  if (config.sensors.en_mcp3421 && (config.config & CFG_MCP3421 || config.config & CFG_DEMO_MODE) ) {
    sprintf_P(buff, PSTR(",mcp3421:%s"), String(mcp3421_power).c_str());
    url += buff;
    json += buff;
  }

  // Does SI7021 is enabled and seen
  if (config.sensors.en_si7021 && (config.config & CFG_SI7021 || config.config & CFG_DEMO_MODE) ) {
    sprintf_P(buff, PSTR(",si7021_temp:%s,si7021_hum:%s"), String(si7021_temperature / 100.0).c_str(), String(si7021_humidity / 100.0).c_str());
    url += buff;
    json += buff;
  }

  // Does SHT10 is enabled and seen
  if (config.sensors.en_sht10 && (config.config & CFG_SHT10 || config.config & CFG_DEMO_MODE) ) {
    sprintf_P(buff, PSTR(",sht10_temp:%s,sht10_hum:%s,sht10_hum_nc:%s"),
              String(sht1x_temperature / 100.0).c_str(),
              String(sht1x_humidity / 100.0).c_str(),
              String(sht1x_humidity_nc / 100.0).c_str()
             );
    url += buff;
    json += buff;
  }

  sprintf_P(buff, PSTR(",reset_cause:%s}"), ESP_getResetReason().c_str()) ;
  url += buff;
  json += buff;

  if (config.emoncms.bmode & CFG_BMODE_HTTP)
  {
    ret = httpPost( config.emoncms.host, config.emoncms.port, (char *) url.c_str()) ;
  }

  if (config.emoncms.bmode & CFG_BMODE_MQTT)
  {
    //TODO: To be checked if it's correct MQTT frame for emoncms
    ret = mqttPost(config.emoncms.topic, json.c_str());
  }

  return ret;
}


/* ======================================================================
  Function: domoticzPost
  Purpose : Do a http post to domotiz server
  Input   :
  Output  : true if post returned 200 OK
  Comments: -
  ====================================================================== */
boolean domoticzPost(void)
{
  boolean ret = true;

  String baseurl;
  String url;

  baseurl = *config.domz.url ? config.domz.url : "/";

  ///Uptime
  // HTTP: /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=VALUE
  // MQTT: '{ "idx" : 1, "svalue" : "25" }'
  if (config.domz.idx_upt > 0)
  {
    if (config.domz.bmode & CFG_BMODE_HTTP)
    {
      url = baseurl;
      url += F("?type=command&param=udevice&");
      url += "idx=";
      url += config.domz.idx_upt;
      url += "&nvalue=0";
      url += "&svalue=";
      url += seconds;

      if (!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
      {
        ret = false;
      }
    }

    if (config.domz.bmode & CFG_BMODE_MQTT)
    {
      String jsonmqttStr;
      StaticJsonDocument<200> jsonmqtt;
      jsonmqtt["idx"] = config.domz.idx_upt;
      jsonmqtt["nvalue"] = 0;
      String svalue = "";
      svalue += seconds;
      jsonmqtt["svalue"] = svalue.c_str();

      serializeJson(jsonmqtt,jsonmqttStr);

      if (!mqttPost(config.domz.topic, jsonmqttStr.c_str()))
      {
        ret = false;
      }
    }
  }

  //Reset_Cause
  // HTTP: /json.htm?type=command&param=switchlight&idx=IDX&switchcmd=Set%20Level&level=LEVEL
  // MQTT: '{ "idx" : 1, switchcmd : "Set Level", "Level" : "6" }'
  //Rst cause No.  Cause                          GPIO state
  //0              Power reboot                   Changed
  //1              Hardware WDT reset             Changed
  //2              Fatal exception                Unchanged
  //3              Software watchdog reset        Unchanged
  //4              Software reset                 Unchanged
  //5              Deep-sleep                     Changed
  //6              Hardware reset                 Changed
  if (config.domz.idx_rst > 0)
  {
    if (config.domz.bmode & CFG_BMODE_HTTP)
    {
      url = baseurl;
      url += F("?type=command&param=switchlight&");
      url += "idx=";
      url += config.domz.idx_rst;
      url += "&switchcmd=Set%20Level&level=";
      //url += ESP_getResetReason()*10;

      if (!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
      {
        ret = false;
      }
    }

    if (config.domz.bmode & CFG_BMODE_MQTT)
    {
      String jsonmqttStr;
      StaticJsonDocument<200> jsonmqtt;
      jsonmqtt["command"] = "switchlight";
      jsonmqtt["idx"] = config.domz.idx_rst;
      jsonmqtt["switchcmd"] = "Set Level";
      //jsonmqtt["level"] = ESP_getResetReason() * 10;

      serializeJson(jsonmqtt, jsonmqttStr);

      if (!mqttPost(config.domz.topic, jsonmqttStr.c_str()))
      {
        ret = false;
      }
    }
  }

  if (config.config & CFG_TINFO || config.config & CFG_DEMO_MODE)
  {
    //Ensure we get last TInfo values
    updateTInfo();

    // HTTP: /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=TXT
    // MQTT: '{ "idx" : 1, "nvalue" : 0, "svalue" : "25.0" }'
    if (config.domz.idx_txt > 0 && tinfo_values.find("ADCO") != tinfo_values.end())
    {
      if (config.domz.bmode & CFG_BMODE_HTTP)
      {
        url = baseurl;
        url += F("?type=command&param=udevice&");
        url += "idx=";
        url += config.domz.idx_txt;
        url += "&nvalue=0";
        url += "&svalue=";
        url += tinfo_values["ADCO"].c_str();

        if (!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
        {
          ret = false;
        }
      }

      if (config.domz.bmode & CFG_BMODE_MQTT)
      {
        String jsonmqttStr;
        StaticJsonDocument<200> jsonmqtt;
        jsonmqtt["idx"] = config.domz.idx_txt;
        jsonmqtt["nvalue"] = 0;
        String svalue = "";
        svalue += tinfo_values["ADCO"].c_str();
        jsonmqtt["svalue"] = svalue.c_str();

        serializeJson(jsonmqtt, jsonmqttStr);

        if (!mqttPost(config.domz.topic, jsonmqttStr.c_str()))
        {
          ret = false;
        }
      }
    }

    // HTML: /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=USAGE1;USAGE2;RETURN1;RETURN2;CONS;PROD
    // MQTT: '{ "idx" : 1, "nvalue" : 0, "svalue" : "25.0" }'
    if (config.domz.idx_p1sm > 0 && tinfo_values.find("BASE") != tinfo_values.end() && tinfo_values.find("PAPP") != tinfo_values.end())
    {
      if (config.domz.bmode & CFG_BMODE_HTTP)
      {
        url = baseurl;
        url += F("?type=command&param=udevice&");
        url += "idx=";
        url += config.domz.idx_p1sm;
        url += "&nvalue=0";
        url += "&svalue=";
        url += String(atoi(tinfo_values["BASE"].c_str())).c_str();
        url += ";0;0;0;";
        url += String(atoi(tinfo_values["PAPP"].c_str())).c_str();
        url += ";0";

        if (!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
        {
          ret = false;
        }
      }

      if (config.domz.bmode & CFG_BMODE_MQTT)
      {
        String jsonmqttStr;
        StaticJsonDocument<200> jsonmqtt;
        jsonmqtt["idx"] = config.domz.idx_p1sm;
        jsonmqtt["nvalue"] = 0;
        String svalue = "";
        svalue += String(atoi(tinfo_values["BASE"].c_str())).c_str();
        svalue += ";0;0;0;";
        svalue += String(atoi(tinfo_values["PAPP"].c_str())).c_str();
        svalue += ";0";
        jsonmqtt["svalue"] = svalue.c_str();

        serializeJson(jsonmqtt, jsonmqttStr);

        if (!mqttPost(config.domz.topic, jsonmqttStr.c_str()))
        {
          ret = false;
        }
      }
    }

    // HTML: /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=ENERGY
    // MQTT: '{ "idx" : 1, "nvalue" : 0, "svalue" : "25.0" }'
    if (config.domz.idx_crt > 0 && tinfo_values.find("IINST") != tinfo_values.end())
    {
      if (config.domz.bmode & CFG_BMODE_HTTP)
      {
        url = baseurl;
        url += F("?type=command&param=udevice&");
        url += "idx=";
        url += config.domz.idx_crt;
        url += "&nvalue=0";
        url += "&svalue=";
        url += String(atoi(tinfo_values["IINST"].c_str())).c_str();

        if (!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
        {
          ret = false;
        }
      }

      if (config.domz.bmode & CFG_BMODE_MQTT)
      {
        String jsonmqttStr;
        StaticJsonDocument<200> jsonmqtt;
        
        jsonmqtt["idx"] = config.domz.idx_crt;
        jsonmqtt["nvalue"] = 0;
        String svalue = "";
        svalue += String(atoi(tinfo_values["IINST"].c_str())).c_str();
        jsonmqtt["svalue"] = svalue.c_str();

        serializeJson(jsonmqtt, jsonmqttStr);

        if (!mqttPost(config.domz.topic, jsonmqttStr.c_str()))
        {
          ret = false;
        }
      }
    }

    // HTTP: /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=ENERGY
    // MQTT: '{ "idx" : 1, "nvalue" : 0, "svalue" : "25.0" }'
    if (config.domz.idx_elec > 0 && tinfo_values.find("PAPP") != tinfo_values.end())
    {
      if (config.domz.bmode & CFG_BMODE_HTTP)
      {
        url = baseurl;
        url += F("?type=command&param=udevice&");
        url += "idx=";
        url += config.domz.idx_elec;
        url += "&nvalue=0";
        url += "&svalue=";
        url += String(atoi(tinfo_values["PAPP"].c_str())).c_str();

        if (!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
        {
          ret = false;
        }
      }

      if (config.domz.bmode & CFG_BMODE_MQTT)
      {
        String jsonmqttStr;
        StaticJsonDocument<200> jsonmqtt;
        
        jsonmqtt["idx"] = config.domz.idx_elec;
        jsonmqtt["nvalue"] = 0;
        String svalue = "";
        svalue += String(atoi(tinfo_values["PAPP"].c_str())).c_str();
        jsonmqtt["svalue"] = svalue.c_str();

        serializeJson(jsonmqtt, jsonmqttStr);

        if (!mqttPost(config.domz.topic, jsonmqttStr.c_str()))
        {
          ret = false;
        }
      }
    }

    // HTTP: /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=POWER,ENERGY
    // MQTT: '{ "idx" : 1, "nvalue" : 0, "svalue" : "25.0" }'
    if (config.domz.idx_kwh > 0 && tinfo_values.find("PAPP") != tinfo_values.end())
    {
      if (config.domz.bmode & CFG_BMODE_HTTP)
      {
        url = baseurl;
        url += F("?type=command&param=udevice&");
        url += "idx=";
        url += config.domz.idx_kwh;
        url += "&nvalue=0";
        url += "&svalue=";
        url += String(atoi(tinfo_values["PAPP"].c_str())).c_str();
        url += ";0";
        //url += String(atoi(tinfo_values["IINST"].c_str())).c_str(); Computed by Domoticz

        if (!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
        {
          ret = false;
        }
      }

      if (config.domz.bmode & CFG_BMODE_MQTT)
      {
        String jsonmqttStr;
        StaticJsonDocument<200> jsonmqtt;
        
        jsonmqtt["idx"] = config.domz.idx_kwh;
        jsonmqtt["nvalue"] = 0;
        String svalue = "";
        svalue += String(atoi(tinfo_values["PAPP"].c_str())).c_str();
        svalue += ";0";
        jsonmqtt["svalue"] = svalue.c_str();

        serializeJson(jsonmqtt, jsonmqttStr);
        


        if (!mqttPost(config.domz.topic, jsonmqttStr.c_str()))
        {
          ret = false;
        }
      }
    }

    // HTTP: /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=PERCENTAGE
    // MQTT: '{ "idx" : 1, "nvalue" : 0, "svalue" : "25.0" }'
    if (config.domz.idx_pct > 0 && tinfo_values.find("IINST") != tinfo_values.end() && tinfo_values.find("ISOUSC") != tinfo_values.end())
    {
      if (config.domz.bmode & CFG_BMODE_HTTP)
      {
        url = baseurl;
        url += F("?type=command&param=udevice&");
        url += "idx=";
        url += config.domz.idx_pct;
        url += "&nvalue=0";
        url += "&svalue=";
        url += String( roundf((atof(tinfo_values["IINST"].c_str()) * 100) / atof(tinfo_values["ISOUSC"].c_str()) * 100) / 100 ).c_str();

        if (!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
        {
          ret = false;
        }
      }

      if (config.domz.bmode & CFG_BMODE_MQTT)
      {
        String jsonmqttStr;
        StaticJsonDocument<200> jsonmqtt;
        
        jsonmqtt["idx"] = config.domz.idx_pct;
        jsonmqtt["nvalue"] = 0;
        String svalue = "";
        svalue += String( roundf((atof(tinfo_values["IINST"].c_str()) * 100) / atof(tinfo_values["ISOUSC"].c_str()) * 100) / 100 ).c_str();
        jsonmqtt["svalue"] = svalue.c_str();

        serializeJson(jsonmqtt, jsonmqttStr);

        if (!mqttPost(config.domz.topic, jsonmqttStr.c_str()))
        {
          ret = false;
        }
      }
    }
  }

  // Do a measurment
  sensors_measure();

  //If sensor is enabled and seen
  if (config.sensors.en_mcp3421 && config.config & CFG_MCP3421)
  {
    //json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=TEMP;HUM;HUM_STAT
    if (config.domz.bmode & CFG_BMODE_HTTP)
    {
      url = baseurl;
      url += F("?type=command&param=udevice&");
      url += "idx=";
      url += config.domz.idx_mcp3421;
      url += "&nvalue=0";
      url += "&svalue=";
      url += String(mcp3421_power).c_str();

      if (!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
      {
        ret = false;
      }
    }

    if (config.domz.bmode & CFG_BMODE_MQTT)
    {
      String jsonmqttStr;
      StaticJsonDocument<200> jsonmqtt;
      
      jsonmqtt["idx"] = config.domz.idx_mcp3421;
      jsonmqtt["nvalue"] = 0;
      String svalue = "";
      svalue += String(mcp3421_power).c_str();
      jsonmqtt["svalue"] = svalue.c_str();

      serializeJson(jsonmqtt, jsonmqttStr);

      if (!mqttPost(config.domz.topic, jsonmqttStr.c_str()))
      {
        ret = false;
      }
    }

  }

  //If sensor is enabled and seen
  if (config.sensors.en_si7021 && config.config & CFG_SI7021)
  {
    // Do a measurment
    sensors_measure();

    //json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=TEMP;HUM;HUM_STAT
    if (config.domz.bmode & CFG_BMODE_HTTP)
    {
      url = baseurl;
      url += F("?type=command&param=udevice&");
      url += "idx=";
      url += config.domz.idx_si7021;
      url += "&nvalue=0";
      url += "&svalue=";
      url += String(si7021_temperature / 100.0).c_str();
      url += ";";
      url += String(si7021_humidity / 100.0).c_str();
      url += ";0";

      if (!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
      {
        ret = false;
      }
    }

    if (config.domz.bmode & CFG_BMODE_MQTT)
    {
      String jsonmqttStr;
      StaticJsonDocument<200> jsonmqtt;
      
      jsonmqtt["idx"] = config.domz.idx_pct;
      jsonmqtt["nvalue"] = 0;
      String svalue = "";
      svalue += String(si7021_temperature / 100.0).c_str();
      svalue += ";";
      svalue += String(si7021_humidity / 100.0).c_str();
      svalue += ";0";
      jsonmqtt["svalue"] = svalue.c_str();

      serializeJson(jsonmqtt, jsonmqttStr);

      if (!mqttPost(config.domz.topic, jsonmqttStr.c_str()))
      {
        ret = false;
      }
    }

  }

  //If sensor is enabled and seen
  if (config.sensors.en_sht10 && config.config & CFG_SHT10)
  {
    //json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=TEMP;HUM;HUM_STAT
    if (config.domz.bmode & CFG_BMODE_HTTP)
    {
      url = baseurl;
      url += F("?type=command&param=udevice&");
      url += "idx=";
      url += config.domz.idx_sht10;
      url += "&nvalue=0";
      url += "&svalue=";
      url += String(sht1x_temperature / 100.0).c_str();
      url += ";";
      url += String(sht1x_humidity / 100.0).c_str();
      url += ";0";

      if (!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
      {
        ret = false;
      }
    }

    if (config.domz.bmode & CFG_BMODE_MQTT)
    {
      String jsonmqttStr;
      StaticJsonDocument<200> jsonmqtt;
      
      jsonmqtt["idx"] = config.domz.idx_sht10;
      jsonmqtt["nvalue"] = 0;
      String svalue = "";
      svalue += String(sht1x_temperature / 100.0).c_str();
      svalue += ";";
      svalue += String(sht1x_humidity / 100.0).c_str();
      svalue += ";0";
      jsonmqtt["svalue"] = svalue.c_str();

      serializeJson(jsonmqtt, jsonmqttStr);

      if (!mqttPost(config.domz.topic, jsonmqttStr.c_str()))
      {
        ret = false;
      }
    }

  }

  return ret;
}



