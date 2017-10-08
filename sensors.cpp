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

#include "sensors.h"

int16_t  si7021_humidity;    // latest humidity value read (*100)
int16_t  si7021_humidity_nc; // latest humidity value read (*100) (no temp comp)
int16_t  si7021_temperature; // latest temperature value read (*100)
uint32_t si7021_last_seen;   // latest read from this sensor (in s)

int16_t  sht1x_humidity;    // latest humidity value read (*100)
int16_t  sht1x_humidity_nc; // latest humidity value read (*100) (no temp comp)
int16_t  sht1x_temperature; // latest temperature value read (*100)
uint32_t sht1x_last_seen;   // latest read from this sensor (in s)
uint8_t  sht1x_crc; // CRC

int16_t  mcp3421_power;         // latest power value read
uint32_t mcp3421_last_seen;     // latest read from this sensor (in s)
uint8_t  mcp3421_configRegShdw; // Configuration register


// Calculate CRC for a single byte
void sht1x_calcCRC(uint8_t value, uint8_t *crc) {
  const uint8_t POLY = 0x31;   // Polynomial: x**8 + x**5 + x**4 + 1
  uint8_t i;
  *crc ^= value;
  for (i = 8; i > 0; i--) {
    if (*crc & 0x80)
      *crc = (*crc << 1) ^ POLY;
    else
      *crc = (*crc << 1);
  }
}

// Bit-reverse a byte (for CRC calculations)
uint8_t sht1x_bitReverse(uint8_t value) {
  uint8_t i;
  uint8_t result = 0;
  for (i = 8; i > 0; i--) {
    result = (result << 1) | (value & 0x01);
    value >>= 1;
  }
  return result;
}

// Sends ACK to the SHT1x confirming byte measurement data was received by the caller.
void sht1x_sendACK()
{
  // Send the required ack
  pinMode(SHT1x_DATA_PIN, OUTPUT);
  digitalWrite(SHT1x_DATA_PIN, HIGH);
  digitalWrite(SHT1x_DATA_PIN, LOW);
  digitalWrite(SHT1x_CLOCK_PIN, HIGH);
  digitalWrite(SHT1x_CLOCK_PIN, LOW);
}

// Gets ACK from the SHT1x confirming data was received by the sensor.
// return true if ack received
bool sht1x_getACK()
{
  bool ack;

  pinMode(SHT1x_DATA_PIN, INPUT);
  digitalWrite(SHT1x_CLOCK_PIN, HIGH);
  ack = digitalRead(SHT1x_DATA_PIN);

  digitalWrite(SHT1x_CLOCK_PIN, LOW);

  return (!ack);
}


bool sht1x_sendCommand(uint8_t command)
{
  bool ack;

  // Init CRC
  sht1x_crc = 0;

  // Transmission Start
  pinMode(SHT1x_DATA_PIN, OUTPUT);
  pinMode(SHT1x_CLOCK_PIN, OUTPUT);
  digitalWrite(SHT1x_DATA_PIN, HIGH);
  digitalWrite(SHT1x_CLOCK_PIN, HIGH);
  digitalWrite(SHT1x_DATA_PIN, LOW);
  digitalWrite(SHT1x_CLOCK_PIN, LOW);
  digitalWrite(SHT1x_CLOCK_PIN, HIGH);
  digitalWrite(SHT1x_DATA_PIN, HIGH);
  digitalWrite(SHT1x_CLOCK_PIN, LOW);

  // Command should be in CRC
  sht1x_calcCRC(command, &sht1x_crc);

  // The command (3 msb are address and must be 000, and last 5 bits are command)
  shiftOut(SHT1x_DATA_PIN, SHT1x_CLOCK_PIN, MSBFIRST, command);

  // Verify we get the correct ack
  ack = sht1x_getACK();

  return ack;
}

