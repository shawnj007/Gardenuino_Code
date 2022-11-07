#ifndef ARDUINO_NUTRIENT_CONTROL_GLOBALS_H
#define ARDUINO_NUTRIENT_CONTROL_GLOBALS_H

#if (defined(_SPI) && defined(_RAM) && !defined(_DIS))
SRAM_23LC Ram(&SPI, SS_RAM, SRAM_23LC512);
#endif

#ifdef _MMC
Sd2Card card;
SdVolume volume;
SdFile root;
#endif

#if defined(_NUT) || defined(_H2O)
PumpStepper nut[NUTRIENTS] = {};
MultiStepper nuts;
PumpStepper stepper[COUNT_STEPPERS] = {};
MultiStepper steppers;
#endif	// defined(_NUT) || defined(_H2O)

#ifdef _KEY
PCF8574 keys((uint8_t)0x20);
#endif

#ifdef _BME
Adafruit_BME280 bme;
#endif

#ifdef _DIS
 #define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
 #define SCREEN_WIDTH 128	  // OLED display width, in pixels
 #define SCREEN_HEIGHT 64	  // OLED display height, in pixels
 #define OLED_RESET -1		  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire,
 #ifdef _RAM
		//&Ram, (uint32_t) 0,
 #endif	 // _RAM
						 OLED_RESET);
#endif

#define WEEKS ((uint8_t)13)

// total mL
extern int opt_h2o_amt[WEEKS];

// cal_factor = (ticks / mL)
//float cal_factor[2] = { 4.0, 4.0 };

// global status
extern bool running_h2o;

#ifdef _NUT
// global status
extern bool running_nut;

	// in mL per 1L
 #define ML_L 1000
extern float opt_nut_ratio[NUTRIENTS][WEEKS];

// remaining nutrients
extern float opt_nut_rem[NUTRIENTS];
#endif

//float cal_factor[2] = { 0.4875, 0.4875 };  // cal_factor = (ticks / mL)

// in various units
/*
int opt_tds[13]  = {490, 490, 1365, 1785, 1925, 1925, 2030, 2030, 2205, 1855, 1470, 1085, 1085};
float opt_ph[13] = {6.3, 6.3, 6.3, 6.4, 6.4, 6.5, 6.5, 6.5, 6.3, 6.6, 6.5, 6.4, 6.4};
float opt_ec[13] = {0.7, 0.7, 1.9, 2.5, 2.7, 2.7, 2.9, 2.9, 3.1, 2.6, 2.1, 1.5, 1.5};
*/

//int opt_next_sch = 0;			// Incremented every 168 hours, not to go above 12

#ifdef _H2O
float h2o_rate = 0;
unsigned long h2o_millis = 0;
#endif	// _H2O

#ifdef _SEN
float sense_ph;
float sense_tds;
float sense_ec;
float sense_dox;
#endif

#ifdef _RTC
bool SET_RTC = false;
tmElements_t check_tm;
tmElements_t tm;

char dt_str[9] = "YY/MM/DD";
char tm_str[9] = "hh:mm:ss";
char dttm_str[20] = "YYYY/MM/DD hh:mm:ss";

unsigned long interv_h2o = 60;	// in seconds
unsigned long interv_nut = 60;

unsigned long millis_rtc;
unsigned long millis_interv_rtc = 125;

unsigned long millis_interv_sch = 6000;
unsigned long millis_sch;
#endif

#ifdef _DO_HW_CHECK
// Hardware check status
bool failed_spi = false;
bool failed_ram = false;
bool failed_mmc = false;
bool failed_dis = false;
bool failed_bme = false;
bool failed_rtc = false;
bool failed_key = false;
#endif	// DO_HW_CHECK

#ifdef _FLO
bool alarm_flow = false;
#endif	// _FLO

#ifdef _H2O
bool alarm_h2o = false;
#endif	// _H2O

#ifdef _NUT
bool alarm_nut = false;
#endif	// _NUT

#ifdef _SEN
bool alarm_flood = false;
int alarm_soil = 0;
bool alarm_env = false;
volatile bool alarm_int = false;
char alarm_state[20];	// = "FLOODFLOWNUTSOILENV";
char alarm_str[3][10];	// = {"FLOOD ENV",
						//    "FLOW SOIL",
						//    "NUT SUPPL"};
#endif					// _SEN

#ifdef _DIS
unsigned long millis_display_loop;
unsigned long interv_display_loop = 150;
#endif	// _DIS

#ifdef _BME
char t_str[9];
char p_str[9];
char h_str[9];
char tph_str[32];

float t;
float p;
float h;
#endif	// _BME

#ifdef _LOG
unsigned long millis_cache_log;
unsigned long millis_interv_cache_log = 1000;

unsigned long millis_log;
unsigned long millis_interv_log = 10000;

char log_entry[166];
char log_string[LOG_CACHE_SIZE][46] = {};
volatile uint8_t s = 0;
#endif

char fil_output[11] = {};
char ser_output[64] = {};
//char dis_output[32] = { };
//char sys_status[24] = { };

#ifdef _FLO
volatile unsigned long h2o_ticks[2] = { 0, 0 };
volatile unsigned long h2o_last_millis[2] = { 0, 0 };

volatile unsigned long h2o_ticks_total[2] = { 0, 0 };
volatile unsigned long h2o_millis_total[2] = { 0, 0 };
#endif

#endif	// ARDUINO_NUTRIENT_CONTROL_GLOBALS_H