// **********************************************************************************
// ESP8266 NRJMeter Debug include file
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
#ifndef DEBUG_H
#define DEBUG_H

// Define here what serial to use for debug
#define DEBUG_SERIAL	Serial
//#define DEBUG_SERIAL	Serial1

#ifdef DEBUG_SERIAL

	// Classic debug symbol
	#define DEBUG

	// debug functions
	#define Debug(x)     	{ if (config.config & CFG_DEBUG) DEBUG_SERIAL.print(x);}
	#define Debug2(x,y)  	{ if (config.config & CFG_DEBUG) DEBUG_SERIAL.print(x,y);}
	#define Debugln(x)		{ if (config.config & CFG_DEBUG) DEBUG_SERIAL.println(x);}
	#define Debugln2(x,y)	{ if (config.config & CFG_DEBUG) DEBUG_SERIAL.println(x,y);}
	#define DebugF(x)   	{ if (config.config & CFG_DEBUG) DEBUG_SERIAL.print(F(x));}
	#define DebuglnF(x) 	{ if (config.config & CFG_DEBUG) DEBUG_SERIAL.println(F(x));}
	#define Debugf(...) 	{ if (config.config & CFG_DEBUG) DEBUG_SERIAL.printf(__VA_ARGS__);}
	#define Debugflush() 	{ if (config.config & CFG_DEBUG) DEBUG_SERIAL.flush();}
#else
	// No debug empty macros
	#define Debug(x)    {}
	#define Debugln(x)  {}
	#define DebugF(x)   {}
	#define DebuglnF(x) {}
	#define Debugf(...) {}
	#define Debugflush(){}
#endif


#endif 