// True if okay, false if timed out
bool sht1x_waitForData(void)
{
  bool data = 1;
  long start = millis();

  pinMode(SHT1x_DATA_PIN, INPUT);

  // until ok or timeout (320 ms max with 14bits, we take 330 ms)
  while ( data == 1 && millis() - start <= 330 ) {
    handle_net();
    data = digitalRead( SHT1x_DATA_PIN );
  }

  /*
    DebugF("SHT1x Measure ");
    if (data) {
      DebugF( "timed out");
    } else {
      DebugF( "end OK");
    }
    DebugF(" in ");
    Debug( millis()-start );
    DebuglnF("ms");
  */

  return !data;
}

uint8_t sht1x_getByte(void)
{
  uint8_t val;

  // Get the byte
  pinMode(SHT1x_DATA_PIN, INPUT);
  val = shiftIn(SHT1x_DATA_PIN, SHT1x_CLOCK_PIN, 8);
  return val;
}

uint16_t sht1x_getWord(void)
{
  uint16_t val;
  uint8_t msb, lsb;

  // Get the most significant bits
  msb = sht1x_getByte();
  sht1x_calcCRC(msb, &sht1x_crc);

  // Send the required ack
  sht1x_sendACK();

  // Get the least significant bits
  lsb = sht1x_getByte();
  sht1x_calcCRC(lsb, &sht1x_crc);

  return ( (msb << 8) | lsb );
}

void sht1x_endTransmission(void)
{
  pinMode(SHT1x_DATA_PIN, OUTPUT);
  digitalWrite(SHT1x_DATA_PIN, HIGH);
  digitalWrite(SHT1x_CLOCK_PIN, HIGH);
  digitalWrite(SHT1x_CLOCK_PIN, LOW);
}

bool sht1x_getCrc(void)
{
  uint8_t crc;

  // Send the required ack, get CRC and end
  sht1x_sendACK();
  crc = sht1x_getByte();
  sht1x_endTransmission();

  // Reverse bits of our calculated CRC
  sht1x_crc = sht1x_bitReverse(sht1x_crc);

  // Check Good CRC
  if (crc != sht1x_crc) {
    DebugF("CRC Error seen=0x");
    Debug(crc);
    DebugF(" calculated=0x");
    Debugln(sht1x_crc);
    return false;
  }

  return true;
}


bool sht1x_readTemperature()
{
  uint16_t val;      // Raw value returned from sensor
  float temperature; // Temperature derived from raw value

  // Command to send to the SHT1x to request Temperature
  sht1x_sendCommand(SHT1x_CMD_MEASURE_TEMP);
  sht1x_waitForData();
  val = sht1x_getWord();

  // CRC OK we continue
  if ( sht1x_getCrc() ) {
    // Convert raw value to degrees Celsius
    temperature = (val * SHT1x_D2) + SHT1x_D1;

    // latest temperature value read (*100)
    sht1x_temperature = round(temperature * 100);

    return true;
  }

  return false;
}

bool sht1x_testDevice()
{
  bool ok = false ;

  // Test if device ACKED sending a dumy command
  if ( sht1x_sendCommand(SHT1x_CMD_READ_SR) )
  {
    ok = true ;

    // End our transmission
    sht1x_endTransmission();
  }

  sht1x_last_seen = 0;
  return ok;
}

bool sht1x_readAll()
{
  uint16_t val;            // Raw humidity value returned from sensor
  float linearHumidity;    // Humidity with linear correction applied
  float correctedHumidity; // Temperature-corrected humidity
  float temperature;       // Raw temperature value

  // Fetch the value from the sensor
  // Command to send to the SHT1x to request humidity
  sht1x_sendCommand(SHT1x_CMD_MEASURE_HUM);
  sht1x_waitForData();
  val = sht1x_getWord();

  // CRC OK we continue
  if ( sht1x_getCrc() ) {
    // Apply linear conversion to raw value
    linearHumidity = SHT1x_C1 + SHT1x_C2 * val + SHT1x_C3 * val * val;

    sht1x_humidity_nc = round(linearHumidity * 100);

    // Get current temperature for humidity correction
    if ( sht1x_readTemperature() ) {
      temperature = sht1x_temperature / 100.0;

      // Correct humidity value for current temperature
      correctedHumidity = (temperature - 25.0 ) * (SHT1x_T1 + SHT1x_T2 * val) + linearHumidity;

      // latest humidity value read (*100)
      sht1x_humidity = round(correctedHumidity * 100);

      return true;
    }
  }

  return false;
}

