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

/* Pin assignments
 
 Arduino Mega 2560 

VIN			
VIN			
GND			
GND			
5V			
5V			
3V3			
3V3			
AREF			
RST			
	 nc	0	RX			
	 nc	1	TX			
INT4	2	D2	PWM		FLOW1
INT5	3	D3	PWM		Interrupt
		4	D4	PWM	Q1	12V   7   BRs
		5	D5	PWM	Q2	12V   8   BR
		6	D6	PWM	Q3	12V   6   G
		7	D7	PWM	Q4	12V   4   B
		8	D8	PWM	Q5	12V   3   O
		9	D9	PWM	Q6 	12V   1   Os
		10	D10	PWM	Q7	12V   2   Gs
	 	11	D11	PWM 		LED1
		12	D12	PWM 		OPT(_EXT)
	 	13	D13	PWM	->	 	D47
		14	D14	TX	XDA		I2C auxiliary Data
		15	D15	RX			(OPT)
	 	16	D16		XCL		I2C auxiliary Clock
	 	17	D17		GPSRX	GPS recieve
INT3 	18	D18		PPS		GPS pulse per second
INT2 	19	D19		GPSTX	GPS transmit
INT1	20	D20		SDA		I2C Data
INT0	21	D21		SCL		I2C Clock
	 	22	D22		SS_EXT	SPI external chip select 
		23	D23		SS_INT	SPI internal MMC chip select / SPI Ram chip select
	 nc	24	D24
	 nc	25	D25
	 nc	26	D26
	 nc	27	D27
	 nc	28	D28		
	 nc	29	D29		
	 	30	D30		J2-3	Motor 0 -3
		31	D31		J2-4	Motor 0 -4
		32	D32		J6-1	Motor 4 -1	Nutrient 3
		33	D33		J6-2	Motor 4 -2
		34	D34		J6-3	Motor 4 -3
		35	D35		J6-4	Motor 4 -4
		36	D36		J3-1	Motor 3 -1	Nutrient 2
		37	D37		J3-2	Motor 3 -2
		38	D38		J3-3	Motor 3 -3
		39	D39		J3-4	Motor 3 -4
		40	D40		J4-1	Motor 2 -1	Nutrient 1
		41	D41		J4-2	Motor 2 -2
		42	D42		J4-3	Motor 2 -3
		43	D43		J4-4	Motor 2 -4
		44	D44	PWM	J1-1	Motor 1 -1	pH
		45	D45	PWM	J1-2	Motor 1 -2
		46	D46	PWM	J1-3	Motor 1 -3
		47	D47	D13	J1-4	Motor 1 -4
		48	D48		J2-2	Motor 0 -2
		49	D49		J2-1	Motor 0 -1	H2O
		50	D50		MISO	Slave out
		51	D51		MOSI	Slave in
		52	D52		SCK		Clock
	 nc	53	D53		CS						* Not used for routing reasons
		54	A0		JS2-7	Sense 0
		55	A1		JS2-6	Sense 1
		56	A2		JS2-5	Sense 2
		57	A3		JS2-4	Sense 3
		58	A4		JS2-3	Sense 4
		59	A5		JS2-2	Sense 5
		60	A6		JS1-7	Sense 6
		61	A7		JS1-6	Sense 7
		62	A8		JS1-5	Sense 8
		63	A9		JS1-4	Sense 9
		64	A10		JS1-3	Sense 10
		65	A11		JS1-2	Sense 11
		66	A12		J5-1	Motor 5 -1	Nutrient 4
		67	A13		J5-2	Motor 5 -2
		68	A14		J5-3	Motor 5 -3
		69	A15		J5-4	Motor 5 -4
 */

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
const uint8_t HUMID_SENSE[COUNT_HUMID] = { A0, A1 };

#define COUNT_FLOOR 2
const uint8_t FLOOR_SENSE[COUNT_FLOOR] = { A2, A3 };

#define COUNT_PANS 4
const uint8_t PANS_SENSE[COUNT_PANS] = { A4, A5, A6, A7 };

#define COUNT_PLANTS 4
const uint8_t PLANTS_SENSE[COUNT_PLANTS] = { A8, A9, A10, A11 };

#define COUNT_SENSORS ((unsigned byte) 12)
const uint8_t SENSORS[COUNT_SENSORS] = { A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11 };
#endif // _SEN

// Flow rate pins
#ifdef _FLO
#define COUNT_FLOW_RATE 2
const uint8_t FLOW_RATE[COUNT_FLOW_RATE] = { 2, 3 };
#endif // _FLO

// Valve pins
#ifdef _VAL
#define COUNT_VALVES 6
const uint8_t VALVES[COUNT_VALVES] = { 4, 5, 6, 7, 8, 9 };
#endif // _VAL

// Motor pins
#ifdef _PWM
#define COUNT_PWM 1
const uint8_t PWM[COUNT_PWM] = { 10 };
#endif // _PWM

// Stepper pins
#if defined(_NUT) || defined(_H2O)

 #ifdef _NUT
#define H2O_PUMP 0
#define PH_PUMP 1
#define NUT_START 1	// for screen display
#define NUTRIENTS 5
 #endif

#define MAXSPEED 1000
#define COUNT_STEPPERS 6
#define WIRES 4
/*
const uint8_t STEPPER_WIRES[COUNT_STEPPERS][WIRES] = {{ 48, 49, 30, 31 },	// H2O
                                                      { 44, 45, 46, 47 },	// PH UP	- PWM
                                                      { 40, 41, 42, 43 },	// NUT 2
                                                      { 36, 37, 38, 39 },	// NUT 1
                                                      { 32, 33, 34, 35 },	// NUT 4
                                                      { 66, 67, 68, 69 }};	// NUT 3
*/                                                    

// Note reversed order of two pins to reverse syringe pump direction
const uint8_t STEPPER_WIRES[COUNT_STEPPERS][WIRES] = {{ 48, 49, 30, 31 },	// H2O
                                                      { 45, 44, 46, 47 },	// PH UP	- PWM
                                                      { 41, 40, 42, 43 },	// NUT 1
                                                      { 37, 36, 38, 39 },	// NUT 2
                                                      { 33, 32, 34, 35 },	// NUT 3
                                                      { 67, 66, 68, 69 }};	// NUT 4

/* TODO: Change in 0.1e PCB version
const uint8_t STEPPER_WIRES[COUNT_STEPPERS][WIRES] = {
                                                      { 44, 45, 46, 47 },	// PH UP	- PWM
                                                      { 49, 48, 30, 31 },	// H2O
                                                      { 36, 37, 38, 39 },	// NUT 1
                                                      { 40, 41, 42, 43 },	// NUT 2
                                                      { 66, 67, 68, 69 },	// NUT 3
                                                      { 32, 33, 34, 35 }	// NUT 4
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
