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

#include <LibTeleinfo.h>
#include <map>
#include <string>
#include "TInfo.h"

// Teleinfo
TInfo tinfo;

std::map<std::string, std::string>  tinfo_values; //Latest TInfo values
std::map<std::string, std::string>  tinfo_flags; //Latest TInfo flags
std::map<std::string, std::string>  tinfo_checksum; //Latest TInfo checksum

uint32_t tinfo_last_seen;   // latest read from this sensor (in s)

boolean updateTInfo()
{
  boolean ret = false;

  tinfo_values.clear();
  tinfo_flags.clear();
  tinfo_checksum.clear();

  if (config.config & CFG_DEMO_MODE)
  {
    /* Sample TINFO
      [
      {na: "OPTARIF", va: "BASE", ck: "0", fl: 4}
      {na: "ISOUSC", va: "30", ck: "9", fl: 4}
      {na: "BASE", va: "008100469", ck: "'", fl: 4}
      {na: "PTEC", va: "TH..", ck: "$", fl: 4}
      {na: "IINST", va: "002", ck: "Y", fl: 4}
      {na: "IMAX", va: "037", ck: "I", fl: 4}
      {na: "PAPP", va: "00450", ck: "*", fl: 8}
      {na: "MOTDETAT", va: "000000", ck: "B", fl: 4}
      {na: "ADCO", va: "0123456789012", ck: ";", fl: 4}
      ]
    */

    tinfo_values["OPTARIF"] = "BASE";
    tinfo_flags["OPTARIF"] = (random(0, 2) ? "4" : "8");
    tinfo_checksum["OPTARIF"] = "0";

    tinfo_values["ISOUSC"] = "30";
    tinfo_flags["ISOUSC"] = (random(0, 2) ? "4" : "8");
    tinfo_checksum["ISOUSC"] = "9";

    tinfo_values["BASE"] = "008100469";
    tinfo_flags["BASE"] = (random(0, 2) ? "4" : "8");
    tinfo_checksum["BASE"] = "'";

    tinfo_values["PTEC"] = "TH..";
    tinfo_flags["PTEC"] = (random(0, 2) ? "4" : "8");
    tinfo_checksum["PTEC"] = "$";

    tinfo_values["IINST"] = String(random(1, 32)).c_str();
    tinfo_flags["IINST"] = (random(0, 2) ? "4" : "8");
    tinfo_checksum["IINST"] = "Y";

    tinfo_values["IMAX"] = "037";
    tinfo_flags["IMAX"] = (random(0, 2) ? "4" : "8");
    tinfo_checksum["IMAX"] = "I";

    tinfo_values["PAPP"] = String(random(400, 1000)).c_str();
    tinfo_flags["PAPP"] = (random(0, 2) ? "4" : "8");
    tinfo_checksum["PAPP"] = "*";

    tinfo_values["MOTDETAT"] = "000000";
    tinfo_flags["MOTDETAT"] = (random(0, 2) ? "4" : "8");
    tinfo_checksum["MOTDETAT"] = "B";

    tinfo_values["ADCO"] = "0123456789";
    tinfo_flags["ADCO"] = (random(0, 2) ? "4" : "8");
    tinfo_checksum["ADCO"] = ";";

    ret = true;
  }
  else
  {
    ValueList * me = tinfo.getList();

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
          tinfo_values[me->name] = me->value;
          tinfo_flags[me->name] = me->flags;
          tinfo_checksum[me->name] = me->checksum;
        }

      } // While me
      ret = true;
    } // if me
  }

  return ret;
}

/* ======================================================================
  Function: ADPSCallback
  Purpose : called by library when we detected a ADPS on any phased
  Input   : phase number
            0 for ADPS (monophase)
            1 for ADIR1 triphase
            2 for ADIR2 triphase
            3 for ADIR3 triphase
  Output  : -
  Comments: should have been initialised in the main sketch with a
          tinfo.attachADPSCallback(ADPSCallback())
  ====================================================================== */
void ADPSCallback(uint8_t phase)
{
  // Monophasé
  if (phase == 0 ) {
    Debugln(F("ADPS"));
  } else {
    Debug(F("ADPS Phase "));
    Debugln('0' + phase);
  }
}

/* ======================================================================
  Function: DataCallback
  Purpose : callback when we detected new or modified data received
  Input   : linked list pointer on the concerned data
          value current state being TINFO_VALUE_ADDED/TINFO_VALUE_UPDATED
  Output  : -
  Comments: -
  ====================================================================== */