bool sht1x_reset()
{
  // Send Reset to device
  return (sht1x_sendCommand(SHT1x_CMD_SOFT_RESET) );
}



/* ======================================================================
  Function: mcp4725_setVoltage
  Purpose : Sets the output voltage
  Input   : fraction of vref. (0..4095)
          true to write output to EEPROM
  Output  : -
  Comments: if write to EEEPROM, output will be written to the MCP4725's
          internal non-volatile memory, meaning that the DAC will
          retain the current voltage output after power-down or reset.
  ====================================================================== */
void mcp4725_setVoltage( uint16_t output, bool writeEEPROM )
{
  Wire.beginTransmission(MCP4725_I2C_ADDRESS);
  if (writeEEPROM)
    Wire.write(MCP4725_CMD_WRITEDACEEPROM);
  else
    Wire.write(MCP4725_CMD_WRITEDAC);

  Wire.write(output / 16);        // Upper data bits (D11.D10.D9.D8.D7.D6.D5.D4)
  Wire.write((output % 16) << 4); // Lower data bits (D3.D2.D1.D0.x.x.x.x)
  Wire.endTransmission();
}


/* ======================================================================
  Function: mcp3421_setConfig
  Purpose : set the MCP3421 configuration
  Input   : configuration register
  Output  : true if okay
  Comments: -
  ====================================================================== */
boolean mcp3421_setConfig(uint8_t value)
{
  uint8_t err;

  mcp3421_configRegShdw = value;

  // Set configuration register
  Wire.beginTransmission(MCP3421_I2C_ADDRESS);
  Wire.write(mcp3421_configRegShdw );
  err = Wire.endTransmission() ;

  Debugf("mcp3421_setConfig : Writing 0x%02X at address 0x%02X => %d\r\n", mcp3421_configRegShdw, MCP3421_I2C_ADDRESS, err )

  return err == 0 ? true : false;
}

/* ======================================================================
  Function: mcp3421_init
  Purpose : set the MCP3421 intitial state
  Input   :
  Output  : true  if okay (module found)
  Comments: -
  ====================================================================== */
boolean mcp3421_init(void)
{
  //boolean ret = mcp3421_setConfig(MCP3421_MODE_CONTINUOUS|MCP3421_SIZE_16BIT|MCP3421_GAIN_1X);
  boolean ret =  mcp3421_setConfig(MCP3421_MODE_CONTINUOUS | MCP3421_SIZE_12BIT | MCP3421_GAIN_1X);

  if (ret) {
    config.config |= CFG_MCP3421;
  } else {
    config.config &= ~CFG_MCP3421;
  }

  DebugF("mcp3421_init()=");
  Debugln(ret);

  mcp3421_last_seen = 0;

  return ret;
}


/* ======================================================================
  Function: mcp3421_getConfig
  Purpose : get the MCP3421 configuration register
  Input   : -
  Output  : configuration register
  Comments: -
  ====================================================================== */
uint8_t mcp3421_getConfig( void )
{
  return mcp3421_configRegShdw ;
}

/* ======================================================================
  Function: mcp3421_async_sample
  Purpose : do async sampling
  Input   : number of sample do take (0 when just take one if ready)
  Output  : -1 "0xFF' if error
  Comments: for 12, 14 or 16 bits only
  ====================================================================== */
