// Serial debug output
//#define SERIAL_OUT
//#define SERIAL_OUT_VERBOSE

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

// I2C pin definitions
#if defined(_DIS) || defined(_RTC) || defined(_BME) || defined(_KEY)
#define _I2C
#define SDA 	 ((uint8_t) 20)
#define SCL 	 ((uint8_t) 21)
#endif // defined(_DIS) || defined(_RTC) || defined(_BME) || defined(_KEY)

// SPI pin definitions
#if defined(_RAM) || defined(_MMC)
#define _SPI
 #ifdef _RAM
#define SS_RAM 53
 #endif
 #ifdef _MMC
#define SS_MMC	23
 #endif
#define MISO 	50
#define MOSI 	51
#define SCK 	52
#endif // defined(_RAM) || defined(_MMC)

// Sensor pins
#ifdef _SEN

#define COUNT_FLOOR 2
const uint8_t FLOOR_SENSE[COUNT_FLOOR] = { A0, A1 };

#define COUNT_HUMID 2
const uint8_t HUMID_SENSE[COUNT_HUMID] = { A2, A3 };

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
#define PH_PUMP 0
#define H2O_PUMP 1
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
                                                      { 41, 40, 42, 43 },	// NUT 2
                                                      { 37, 36, 38, 39 },	// NUT 1
                                                      { 33, 32, 34, 35 },	// NUT 4
                                                      { 67, 66, 68, 69 }};	// NUT 3

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
 #ifdef _RAM

//#include "SRAM_23LC.h"
//SRAM_23LC Ram(&SPI, SS_RAM, SRAM_23LC512);
 #endif // _RAM

 #ifdef _MMC
#include <SD.h>
Sd2Card  card;
SdVolume volume;
SdFile   root;

 #endif // _MMC

#endif // _SPI

// I2C device headers
#ifdef _I2C
#include <Wire.h>
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
 #ifdef _DIS
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// FIXME

  #ifdef _RAM
#include <Adafruit_SSD1306_ram.h>
  #else
#include <Adafruit_SSD1306.h>
  #endif // _RAM

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire,
  #ifdef _RAM
//&Ram, (uint32_t) 0,
  #endif // _RAM
OLED_RESET);
 #endif // _DIS
 
 #ifdef _BME
// Decalration for an BMP280 temperature, humidity, and pressure sensor
#include <Adafruit_BME280.h>
Adafruit_BME280 bme;
 #endif // _BME
 
 #ifdef _RTC
#include <TimeLib.h>
#include <DS1307RTC.h>
 #endif // _RTC

 #ifdef _KEY
#include <PCF8574.h>
PCF8574 keys((uint8_t) 0x20);
 #endif // _KEY

#endif // _I2C

#if defined(_NUT) || defined(_H2O)
#include "PumpStepper.h"
//#include <AccelStepper.h>
#include <MultiStepper.h>
#ifdef _NUT
PumpStepper nut[NUTRIENTS] = {};
MultiStepper nuts;
#endif // _NUT
PumpStepper stepper[COUNT_STEPPERS] = {};
MultiStepper steppers;
#endif // defined(_NUT) || defined(_H2O)

#ifdef _DO_HW_CHECK
// Hardware check status
bool failed_spi = false;
bool failed_ram = false;
bool failed_mmc = false;
bool failed_dis = false;
bool failed_bme = false;
bool failed_rtc = false;
bool failed_key = false;
#endif // DO_HW_CHECK

#ifdef _FLO
bool alarm_flow  = false;
#endif // _FLO

#ifdef _H2O
bool alarm_h2o   = false;
#endif // _H2O

#ifdef _NUT
bool alarm_nut   = false;
#endif // _NUT

#ifdef _SEN
bool alarm_flood = false;
int  alarm_soil  = 0;
bool alarm_env   = false;
volatile bool alarm_int = false;
char alarm_state[20] = "FLOODFLOWNUTSOILENV";
#endif // _SEN

#ifdef _DIS
unsigned long millis_display_loop;
unsigned long interv_display_loop = 3000;
#endif // _DIS

#ifdef _BME
char t_str[9];
char p_str[9];
char h_str[9];
char tph_str[32];

float t;
float p;
float h;
#endif // _BME

#define PULSE_PER_ML ((float) 925)

#define SEALEVELPRESSURE_HPA (1013.25)

#ifdef _KEY
int ROW[4] = {P0, P1, P2, P3};
int COL[4] = {P4, P5, P6, P7};

uint8_t key_[4][4] = {{ 1, 2,  3, 10}, 
                      { 4, 5,  6, 11},
                      { 7, 8,  9, 12},
                      {15, 0, 14, 13}};

#define KEY_NO_PRESS 0
#define KEY_NO_CHANGE 0

#define KEY_UP 2
#define KEY_LT 4
#define KEY_RT 6
#define KEY_DN 8

#define KEY_F1 11
#define KEY_F2 12
#define KEY_F3 13
#define KEY_F4 14

#define KEY_STOP  15
#define KEY_START 16

#define MENU_MAIN 0
#define MENU_MANUAL 1

float last_key = KEY_NO_PRESS;
float menu_pos = 0; // fmt: XX.YY
char menu_buff[8];
#endif // _KEY

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

#define WEEKS ((uint8_t) 13)
