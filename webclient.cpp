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
  boolean ret = true;

    // Some basic checking
  if (*config.domz.host) {
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
        if (*me->name =='_')
        {
          //Nothing
        }
        else
        {
          meMap[me->name] = me->value;
        }
        
      } // While me
          
      // /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=TXT
      if(config.domz.idx_txt > 0)
      {
          url = baseurl;
          url += "idx=";
          url += config.domz.idx_txt;
          url += "&nvalue=0";
          url += "&svalue=";
          url += meMap["ADCO"].c_str();

          if(!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
          {
            ret = false;
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

      // /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=USAGE1;USAGE2;RETURN1;RETURN2;CONS;PROD
      if(config.domz.idx_p1sm > 0)
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

          if(!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
          {
            ret = false;
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
      
      // /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=ENERGY
      if(config.domz.idx_crt > 0)
      {
          url = baseurl;
          url += "idx=";
          url += config.domz.idx_crt;
          url += "&nvalue=0";
          url += "&svalue=";
          url += String(atoi(meMap["IINST"].c_str())).c_str();

          if(!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
          {
            ret = false;
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

      // /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=ENERGY
      if(config.domz.idx_elec > 0)
      {
          url = baseurl;
          url += "idx=";
          url += config.domz.idx_elec;
          url += "&nvalue=0";
          url += "&svalue=";
          url += String(atoi(meMap["PAPP"].c_str())).c_str();

          if(!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
          {
            ret = false;
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

      // /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=POWER,ENERGY
      if(config.domz.idx_kwh > 0)
      {
          url = baseurl;
          url += "idx=";
          url += config.domz.idx_kwh;
          url += "&nvalue=0";
          url += "&svalue=";
          url += String(atoi(meMap["PAPP"].c_str())).c_str();
          url += ";0";
          //url += String(atoi(meMap["IINST"].c_str())).c_str(); Computed by Domoticz

          if(!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
          {
            ret = false;
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
      // /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=PERCENTAGE
      if(config.domz.idx_pct > 0)
      {
          url = baseurl;
          url += "idx=";
          url += config.domz.idx_pct;
          url += "&nvalue=0";
          url += "&svalue=";
          url += String( roundf((atof(meMap["IINST"].c_str())* 100) / atof(meMap["ISOUSC"].c_str()) * 100) / 100 ).c_str();

          if(!httpPost( config.domz.host, config.domz.port, (char *) url.c_str()))
          {
            ret = false;
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
  }
  return ret;
}



