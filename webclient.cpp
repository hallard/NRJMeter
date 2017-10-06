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

#include "webclient.h"

#include <map>
#include <string>

#include "TInfo.h"
#include "MQTT.h"

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
    HTTPClient http;

    unsigned long start = millis();

    // Post should not take more than 2.5 sec
    http.setTimeout(2500);

    // configure traged server and url
    http.begin(host, port, url);

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
    Debugf(" in %d ms\r\n", millis() - start);
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
  String json = "";

  // Some basic checking
  if (*config.jeedom.host) {
    ValueList * me = tinfo.getList();
    // Got at least one ?
    if (me && me->next) {
      String url ;
      boolean skip_item;

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

      // Loop thru the node
      while (me->next) {
        // go to next node
        me = me->next;
        skip_item = false;

        // Si ADCO déjà renseigné, on le remet pas
        if (!strcmp(me->name, "ADCO")) {
          if (*config.jeedom.adco)
            skip_item = true;
        }

        // Si Item virtuel, on le met pas
        if (*me->name == '_')
          skip_item = true;

        // On doit ajouter l'item ?
        if (!skip_item) {
          url +=  me->name ;
          url += "=" ;
          url +=  me->value;
          url += "&" ;
        }
      } // While me

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

    } // if me
  } // if host
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

  // Some basic checking
  if (*config.emoncms.host) {
    String url ;
    struct rst_info * p = ESP.getResetInfoPtr();
    String json = "";

    url = *config.emoncms.url ? config.emoncms.url : "/";

    sprintf_P(buff, PSTR("?node=%d&apikey=%s&json="), config.emoncms.node, config.emoncms.apikey) ;
    url += buff;
    sprintf_P(buff, PSTR("{uptime:%ld"), seconds) ;
    url += buff;
    json += buff;

    // Does SI7021 is enabled and seen
    if (config.sensors.en_si7021 && config.config & CFG_SI7021) {
      sprintf_P(buff, PSTR(",int_temp:%s,int_hum:%s"), String(si7021_temperature / 100.0).c_str(), String(si7021_humidity / 100.0).c_str());
      url += buff;
      json += buff;
    }
    // Does SI7021 is enabled and seen
    if (config.sensors.en_sht10 && config.config & CFG_SHT10) {
      sprintf_P(buff, PSTR(",ext_temp:%s,ext_hum:%s,ext_hum_nc:%s"),
                String(sht1x_temperature / 100.0).c_str(),
                String(sht1x_humidity / 100.0).c_str(),
                String(sht1x_humidity_nc / 100.0).c_str()
               );
      url += buff;
      json += buff;
    }

    sprintf_P(buff, PSTR(",reset:%d}"), p->reason) ;
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
} // if host

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

  ValueList * me = tinfo.getList();
  std::map<std::string, std::string>  meMap;

  String baseurl;
  String url;

  baseurl = *config.domz.url ? config.domz.url : "/";
  baseurl += F("?type=command&param=udevice&");

  // Got at least one ?
  if (me && me->next) {
    // Loop thru the node
    while (me->next) {
      // go to next node
      me = me->next;
      // Si Item virtuel, on le met pas
      if (*me->name == '_')
      {
        //Nothing
      }
      else
      {
        meMap[me->name] = me->value;
      }

    } // While me

    // HTTP: /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=TXT
    // MQTT: '{ "idx" : 1, "nvalue" : 0, "svalue" : "25.0" }'
    if (config.domz.idx_txt > 0)
    {
      if (config.domz.bmode & CFG_BMODE_HTTP)
      {
        url = baseurl;
        url += "idx=";
        url += config.domz.idx_txt;
        url += "&nvalue=0";
        url += "&svalue=";
        url += meMap["ADCO"].c_str();

        if (!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
        {
          ret = false;
        }
      }

      if (config.domz.bmode & CFG_BMODE_MQTT)
      {
        String jsonmqttStr;
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& jsonmqtt = jsonBuffer.createObject();
        jsonmqtt["idx"] = config.domz.idx_txt;
        jsonmqtt["nvalue"] = 0;
        String svalue = "";
        svalue += meMap["ADCO"].c_str();
        jsonmqtt["svalue"] = svalue.c_str();

        jsonmqtt.printTo(jsonmqttStr);

        if (!mqttPost(config.domz.topic, jsonmqttStr.c_str()))
        {
          ret = false;
        }
      }


      /*
        Info(config.domz.host);
        InfoF(":");
        Info(config.domz.port);
        Infoln((char *) url.c_str());
        InfoF("ret=");
        Infoln(ret);
        Infoflush();
      */
    }

    // HTML: /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=USAGE1;USAGE2;RETURN1;RETURN2;CONS;PROD
    // MQTT: '{ "idx" : 1, "nvalue" : 0, "svalue" : "25.0" }'
    if (config.domz.idx_p1sm > 0)
    {
      if (config.domz.bmode & CFG_BMODE_HTTP)
      {
        url = baseurl;
        url += "idx=";
        url += config.domz.idx_p1sm;
        url += "&nvalue=0";
        url += "&svalue=";
        url += String(atoi(meMap["BASE"].c_str())).c_str();
        url += ";0;0;0;";
        url += String(atoi(meMap["PAPP"].c_str())).c_str();
        url += ";0";

        if (!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
        {
          ret = false;
        }
      }

      if (config.domz.bmode & CFG_BMODE_MQTT)
      {
        String jsonmqttStr;
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& jsonmqtt = jsonBuffer.createObject();
        jsonmqtt["idx"] = config.domz.idx_p1sm;
        jsonmqtt["nvalue"] = 0;
        String svalue = "";
        svalue += String(atoi(meMap["BASE"].c_str())).c_str();
        svalue += ";0;0;0;";
        svalue += String(atoi(meMap["PAPP"].c_str())).c_str();
        svalue += ";0";
        jsonmqtt["svalue"] = svalue.c_str();

        jsonmqtt.printTo(jsonmqttStr);

        if (!mqttPost(config.domz.topic, jsonmqttStr.c_str()))
        {
          ret = false;
        }
      }

      /*
        Info(config.domz.host);
        InfoF(":");
        Info(config.domz.port);
        Infoln((char *) url.c_str());
        InfoF("ret=");
        Infoln(ret);
        Infoflush();
      */
    }

    // HTML: /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=ENERGY
    // MQTT: '{ "idx" : 1, "nvalue" : 0, "svalue" : "25.0" }'
    if (config.domz.idx_crt > 0)
    {
      if (config.domz.bmode & CFG_BMODE_HTTP)
      {
        url = baseurl;
        url += "idx=";
        url += config.domz.idx_crt;
        url += "&nvalue=0";
        url += "&svalue=";
        url += String(atoi(meMap["IINST"].c_str())).c_str();

        if (!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
        {
          ret = false;
        }
      }

      if (config.domz.bmode & CFG_BMODE_MQTT)
      {
        String jsonmqttStr;
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& jsonmqtt = jsonBuffer.createObject();
        jsonmqtt["idx"] = config.domz.idx_crt;
        jsonmqtt["nvalue"] = 0;
        String svalue = "";
        svalue += String(atoi(meMap["IINST"].c_str())).c_str();
        jsonmqtt["svalue"] = svalue.c_str();

        jsonmqtt.printTo(jsonmqttStr);

        if (!mqttPost(config.domz.topic, jsonmqttStr.c_str()))
        {
          ret = false;
        }
      }

      /*
        Info(config.domz.host);
        InfoF(":");
        Info(config.domz.port);
        Infoln((char *) url.c_str());
        InfoF("ret=");
        Infoln(ret);
        Infoflush();
      */
    }

    // HTTP: /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=ENERGY
    // MQTT: '{ "idx" : 1, "nvalue" : 0, "svalue" : "25.0" }'
    if (config.domz.idx_elec > 0)
    {
      if (config.domz.bmode & CFG_BMODE_HTTP)
      {
        url = baseurl;
        url += "idx=";
        url += config.domz.idx_elec;
        url += "&nvalue=0";
        url += "&svalue=";
        url += String(atoi(meMap["PAPP"].c_str())).c_str();

        if (!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
        {
          ret = false;
        }
      }

      if (config.domz.bmode & CFG_BMODE_MQTT)
      {
        String jsonmqttStr;
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& jsonmqtt = jsonBuffer.createObject();
        jsonmqtt["idx"] = config.domz.idx_elec;
        jsonmqtt["nvalue"] = 0;
        String svalue = "";
        svalue += String(atoi(meMap["PAPP"].c_str())).c_str();
        jsonmqtt["svalue"] = svalue.c_str();

        jsonmqtt.printTo(jsonmqttStr);

        if (!mqttPost(config.domz.topic, jsonmqttStr.c_str()))
        {
          ret = false;
        }
      }

      /*
        Info(config.domz.host);
        InfoF(":");
        Info(config.domz.port);
        Infoln((char *) url.c_str());
        InfoF("ret=");
        Infoln(ret);
        Infoflush();
      */
    }

    // HTTP: /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=POWER,ENERGY
    // MQTT: '{ "idx" : 1, "nvalue" : 0, "svalue" : "25.0" }'
    if (config.domz.idx_kwh > 0)
    {
      if (config.domz.bmode & CFG_BMODE_HTTP)
      {
        url = baseurl;
        url += "idx=";
        url += config.domz.idx_kwh;
        url += "&nvalue=0";
        url += "&svalue=";
        url += String(atoi(meMap["PAPP"].c_str())).c_str();
        url += ";0";
        //url += String(atoi(meMap["IINST"].c_str())).c_str(); Computed by Domoticz

        if (!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
        {
          ret = false;
        }
      }

      if (config.domz.bmode & CFG_BMODE_MQTT)
      {
        String jsonmqttStr;
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& jsonmqtt = jsonBuffer.createObject();
        jsonmqtt["idx"] = config.domz.idx_kwh;
        jsonmqtt["nvalue"] = 0;
        String svalue = "";
        svalue += String(atoi(meMap["PAPP"].c_str())).c_str();
        svalue += ";0";
        jsonmqtt["svalue"] = svalue.c_str();

        jsonmqtt.printTo(jsonmqttStr);

        if (!mqttPost(config.domz.topic, jsonmqttStr.c_str()))
        {
          ret = false;
        }
      }

      /*
        Info(config.domz.host);
        InfoF(":");
        Info(config.domz.port);
        Infoln((char *) url.c_str());
        InfoF("ret=");
        Infoln(ret);
        Infoflush();
      */
    }

    // HTTP: /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=PERCENTAGE
    // MQTT: '{ "idx" : 1, "nvalue" : 0, "svalue" : "25.0" }'
    if (config.domz.idx_pct > 0)
    {
      if (config.domz.bmode & CFG_BMODE_HTTP)
      {
        url = baseurl;
        url += "idx=";
        url += config.domz.idx_pct;
        url += "&nvalue=0";
        url += "&svalue=";
        url += String( roundf((atof(meMap["IINST"].c_str()) * 100) / atof(meMap["ISOUSC"].c_str()) * 100) / 100 ).c_str();

        if (!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
        {
          ret = false;
        }
      }

      if (config.domz.bmode & CFG_BMODE_MQTT)
      {
        String jsonmqttStr;
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& jsonmqtt = jsonBuffer.createObject();
        jsonmqtt["idx"] = config.domz.idx_pct;
        jsonmqtt["nvalue"] = 0;
        String svalue = "";
        svalue += String( roundf((atof(meMap["IINST"].c_str()) * 100) / atof(meMap["ISOUSC"].c_str()) * 100) / 100 ).c_str();
        jsonmqtt["svalue"] = svalue.c_str();

        jsonmqtt.printTo(jsonmqttStr);

        if (!mqttPost(config.domz.topic, jsonmqttStr.c_str()))
        {
          ret = false;
        }
      }

      /*
        Info(config.domz.host);
        InfoF(":");
        Info(config.domz.port);
        Infoln((char *) url.c_str());
        InfoF("ret=");
        Infoln(ret);
        Infoflush();
      */
    }

  } // if me

  // Do a measurment
  sensors_measure();

  //If sensor is enabled and seen
  if (config.sensors.en_mcp3421 && config.config & CFG_MCP3421)
  {
    //json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=TEMP;HUM;HUM_STAT
    if (config.domz.bmode & CFG_BMODE_HTTP)
    {
      url = baseurl;
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
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& jsonmqtt = jsonBuffer.createObject();
      jsonmqtt["idx"] = config.domz.idx_mcp3421;
      jsonmqtt["nvalue"] = 0;
      String svalue = "";
      svalue += String(mcp3421_power).c_str();
      jsonmqtt["svalue"] = svalue.c_str();

      jsonmqtt.printTo(jsonmqttStr);

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
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& jsonmqtt = jsonBuffer.createObject();
      jsonmqtt["idx"] = config.domz.idx_pct;
      jsonmqtt["nvalue"] = 0;
      String svalue = "";
      svalue += String(si7021_temperature / 100.0).c_str();
      svalue += ";";
      svalue += String(si7021_humidity / 100.0).c_str();
      svalue += ";0";
      jsonmqtt["svalue"] = svalue.c_str();

      jsonmqtt.printTo(jsonmqttStr);

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
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& jsonmqtt = jsonBuffer.createObject();
      jsonmqtt["idx"] = config.domz.idx_sht10;
      jsonmqtt["nvalue"] = 0;
      String svalue = "";
      svalue += String(sht1x_temperature / 100.0).c_str();
      svalue += ";";
      svalue += String(sht1x_humidity / 100.0).c_str();
      svalue += ";0";
      jsonmqtt["svalue"] = svalue.c_str();

      jsonmqtt.printTo(jsonmqttStr);

      if (!mqttPost(config.domz.topic, jsonmqttStr.c_str()))
      {
        ret = false;
      }
    }

  }

  return ret;
}



