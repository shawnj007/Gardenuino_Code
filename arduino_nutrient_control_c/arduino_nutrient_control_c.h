#ifndef ARDUINO_NUTRIENT_CONTROL_C_H
#define ARDUINO_NUTRIENT_CONTROL_C_H

//
// Standard Dependencies
// -----------------------
// Adafruit BME280		(Temperature, Humidity, Pressure Sensor)
// DS1307RTC			(Real-time Clock)
// TimeLib				(Time Macros)
// Adafruit PCF8574		(I/O extender, 4x4 keypad)
//
// Modified Dependencies
// -----------------------
// SRAM 23LC 			(Modified to invert SS and expose the Ram variable)
// Adafruit SSD_1306 	(Modifed to use RAM for display memory)
//
// Brand new Dependencies
// -----------------------
// Pump Stepper			(Syringe- and Peristaltic- pump control)
//

// Serial debug output
//#define SER_OUT
//#define SER_OUT_VERBOSE

// *****************
// HARDWARE enables
// *****************
#define _DIS  // Display
#define _RTC  // Real time clock
#define _BME  // Temperature/Pressure/Humidity
#define _KEY  // 4x4 key interface
#define _RAM  // Random Access Memory expansion
#define _MMC  // SD card reader
#define _SEN  // Sensors
#define _FLO  // Flow rate sensors for h2o (used for leak awareness, and h2o volume delivery watchdog, NOT used for nutrient delivery mixing)
#define _H2O  // Stepper control for h2o (used for nutrient delivery mixing)
#define _PWM  // Pulse width modulation (Motor speed) control for stirrers
#define _VAL  // Valves/Solenoids
#define _NUT  // Stepper control for nutrients
#define _DO_HW_CHECK

// *****************
// SOFTWARE enables
// *****************
#define _LOG
#define _MENU

// I2C pin definitions
#if defined(_DIS) || defined(_RTC) || defined(_BME) || defined(_KEY)
#define _I2C
#define SDA 	 ((uint8_t) 20)
#define SCL 	 ((uint8_t) 21)
#endif // defined(_DIS) || defined(_RTC) || defined(_BME) || defined(_KEY)

// SPI pin definitions
#if defined(_RAM) || defined(_MMC)
#define _SPI
 #ifdef _MMC
#define SS_MMC	23
 #endif
#define MISO 	50
#define MOSI 	51
#define SCK 	52
 #ifdef _RAM
#define SS_RAM  53
 #endif
#endif // defined(_RAM) || defined(_MMC)

// Sensor pins
#ifdef _SEN

#define COUNT_HUMID 2
const uint8_t HUMID_SENSE[COUNT_HUMID] 		= { A0, A1 };

#define COUNT_FLOOR 2
const uint8_t FLOOR_SENSE[COUNT_FLOOR] 		= { A2, A3 };

#define COUNT_PANS 4
const uint8_t PANS_SENSE[COUNT_PANS] 		= { A4, A5, A6, A7 };

#define COUNT_PLANTS 4
const uint8_t PLANTS_SENSE[COUNT_PLANTS] 	= { A8, A9, A10, A11 };

#define COUNT_SENSORS ((unsigned byte) 12)
const uint8_t SENSORS[COUNT_SENSORS] 		= { A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11 };

#endif // _SEN

// Flow rate pins
#ifdef _FLO
#define COUNT_FLOW_RATE 2
const uint8_t FLOW_RATE[COUNT_FLOW_RATE] 	= { 2, 3 };
#endif // _FLO

// Valve pins
#ifdef _VAL
#define COUNT_VALVES 6
const uint8_t VALVES[COUNT_VALVES] 			= { 4, 5, 6, 7, 8, 9 };
#endif // _VAL

// Motor pins
#ifdef _PWM
#define COUNT_PWM 1
const uint8_t PWM[COUNT_PWM] 				= { 10 };
#endif // _PWM

// Stepper pins
#if defined(_NUT) || defined(_H2O)

 #ifdef _NUT
#define H2O_PUMP 0
#define NUT_START 1	// for screen display
#define NUTRIENTS 5
#define LARGE_SYRINGES 4
#define PH_PUMP 5
 #endif

#define MAXSPEED 1000
#define COUNT_STEPPERS 6
#define WIRES 4

const uint8_t STEPPER_WIRES[COUNT_STEPPERS][WIRES] = { { 49, 48, 30, 31 },	 // H2O
                                                       { 40, 41, 42, 43 },	 // NUT 1
                                                       { 36, 37, 38, 39 },	 // NUT 2
                                                       { 32, 33, 34, 35 },	 // NUT 3
                                                       { 66, 67, 68, 69 }, 	 // NUT 4
                                                       { 44, 45, 46, 47 }	 // PH UP	- PWM
                                                       }; 

/* TODO: Change in 0.1e PCB version
const uint8_t STEPPER_WIRES[COUNT_STEPPERS][WIRES] = {
                                                      { 49, 48, 30, 31 },	// H2O
                                                      { 36, 37, 38, 39 },	// NUT 1
                                                      { 40, 41, 42, 43 },	// NUT 2
                                                      { 66, 67, 68, 69 },	// NUT 3
                                                      { 32, 33, 34, 35 },	// NUT 4
                                                      { 44, 45, 46, 47 }	// PH UP	- PWM
                                                      };
*/
#endif // defined(_NUT) || defined(_H2O)

// SPI device headers
#ifdef _SPI
#include <SPI.h>

 #if ( defined(_RAM) && !defined(_DIS) )
#include "SRAM_23LC.h"
 #endif // _RAM

 #ifdef _MMC
#include <SD.h>
 #endif // _MMC
 
#endif // _SPI

// I2C device headers
#ifdef _I2C
#include <Wire.h>

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
 #ifdef _DIS
  #ifdef _RAM
#include <Adafruit_SSD1306_ram.h>
  #else
#include <Adafruit_SSD1306.h>
  #endif // _RAM
  
 #endif // _DIS
 
 #ifdef _BME
// Decalration for an BMP280 temperature, humidity, and pressure sensor
#include <Adafruit_BME280.h>
 #endif // _BME
 
 #ifdef _RTC
#include <TimeLib.h>
#include <DS1307RTC.h>
 #endif // _RTC

 #ifdef _KEY
#include <PCF8574.h>
 #endif // _KEY

#endif // _I2C

#if defined(_NUT) || defined(_H2O)
#include "PumpStepper.h"
//#include <AccelStepper.h>
#include <MultiStepper.h>
#endif // defined(_NUT) || defined(_H2O)

#define PULSE_PER_ML ((float) 925)

#define SEALEVELPRESSURE_HPA (1013.25)

#define SAMPLE_COUNT 10

#define FLOW_RATE_MIN ((float) 0.3)
#define FLOW_RATE_MAX ((float) 3.0)

// TODO Needs calibrated values
#define SOIL_MOIST_MIN ((uint8_t) 300)	// Require un-scheduled water if below this value (no nutrient)
#define SOIL_MOIST_MAX ((uint8_t) 600)	// Inhibit scheduled watering if soil starts out this high

#ifdef _LOG
#define LOG_CACHE_SIZE 10
#endif

#define POSITION (27000) // 27,000 pulses = ? ticks = 50mL

#define ZONES (uint8_t) 1

#endif // ARDUINO_NUTRIENT_CONTROL_C_H
