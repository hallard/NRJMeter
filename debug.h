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

#include "PString.h"
#include <FS.h>

extern char floggerbuffer[255];
extern PString flogger;

// Define here what serial to use for debug
//#define DEBUG_SERIAL	Serial
#define DEBUG_SERIAL	Serial1
#define DEBUG_SERIAL1  

#ifdef DEBUG_SERIAL

	// Classic debug symbol
	#define DEBUG

	// debug functions
	#define Debug(x)     	{ if (config.config & CFG_DEBUG) {DEBUG_SERIAL.print(x);} if (config.config & CFG_LOGGER) {flogger.print(x); floggerflush();} }
	#define Debug2(x,y)  	{ if (config.config & CFG_DEBUG) {DEBUG_SERIAL.print(x,y);} if (config.config & CFG_LOGGER) {flogger.print(x,y); floggerflush();} }
	#define Debugln(x)		{ if (config.config & CFG_DEBUG) {DEBUG_SERIAL.println(x);} if (config.config & CFG_LOGGER) {flogger.println(x); floggerflush();} }
	#define Debugln2(x,y)	{ if (config.config & CFG_DEBUG) {DEBUG_SERIAL.println(x,y);} if (config.config & CFG_LOGGER) {flogger.println(x,y); floggerflush();} }
	#define DebugF(x)   	{ if (config.config & CFG_DEBUG) {DEBUG_SERIAL.print(F(x));} if (config.config & CFG_LOGGER) {flogger.print(F(x)); floggerflush();} }
	#define DebuglnF(x) 	{ if (config.config & CFG_DEBUG) {DEBUG_SERIAL.println(F(x));} if (config.config & CFG_LOGGER) {flogger.println(F(x)); floggerflush();} }
	#define Debugf(...) 	{ if (config.config & CFG_DEBUG) {DEBUG_SERIAL.printf(__VA_ARGS__);} if (config.config & CFG_LOGGER) {flogger.printf(__VA_ARGS__); floggerflush();} }
	#define Debugflush() 	{ if (config.config & CFG_DEBUG) {DEBUG_SERIAL.flush();} if (config.config & CFG_LOGGER) {floggerflush();} }
#else
	// No debug empty macros
	#define Debug(x)    {}
	#define Debugln(x)  {}
	#define DebugF(x)   {}
	#define DebuglnF(x) {}
	#define Debugf(...) {}
	#define Debugflush(){}
#endif

void floggerflush();

#endif 
