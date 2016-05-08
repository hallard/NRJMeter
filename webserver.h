// **********************************************************************************
// ESP8266 NRJMeter WEB Server routing Include file
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

#ifndef WEBSERVER_H
#define WEBSERVER_H

// Include main project include file
#include "NRJMeter.h"

// Exported variables/object instancied in main sketch
// ===================================================

// declared exported function from route.cpp
// ===================================================
void handleTest(AsyncWebServerRequest * request);
void handleRoot(AsyncWebServerRequest * request); 
void handleFormConfig(AsyncWebServerRequest * request) ;
void handleFormCounter(AsyncWebServerRequest * request) ;
void handleNotFound(AsyncWebServerRequest * request);
void tinfoJSONTable(AsyncWebServerRequest * request);
String sysJSONTable(AsyncWebServerRequest * request);
String sensorsJSONTable(AsyncWebServerRequest * request);
void confJSONTable(AsyncWebServerRequest * request);
void spiffsJSONTable(AsyncWebServerRequest * request);
void sendJSON(AsyncWebServerRequest * request);
void wifiScanJSON(AsyncWebServerRequest * request);
void handleFactoryReset(AsyncWebServerRequest * request);
void handleReset(AsyncWebServerRequest * request);

#endif
