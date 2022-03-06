// **********************************************************************************
// ESP8266 NRJMeter Debug 
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

#include "debug.h"

char floggerbuffer[255];
PString flogger(floggerbuffer, sizeof(floggerbuffer));

void floggerflush()
{
  if(LittleFS.begin())
  {

   //check max size & switch file if needed
  File fr = LittleFS.open("/log.txt", "r");
  if(fr)
  {
    if (fr.size() >= 10000)
      {
        fr.close();
        if (LittleFS.exists("/log.1.txt"))
        {
          LittleFS.remove("/log.1.txt");
        }
        LittleFS.rename("/log.txt","/log.1.txt");
      }
      else
      {
        fr.close();
      }
  }
   
   // open file for writing
  File f = LittleFS.open("/log.txt", "a+");
  if (f) {
      f.print(floggerbuffer);
      flogger.begin();
  }
  f.close();
  }
}