uint8_t mcp3421_async_sample(uint16_t _samples)
{
  static uint16_t samples = 0;
  static int16_t high = 0;
  static unsigned long started = millis();
  uint8_t adcRegister = 0xFF;

  // We need to start
  if (_samples > 0) {
    // not finished our previous samples
    if (samples > 0) {
      Debugf("mcp3421_sample : Not finished previous, remaining %d samples\r\n", samples);
    } else {
      // start
      samples = _samples;
      high = 0;
      started = millis();
    }
  }

  // need at least one sample to get ?
  if (samples > 0 && _samples == 0) {
    if ((mcp3421_configRegShdw & MCP3421_SIZE_MASK) == MCP3421_SIZE_18BIT) {
      DebugF("mcp3421_sample : Error we're in MCP3421_SIZE_18BIT mode\r\n");
    } else {
      // Get data from ADC
      if (Wire.requestFrom( (uint8_t) MCP3421_I2C_ADDRESS, (uint8_t) 3) < 3) {
        Debugf("mcp3421_sample[%d] : I2C Request[3] error!", samples);
        samples = 0;
        high = 0;
      } else {
        int16_t raw;
        uint8_t b1, b2 ;

        // Comes back in three bytes, data(MSB) / data(LSB) / Checksum
        b1 = Wire.read();
        b2 = Wire.read();
        adcRegister = Wire.read();

        //Debugf("mcp3421_sample[%d] : Reading 2 bytes from 0x%02X=%02X-%02X => %02X\r\n", samples, MCP3421_I2C_ADDRESS, b1, b2, adcRegister )

        // ADC RDY bit = 0 when conversion is done
        if ((adcRegister & MCP3421_RDY) == 0) {
          raw = (int16_t)(b1 << 8) | b2;
          if (raw < 0) {
            raw = - raw;
          }
          if (raw > high) {
            Debugf("mcp3421_sample[%d] : peak => %d\r\n", samples, raw);
            high = raw;
          }

          // Got one sample
          samples--;
        }
      }
      Wire.endTransmission();
    }

    // it was our last sample ?now calc value
    // Post Sample calculations
    if (samples == 0) {
      float power, amp;
      float mvolt = high ;

      //mvolt = 1000 * 62.5 * raw;
      uint8_t gain = mcp3421_getConfig() & MCP3421_GAIN_MASK;

      // Gain 2, need to divide by 2
      if ( gain == MCP3421_GAIN_8X) {
        mvolt /= 8;
      } else if ( gain == MCP3421_GAIN_4X ) {
        mvolt /= 4;
      } else if ( gain == MCP3421_GAIN_2X ) {
        mvolt /= 2;
      }

      // Amp = 5A / 1000 mV => 5mA/mV
      amp = mvolt * 0.005;

      // Power = Amp * Live Voltage (220V)
      power = amp * 220;

      mcp3421_power = (int16_t) power;

      Debugf("mcp3421_sample in %d ms : V=%fmV  A=%fA  P=%fW\r\n", millis() - started, mvolt, amp, power );
    }
  }

  return adcRegister;
}


/* ======================================================================
  Function: si7021_checkCRC
  Purpose : check the CRC of received data
  Input   : value read from sensor
  Output  : CRC read from sensor
  Comments: 0 if okay
  ====================================================================== */
uint8_t si7021_checkCRC(uint16_t data, uint8_t check)
{
  uint32_t remainder, divisor;

  //Pad with 8 bits because we have to add in the check value
  remainder = (uint32_t)data ;
  remainder <<= 8;

  // From: http://www.nongnu.org/avr-libc/user-manual/group__util__crc.html
  // POLYNOMIAL = 0x0131 = x^8 + x^5 + x^4 + 1 : http://en.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks
  // 0x988000 is the 0x0131 polynomial shifted to farthest left of three bytes
  divisor = (uint32_t) 0x988000;

  // Add the check value
  remainder |= check;

  // Operate on only 16 positions of max 24.
  // The remaining 8 are our remainder and should be zero when we're done.
  for (uint8_t i = 0 ; i < 16 ; i++) {
    //Check if there is a one in the left position
    if ( remainder & ((uint32_t)1 << (23 - i)) )
      remainder ^= divisor;

    //Rotate the divisor max 16 times so that we have 8 bits left of a remainder
    divisor >>= 1;
  }
  return ((uint8_t) remainder);
}

/* ======================================================================
  Function: si7021_readRegister
  Purpose : read the user register from the sensor
  Input   : user register value filled by function
  Output  : 0 if okay
  Comments: -
  ====================================================================== */
int8_t si7021_readRegister(uint8_t * value)
{
  uint8_t error ;

  // Request user register
  Wire.beginTransmission(SI7021_I2C_ADDRESS);
  Wire.write(SI7021_READ_REG);
  Wire.endTransmission();

  // request 1 byte result
  Wire.requestFrom(SI7021_I2C_ADDRESS, 1);
  if (Wire.available() >= 1) {
    *value = Wire.read();
    return 0;
  }

  return 1;
}

