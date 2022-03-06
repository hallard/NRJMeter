// **********************************************************************************
// ESP8266 NRJMeter sensors drivers
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
// History : V1.00 2015-06-01 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#ifndef SENSORS_H
#define SENSORS_H

// Include main project include file
#include "NRJMeter.h"

// ======================================
// SHT1x sensor 
// ======================================
// Connected to I2C bus
//#define SHT1x_DATA_PIN  4
//#define SHT1x_CLOCK_PIN 5

// Share I2C clock only
#define SHT1x_DATA_PIN  14
#define SHT1x_CLOCK_PIN 5

//#define SHT1x_DATA_PIN  2
//#define SHT1x_CLOCK_PIN 14

// Conversion coefficients from SHT1x datasheet
//#define SHT1X_D1 = -40.0;  // for 14 Bit @ 5V
#define SHT1x_D1 -39.66 		 // for 14 Bit @ 3.3V
#define SHT1x_D2   0.01 	   // for 14 Bit DEGC
#define SHT1x_C1  -4.0       // for 12 Bit
#define SHT1x_C2   0.0405    // for 12 Bit
#define SHT1x_C3  -0.0000028 // for 12 Bit
#define SHT1x_T1   0.01      // for 14 Bit 
#define SHT1x_T2   0.00008   // for 14 Bit

#define SHT1x_CMD_MEASURE_TEMP	0b00000011 // Measure Temperature 00011
#define SHT1x_CMD_MEASURE_HUM 	0b00000101 // Measure Relative Humidity 00101
#define SHT1x_CMD_WRITE_SR			0b00000110 // Write Status Register 00110
#define SHT1x_CMD_READ_SR 			0b00000111 // Read  Status Register 00111
#define SHT1x_CMD_SOFT_RESET		0b00011110 // Soft reset, resets the interface, clears the status register to default values

// ======================================
// SI7021 sensor 
// ======================================
#define SI7021_I2C_ADDRESS    	0x40 // I2C address for the sensor
#define SI7021_MEASURE_TEMP_HUM	0xE0 // Measure Temp only after a RH conversion done
#define SI7021_MEASURE_TEMP_HM	0xE3 // Default hold Master
#define SI7021_MEASURE_HUM_HM 	0xE5 // Default hold Master
#define SI7021_MEASURE_TEMP			0xF3 // No hold
#define SI7021_MEASURE_HUM 		 	0xF5 // No hold
#define SI7021_WRITE_REG      	0xE6
#define SI7021_READ_REG       	0xE7
#define SI7021_SOFT_RESET     	0xFE

// SI7021 Sensor resolution
// default at power up is SI7021_RESOLUTION_14T_12RH
#define SI7021_RESOLUTION_14T_12RH 0x00 // 12 bits RH / 14 bits Temp
#define SI7021_RESOLUTION_13T_10RH 0x80 // 10 bits RH / 13 bits Temp
#define SI7021_RESOLUTION_12T_08RH 0x01 //  8 bits RH / 12 bits Temp
#define SI7021_RESOLUTION_11T_11RH 0x81 // 11 bits RH / 11 bits Temp
#define SI7021_RESOLUTION_MASK 0B01111110

// ======================================
// MCP3421 ADC
// ======================================
#define MCP3421_I2C_ADDRESS	0x68

// Conversion mode definitions
#define MCP3421_MODE_ONESHOT		0x00
#define MCP3421_MODE_CONTINUOUS	0x10

// Sample size definitions - these also affect the sampling rate
// 12-bit has a max sample rate of 240sps
// 14-bit has a max sample rate of  60sps
// 16-bit has a max sample rate of  15sps
// 18-bit has a max sample rate of   3.75sps (MCP3421, MCP3422, MCP3423, MCP3424 only)
#define MCP3421_SIZE_12BIT	0x00
#define MCP3421_SIZE_14BIT	0x04
#define MCP3421_SIZE_16BIT	0x08
#define MCP3421_SIZE_18BIT	0x0C
#define MCP3421_SIZE_MASK	0x0C

// Programmable Gain definitions
#define MCP3421_GAIN_1X		0x00
#define MCP3421_GAIN_2X		0x01
#define MCP3421_GAIN_4X		0x02
#define MCP3421_GAIN_8X		0x03
#define MCP3421_GAIN_MASK 0x03

// /RDY bit definition
#define MCP3421_RDY	0x80

// ======================================
// MCP4725 DAC 
// ======================================
// For MCP4725A1 the address is 0x62 (default) or 0x63 (ADDR pin tied to VCC)
// For MCP4725A0 the address is 0x60 or 0x61
// For MCP4725A2 the address is 0x64 or 0x65
#define MCP4725_I2C_ADDRESS         (0x60)  // MCP4725 I2C Address

#define MCP4725_CMD_WRITEDAC        (0x40)  // Writes data to the DAC
#define MCP4725_CMD_WRITEDACEEPROM  (0x60)  // Writes data to the DAC and the EEPROM (persisting the assigned value after reset)


// Exported variables/object instancied in main sketch
// ===================================================
extern int16_t  si7021_humidity;    // latest humidity value read (*100)
extern int16_t  si7021_humidity_nc; // latest humidity value read (*100) (no temp comp)
extern int16_t  si7021_temperature; // latest temperature value read (*100)
extern int16_t  sht1x_humidity;    	// latest humidity value read (*100)
extern int16_t  sht1x_humidity_nc; 	// latest humidity value read (*100) (no temp comp)
extern int16_t  sht1x_temperature; 	// latest temperature value read (*100)
extern int16_t  mcp3421_power;    	// latest power read (W)
extern uint32_t  si7021_last_seen;  // latest read from this sensor (in s)
extern uint32_t  sht1x_last_seen;  	// latest read from this sensor (in s)
extern uint32_t  mcp3421_last_seen;	// latest read from this sensor (in s)


// declared exported function from sensors.cpp
// ===================================================
uint8_t mcp3421_async_sample(uint16_t _samples = 0) ;
bool i2c_init(bool reset_bus=false);
uint8_t i2c_scan(void);
void sensors_setup();
void sensors_measure();

#endif
