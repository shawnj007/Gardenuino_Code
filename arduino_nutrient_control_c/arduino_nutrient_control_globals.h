// total mL
int opt_h2o_amt[WEEKS]   = { 200, 300, 500, 750, 1000, 1000, 1500, 1500, 1500, 1500, 1200, 1000, 1000 };
float cal_factor[2] = { 4.0, 4.0 };  // cal_factor = (ticks / mL)

bool running_h2o = false;

#ifdef _NUT
// in mL per 1L
#define ML_L 1000
float opt_nut_ratio[NUTRIENTS][WEEKS] = { { 7.8, 7.8, 7.8, 7.8, 3.9, 3.9, 3.9, 3.9, 3.9, 3.9, 3.9, 3.9, 3.9 },
									      { 0.0, 0.0, 2.6, 3.9, 3.9, 2.6, 2.6, 2.6, 2.6, 1.3, 0.0, 0.0, 0.0 },
									      { 0.0, 0.0, 0.0, 0.0, 0.0, 2.6, 2.6, 2.6, 2.6, 2.6, 2.6, 1.3, 1.3 },
							 		      { 1.0, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 0.8, 0.6 },
							 		      { 0.8, 0.8, 0.8, 0.8, 0.8, 0.9, 0.9, 0.9, 0.9, 0.8, 0.8, 0.8, 0.8 } }; /*,
							 		      { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 } }; */

// remaining nutrients
float opt_nut_rem[NUTRIENTS] = { 946.0, 946.0, 946.0, 568.0, 237.0 };
bool running_nut = false;
#endif

//float cal_factor[2] = { 0.4875, 0.4875 };  // cal_factor = (ticks / mL)

// in various units 
/*
int opt_tds[13]         = {490, 490, 1365, 1785, 1925, 1925, 2030, 2030, 2205, 1855, 1470, 1085, 1085};
float opt_ph[13]        = {6.3, 6.3, 6.3, 6.4, 6.4, 6.5, 6.5, 6.5, 6.3, 6.6, 6.5, 6.4, 6.4};
float opt_ec[13]        = {0.7, 0.7, 1.9, 2.5, 2.7, 2.7, 2.9, 2.9, 3.1, 2.6, 2.1, 1.5, 1.5};
*/

//int opt_next_sch = 0;			// Incremented every 168 hours, not to go above 12

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

char tm_str[11]   = "YYYY/MM/DD";
char dt_str[9]    = "hh:mm:ss";
char dttm_str[20] = "YYYY/MM/DD hh:mm:ss";

unsigned long interv_h2o = 60; // in seconds
unsigned long interv_nut = 60;

unsigned long millis_rtc;
unsigned long millis_interv_rtc = 200;

unsigned long millis_interv_sch = 60000;
unsigned long millis_sch;
#endif

#ifdef _LOG
unsigned long millis_cache_log;
unsigned long millis_interv_cache_log = 1000;

unsigned long millis_log;
unsigned long millis_interv_log = 10000;

char log_entry[166];
char log_string[LOG_CACHE_SIZE][46] = { };
volatile uint8_t s = 0;
#endif

char fil_output[11] = { };
char ser_output[64] = { };
//char dis_output[32] = { };
//char sys_status[24] = { };

#ifdef _FLO
volatile unsigned long h2o_ticks[2] = { 0, 0 };
volatile unsigned long h2o_last_millis[2] = { 0, 0 };

volatile unsigned long h2o_ticks_total[2] = { 0, 0 };
volatile unsigned long h2o_millis_total[2] = { 0, 0 };
#endif