/* ======================================================================
  Function: si7021_startConv
  Purpose : return temperature or humidity measured
  Input   : data type SI7021_READ_HUM or SI7021_READ_TEMP
          current config resolution
  Output  : 0 if okay
  Comments: internal values of temp and rh are set
  ====================================================================== */
int8_t si7021_startConv(uint8_t datatype, uint8_t resolution)
{
  long data;
  uint16_t raw ;
  uint8_t checksum, tmp;
  uint8_t error;

  //Request a reading
  Wire.beginTransmission(SI7021_I2C_ADDRESS);
  Wire.write(datatype);
  Wire.endTransmission();


  // If we need to get temp from a previous humidity measure
  // we have the value, no need to wait
  if (datatype != SI7021_MEASURE_TEMP_HUM) {

    // Tried clock streching and looping until no NACK from SI7021 to know
    // when conversion's done. None have worked so far !!!
    // I fade up, I'm waiting maximum conversion time + 1ms, this works !!
    // I increased these value to add HTU21D compatibility
    // Max for SI7021 is 3/5/7/12 ms
    // max for HTU21D is 3/5/8/16 ms
    if (resolution == SI7021_RESOLUTION_11T_11RH)
      tmp = 4;
    else if (resolution == SI7021_RESOLUTION_12T_08RH)
      tmp = 6;
    else if (resolution == SI7021_RESOLUTION_13T_10RH)
      tmp = 9;
    else
      tmp = 17;

    // Humidity fire also temp measurment so delay
    // need to be increased by 2 if no Hold Master
    if (datatype == SI7021_MEASURE_HUM)
      tmp *= 2;

    //DebugF("SI7021 delay : ");
    //Debug(tmp);
    //DebuglnF(" ms");

    delay(tmp);
  }

  if ( Wire.requestFrom(SI7021_I2C_ADDRESS, 3) < 3 ) {
    DebugF("SI7021 : I2C Request error!");
    return -1;
  }

  // Comes back in three bytes, data(MSB) / data(LSB) / Checksum
  raw  = ((uint16_t) Wire.read()) << 8;
  raw |= Wire.read();
  checksum = Wire.read();

  // Check CRC of data received
  if (si7021_checkCRC(raw, checksum) != 0) {
    DebugF("SI7021 : checksum error! data=0x");
    Debug2( raw, HEX );
    DebugF(" checksum=0x");
    Debug2(checksum, HEX );
    return -1;
  }

  // Humidity
  if (datatype == SI7021_MEASURE_HUM || datatype == SI7021_MEASURE_HUM_HM ) {
    // Convert value to Himidity percent
    data = ((125 * (long)raw) >> 16) - 6;

    // Datasheet says doing this check
    if (data > 100) data = 100;
    if (data < 0)   data = 0;

    // Convert value to Humidity (*100)
    si7021_humidity = (int16_t) data * 100;

    // Temperature
  } else  if (datatype == SI7021_MEASURE_TEMP || datatype == SI7021_MEASURE_TEMP_HM || datatype == SI7021_MEASURE_TEMP_HUM) {
    // Convert value to Temperature (*100)
    // for 23.45C value will be 2345
    data =  ((17572 * (long)raw) >> 16) - 4685;

    // save value
    si7021_temperature = (int16_t) data;
  }

  return 0;
}


/* ======================================================================
  Function: si7021_readValues
  Purpose : read temperature and humidity from SI7021 sensor
  Input   : current config resolution
  Output  : 0 if okay
  Comments: -
  ====================================================================== */
int8_t si7021_readValues(uint8_t resolution)
{
  int8_t error = 0;

  // start humidity conversion
  error |= si7021_startConv(SI7021_MEASURE_HUM, resolution);

  // read temperature conversion from previous humidity reading
  //error |= si7021_startConv(SI7021_MEASURE_TEMP_HUM, resolution);
  error |= si7021_startConv(SI7021_MEASURE_TEMP, resolution);

  return error;
}