void DataCallback(ValueList * me, uint8_t flags)
{

  // This is for simulating ADPS during my tests
  // ===========================================
  /*
    static uint8_t test = 0;
    // Each new/updated values
    if (++test >= 20) {
    test=0;
    uint8_t anotherflag = TINFO_FLAGS_NONE;
    ValueList * anotherme = tinfo.addCustomValue("ADPS", "46", &anotherflag);

    // Do our job (mainly debug)
    DataCallback(anotherme, anotherflag);
    }
    Debugf("%02d:",test);
  */
  // ===========================================

  /*
    // Do whatever you want there
    Debug(me->name);
    Debug('=');
    Debug(me->value);

    if ( flags & TINFO_FLAGS_NOTHING ) Debug(F(" Nothing"));
    if ( flags & TINFO_FLAGS_ADDED )   Debug(F(" Added"));
    if ( flags & TINFO_FLAGS_UPDATED ) Debug(F(" Updated"));
    if ( flags & TINFO_FLAGS_EXIST )   Debug(F(" Exist"));
    if ( flags & TINFO_FLAGS_ALERT )   Debug(F(" Alert"));

    Debugln();
  */
}

/* ======================================================================
  Function: NewFrame
  Purpose : callback when we received a complete teleinfo frame
  Input   : linked list pointer on the concerned data
  Output  : -
  Comments: -
  ====================================================================== */
void NewFrame(ValueList * me)
{
  char buff[32];

  // Light the RGB LED
  if ( config.config & CFG_RGB_LED) {
    LedRGBON(COLOR_GREEN);

    // led off after delay
    //TODO: rgb_ticker.once_ms( (uint32_t) BLINK_LED_MS, LedOff, (int) RGB_LED_PIN);
  }

  sprintf_P( buff, PSTR("New Frame (%ld Bytes free)"), ESP.getFreeHeap() );
  Debugln(buff);
}

/* ======================================================================
  Function: NewFrame
  Purpose : callback when we received a complete teleinfo frame
  Input   : linked list pointer on the concerned data
  Output  : -
  Comments: it's called only if one data in the frame is different than
          the previous frame
  ====================================================================== */
void UpdatedFrame(ValueList * me)
{
  char buff[32];

  // Light the RGB LED (purple)
  if ( config.config & CFG_RGB_LED) {
    LedRGBON(COLOR_MAGENTA);

    // led off after delay
    //TODO: rgb_ticker.once_ms(BLINK_LED_MS, LedOff, RGB_LED_PIN);
  }

  sprintf_P( buff, PSTR("Updated Frame (%ld Bytes free)"), ESP.getFreeHeap() );
  Debugln(buff);

  /*
    // Got at least one ?
    if (me) {
      WiFiUDP myudp;
      IPAddress ip = WiFi.localIP();

      // start UDP server
      myudp.begin(1201);
      ip[3] = 255;

      // transmit broadcast package
      myudp.beginPacket(ip, 1201);

      // start of frame
      myudp.write(TINFO_STX);

      // Loop thru the node
      while (me->next) {
        me = me->next;
        // prepare line and write it
        sprintf_P( buff, PSTR("%s %s %c\n"),me->name, me->value, me->checksum );
        myudp.write( buff);
      }

      // End of frame
      myudp.write(TINFO_ETX);
      myudp.endPacket();
      myudp.flush();

    }
  */
}

void TInfo_setup()
{

  if (config.config & CFG_TINFO)
  {
    DebugF("TInfo initialization ");

    if (config.tinfo.type & CFG_TI_EDF)
    {
      DebugF("EDF Mode ... ");

      // Teleinfo is connected to RXD2 (GPIO13) to
      // avoid conflict when flashing, this is why
      // we swap RXD1/RXD1 to RXD2/TXD2
      // Note that TXD2 is not used teleinfo is receive only

#ifdef TINFO_SERIAL
      //This section cause WDT reset, it seems to be related to second time calling Serial.begin(..)
      //Found old issue related but seems to be fixed
      //https://github.com/esp8266/Arduino/issues/2305
      TINFO_SERIAL.end();
      DebugF("Setting up Teleinfo on Serial ... ");
      TINFO_SERIAL.begin(1200, SERIAL_7E1);
      //DebugF("Swap Serial ... ");
      //TINFO_SERIAL.swap();
#else
      DebugF("No Teleinfo Serial defined ... ");
#endif

      // Init teleinfo
      tinfo.init();

      tinfo_last_seen = 0;

      // Attach the callback we need
      // set all as an example
      tinfo.attachADPS(ADPSCallback);
      tinfo.attachData(DataCallback);
      tinfo.attachNewFrame(NewFrame);
      tinfo.attachUpdatedFrame(UpdatedFrame);
    }

    DebuglnF("Done");
  }
}

void handle_tinfo()
{
  #ifdef TINFO_SERIAL
  char c;
  // Handle teleinfo serial
  if ( TINFO_SERIAL.available() ) {
    // Read Serial and process to tinfo
    c = TINFO_SERIAL.read();
    //DEBUG_SERIAL.print(c);
    tinfo.process(c);
    
    tinfo_last_seen = 0;
  }
  #endif
}

