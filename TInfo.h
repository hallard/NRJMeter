// **********************************************************************************
// ESP8266 NRJMeter TInfo 
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
// History : V1.03 2017-09-15 - Sylvain REMY
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#include "debug.h"
#include "config.h"
#include "NRJMeter.h"
#include <map>
#include <string>

#define TINFO_SERIAL  Serial

// Exported variables/object instancied in main sketch
// ===================================================

extern TInfo tinfo;
extern uint32_t  tinfo_last_seen;  // latest read from this sensor (in s)
extern std::map<std::string, std::string>  tinfo_values; //Latest TInfo values
extern std::map<std::string, std::string>  tinfo_flags; //Latest TInfo flags
extern std::map<std::string, std::string>  tinfo_checksum; //Latest TInfo checksum

// Exported function located in main sketch
// ===================================================
void TInfo_setup();
void handle_tinfo();
boolean updateTInfo();