/* ======================================================================
  Function: si7021_setResolution
  Purpose : Sets the sensor resolution to one of four levels
  Input   : see #define default is SI7021_RESOLUTION_14T_12RH
  Output  : 0 if okay
  Comments: -
  ====================================================================== */
int8_t si7021_setResolution(uint8_t res)
{
  uint8_t reg;
  uint8_t error;

  // Get the current register value
  error = si7021_readRegister(&reg);
  if ( error == 0) {
    // remove resolution bits
    reg &= SI7021_RESOLUTION_MASK ;

    // Prepare to write to the register value
    Wire.beginTransmission(SI7021_I2C_ADDRESS);
    Wire.write(SI7021_WRITE_REG);

    // Write the new resolution bits but clear unused before
    Wire.write(reg | ( res &= ~SI7021_RESOLUTION_MASK) );
    return (int8_t) Wire.endTransmission();
  }

  return error;
}

/* ======================================================================
  Function: si7021_init
  Purpose : initialize the sensor
  Input   : user register value filled by function
  Output  : true if okay
  Comments: -
  ====================================================================== */
boolean si7021_init(uint8_t resolution)
{
  uint8_t ret;

  // Does SI7021 is enabled
  if (config.sensors.en_si7021) {
    // Set the resolution we want
    ret = si7021_setResolution(resolution);
    if ( ret == 0 ) {
      config.config |= CFG_SI7021;
      ret = true;
    } else {
      config.config &= ~CFG_SI7021;
      ret = false;
      String log = F("SI7021 : Res=0x");
      log += String(resolution, HEX);
      log += F(" => Error 0x");
      log += String(ret, HEX);
      Debug(log);
    }
  }

  si7021_last_seen = 0;;

  return ret;
}

/* ======================================================================
  Function: si7021_init
  Purpose : initialize the sensor with default values
  Input   : -
  Output  : true if okay
  Comments: -
  ====================================================================== */
boolean si7021_init(void)
{
  return si7021_init(SI7021_RESOLUTION_14T_12RH);
}


/* ======================================================================
  Function: i2c_clearBus
  Purpose : This routine turns off the I2C bus and clears it
  Input   : -
  Output  : 0 if bus cleared
          1 if SCL held low.
          2 if SDA held low by slave clock stretch for > 2sec
          3 if SDA held low after 20 clocks
  Comments: You need to call Wire.begin() after this to re-enable I2C
  ===================================================================== */
int i2c_clearBus() {

  uint8_t sda = 4;
  uint8_t scl = 5;
  bool sda_low;
  bool scl_low;

  DebuglnF("Entering i2c_clearBus()");

  pinMode(sda, INPUT_PULLUP); // Make SDA (data) and SCL (clock) pins Inputs with pullup.
  pinMode(scl, INPUT_PULLUP);

  scl_low = (digitalRead(scl) == LOW); // Check is SCL is Low.

  //I2C bus error. Could not clear SCL clock line held low
  if (scl_low) {
    DebugF("SCL clock line held low");
    return 1;
  }

  sda_low = (digitalRead(sda) == LOW);  // vi. Check SDA input.
  int clockCount = 20; // > 2x9 clock

  while (sda_low && (clockCount > 0)) { //  vii. If SDA is Low,
    clockCount--;

    // Note: I2C bus is open collector so do NOT drive SCL or SDA high.
    pinMode(scl, INPUT); // release SCL pullup so that when made output it will be LOW
    pinMode(scl, OUTPUT); // then clock SCL Low
    delayMicroseconds(10); //  for >5uS
    pinMode(scl, INPUT); // release SCL LOW
    pinMode(scl, INPUT_PULLUP); // turn on pullup resistors again

    // do not force high as slave may be holding it low for clock stretching.
    delayMicroseconds(10); //  for >5uS

    // The >5uS is so that even the slowest I2C devices are handled.
    scl_low = (digitalRead(scl) == LOW); // Check if SCL is Low.
    int counter = 20;

    // loop waiting for SCL to become High only wait 2sec.
    while (scl_low && (counter > 0)) {
      counter--;
      delay(100);
      scl_low = (digitalRead(scl) == LOW);
    }

    // still low after 2 sec error
    // I2C bus error. Could not clear. SCL clock line held low by slave clock stretch for >2sec
    if (scl_low) {
      DebugF("SCL clock line held low by slave clock stretch");
      return 2;
    }

    sda_low = (digitalRead(sda) == LOW); //   and check SDA input again and loop
  }

  // still low
  // I2C bus error. Could not clear. SDA data line held low
  if (sda_low) {
    DebugF("SDA data line held low");
    return 3;
  }

  // else pull SDA line low for Start or Repeated Start
  pinMode(sda, INPUT); // remove pullup.
  pinMode(sda, OUTPUT);  // and then make it LOW i.e. send an I2C Start or Repeated start control.
  // When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
  /// A Repeat Start is a Start occurring after a Start with no intervening Stop.
  delayMicroseconds(10); // wait >5uS
  pinMode(sda, INPUT); // remove output low
  pinMode(sda, INPUT_PULLUP); // and make SDA high i.e. send I2C STOP control.
  delayMicroseconds(10); // x. wait >5uS
  pinMode(sda, INPUT); // and reset pins as tri-state inputs which is the default state on reset
  pinMode(sda, INPUT);

  DebuglnF("i2c_clearBus() exiting OK");

  return 0; // all ok
}


