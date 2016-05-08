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

  if ( WiFi.status()==WL_CONNECTED ) {
    HTTPClient http;

    unsigned long start = millis();

    // Post should not take more than 2.5 sec
    http.setTimeout(2500);

    // configure traged server and url
    http.begin(host, port, url); 

    Debugf("http%s://%s:%d%s => ", port==443?"s":"", host, port, url);

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
    Debugf(" in %d ms\r\n",millis()-start);
  } else {
    DebuglnF("Wifi not connected!");
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

  // Some basic checking
  if (*config.emoncms.host) {
    String url ; 
    struct rst_info * p = ESP.getResetInfoPtr();

    url = *config.emoncms.url ? config.emoncms.url : "/";

    sprintf_P(buff, PSTR("?node=%d&apikey=%s&json={uptime:%ld"),config.emoncms.node, config.emoncms.apikey, seconds) ;
    url += buff;

    // Does SI7021 is enabled and seen
    if (config.sensors.en_si7021 && config.config & CFG_SI7021) {
      sprintf_P(buff, PSTR(",int_temp:%s,int_hum:%s"), String(si7021_temperature/100.0).c_str(), String(si7021_humidity/100.0).c_str());
      url += buff;
    }
    // Does SI7021 is enabled and seen
    if (config.sensors.en_sht10 && config.config & CFG_SHT10) {
      sprintf_P(buff, PSTR(",ext_temp:%s,ext_hum:%s,ext_hum_nc:%s"),
                      String(sht1x_temperature/100.0).c_str(), 
                      String(sht1x_humidity/100.0).c_str(),
                      String(sht1x_humidity_nc/100.0).c_str()  
                );
      url += buff;
    }

    sprintf_P(buff, PSTR(",reset:%d}"),p->reason) ;
    url += buff;

    ret = httpPost( config.emoncms.host, config.emoncms.port, (char *) url.c_str()) ;
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
  boolean ret = false;
  char buff[128];

  // Some basic checking
  if (*config.domz.host) {
    String url ; 
    struct rst_info * p = ESP.getResetInfoPtr();

    url = *config.domz.url ? config.domz.url : "/";
  } // if host

  return ret;
}