/* ======================================================================
  Function: i2c_init
  Purpose : initialize the I2C function and setup
  Input   : -
  Output  : -
  Comments: -
  ====================================================================== */
bool i2c_init(bool reset_bus)
{

  // Do we need to reset bus ?
  if (reset_bus)  {
    i2c_clearBus();
  }

  // Set i2C speed
  // Sepecific ESP8266 to set I2C Speed
  Wire.begin();
  Wire.setClock(100000);
}

/* ======================================================================
  Function: i2c_detect
  Purpose : check that a adressed device respond
  Input   : I2C device address
  Output  : true is seen (ACKed device) false otherwise
  Comments: i2c_init should have been called before
  ====================================================================== */
bool i2c_detect(uint8_t _address)
{
  Wire.beginTransmission(_address);
  return (Wire.endTransmission() == 0 ? true : false);
}

/* ======================================================================
  Function: i2c_scan
  Purpose : scan I2C bus and display result
  Input   : address wanted to search (0xFF)
  Output  : true if I2C device found at address given
  Comments: mostly used for debug purpose
  ====================================================================== */
uint8_t i2c_scan(void)
{
  byte error, address;
  uint8_t nDevices = 0;

  unsigned long start = millis();

  DebuglnF("Scanning I2C bus ...");

  // slow down i2C speed in case of slow device
  Wire.setClock(100000);

  for (address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      DebugF("I2C device found at address 0x");
      nDevices++;
      if (address < 16)
        Debug("0");
      Debug2(address, HEX);

      if (address == 0x3C || address == 0x3D) {
        DebugF("-> OLED !");
        config.config |= CFG_HASOLED;
      } else if (address == 0x29 || address == 0x39 || address == 0x49) {
        DebugF("-> TSL2561 !");
      } else if (address == 0x40) {
        DebugF("-> SI70221/HTU21D");
        config.config |= CFG_SI7021;
      } else if (address >= 0x60 && address <= 0x62 ) {
        config.config |= CFG_MCP4725;
        DebugF("-> MCP4725_A");
        Debugln((char) ('0' + (address & 0x03)) );
      } else if (address >= 0x68 && address <= 0x6A) {
        config.config |= CFG_MCP3421;
        DebugF("-> MCP3421_A");
        Debugln((char)('0' + (address & 0x03)));
      } else {
        DebuglnF("-> Unknown device !");
      }
    }
  }

  Debug(nDevices);
  DebugF(" I2C devices found, scan took ");
  Debug(millis() - start);
  DebuglnF(" ms");

  // Get back to full speed
  Wire.setClock(400000);

  return (nDevices);
}


void sensors_setDAC(uint16_t out_power)
{

}

void sensors_setup(void)
{
  i2c_init();

  // Does SI7021 is enabled
  if (config.sensors.en_si7021) {
    si7021_init();
  }

  // Does MCP3421 is enabled
  if (config.sensors.en_mcp3421) {
    mcp3421_init() ;
  }

  // Issue à 1st Measurement
  sensors_measure();
}

void sensors_measure(void)
{
  if (config.config & CFG_DEMO_MODE)
  {
      //Demo mode, generating random values
      sht1x_last_seen = 0;
      sht1x_temperature = random(2500, 3000);
      sht1x_humidity = random(2500, 3000);;
      si7021_last_seen = 0;
      si7021_temperature = random(2500, 3000);;
      si7021_humidity = random(2500, 3000);;
      mcp3421_last_seen = 0;
      mcp3421_power = random(100, 250);;
  }
  else
  {
    uint8_t sr;

    // Does SHT10 is enabled
    if (!config.sensors.en_sht10) {
      DebuglnF("SHT10 Disabled in config");
    } else {
      // Check sensor here (will respond ACK)
      if ( sht1x_testDevice())
      {
        DebugF("SHT10 OK");
        // Read values from the sensor
        if (sht1x_readAll())
        {
          config.config |= CFG_SHT10;
          sht1x_last_seen = 0;

          // Print the values to the serial port
          DebugF(" Temperature: ");
          Debug( sht1x_temperature / 100.0 );
          DebugF("C  Humidity: ");
          Debug( sht1x_humidity / 100.0 );
          DebugF("% Reset ");
          if (sht1x_reset()) {
            DebuglnF("OK!");
          } else {
            DebuglnF("NOT ACKed!");
          }
        } else {
          DebuglnF("Error reading SHT10");
        }
      } else {
        DebuglnF("SHT10 Not Found!");
        config.config &= ~CFG_SHT10;
      }
    } // SHT10 enabled

    // Does SI7021 is enabled
    if (!config.sensors.en_si7021) {
      DebuglnF("SI7021 Disabled in config");
    } else {
      // Re init I2C Bus if needed
      if ((config.config & CFG_SI7021) == 0)
      {
        i2c_init(true);
        si7021_init();
        delay(20);
      } else {
        i2c_init(false);
      }

      // Read values
      if (si7021_readValues(SI7021_RESOLUTION_14T_12RH) == 0) {
        config.config |= CFG_SI7021;
        si7021_last_seen = 0;
        // Print the values to the serial port
        DebugF("SI7021 Temperature: ");
        Debug( si7021_temperature / 100.0);
        DebugF("C  Humidity: ");
        Debug( si7021_humidity / 100.0);
        Debugln("%");
      } else {
        config.config &= ~CFG_SI7021;
        DebuglnF("Error reading SI7021");
      }
    } // SI7021 enabled

    // Does 0V 10V Output detected  ?
    if ( config.config & CFG_MCP4725 )  {
      float pwr_scale;
      float power = config.led_panel;

      DebugF("power=");
      Debug(power);

      DebugF("  scale=");
      pwr_scale = (4095.0 * power / 100.0);
      if (pwr_scale < 0)    pwr_scale = 0;
      if (pwr_scale > 4095) pwr_scale = 4095;

      // 0 to 3V3 to 0V-10V = *3 but AOP = * 4 so adjust
      pwr_scale = pwr_scale / 4.0 * 3.0;

      Debug( (uint32_t) pwr_scale);
      DebugF( " => VDAC=");
      Debug( 3300 * pwr_scale / 4095 / 1000.0 );
      Debug("V  Vout=");
      Debug( 4 * 3300 * pwr_scale / 4095 / 1000.0 );
      DebuglnF("V");

      mcp4725_setVoltage( (uint32_t) pwr_scale, false);
    }


    // Does MCP3421 is enabled
    if (!config.sensors.en_mcp3421) {
      DebuglnF("MCP3421 Disabled in config");
    } else {
      // Re init I2C Bus if needed
      if ((config.config & CFG_MCP3421) == 0) {
        i2c_init(false);
        mcp3421_init();
        delay(70);
      }

      // Start sampling if device found
      if ( config.config & CFG_MCP3421) {
        mcp3421_async_sample(250);
      }

    } // MCP3421 enabled
  }
}
