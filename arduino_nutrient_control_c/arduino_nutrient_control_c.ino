
#include "arduino_nutrient_control_c.h"
#include "arduino_nutrient_control_globals.h"

#include "arduino_nutrient_print.c"
#include "arduino_nutrient_display.c"
#include "arduino_nutrient_rtc.c"
#include "arduino_nutrient_mmc.c"
#include "arduino_nutrient_bme.c"
#include "arduino_nutrient_key.c"
#include "arduino_nutrient_sen.c"
#include "arduino_nutrient_out.c"

// TODO System leak detection
// TODO Low flow detection

#ifdef _I2C
void setup_i2c() {
 #ifdef _DIS
	// Display
	setup_display();
 #endif

 #ifdef _RTC
	// Real time clock
	setup_rtc();
 #endif

 #ifdef _BME
	// THP sensor
	setup_thp();
 #endif

 #ifdef _KEY
	// Key interface
	setup_key();
 #endif
}
#endif // _I2C

#ifdef _RAM
void setup_ram() {
 #ifdef SERIAL_OUT
	Serial.println(F("Setting up RAM"));
 #endif
	uint32_t address = 0;
	uint8_t set_byte = 175;
	//size_t ret = 
	
	Ram.begin();
 #ifdef SERIAL_OUT_VERBOSE
	Serial.print(F("set test value\n"));
	Serial.println(set_byte);
 #endif
	Ram.writeByte(address, set_byte);
	
	uint8_t get_byte = Ram.readByte(address);
 #ifdef SERIAL_OUT_VERBOSE
	Serial.print(F("get test value\n"));
	Serial.println(get_byte);
 #endif
	if (set_byte == get_byte) {
	
 #ifdef SERIAL_OUT
		Serial.println(F("RAM byte read/write passed"));
 #endif
		size_t BUFFER_SIZE = 32;
		uint8_t get_buffer[BUFFER_SIZE] = { };
		uint8_t set_buffer[BUFFER_SIZE] = { };
		
		//uint8_t j = 0;

 #ifdef SERIAL_OUT_VERBOSE
		Serial.print(F("set test block:\n"));
 #endif
		for (size_t i = 0; i < BUFFER_SIZE; i++) {
			set_buffer[i] = i;
			//Serial.println(set_buffer[i]);
		}
		
		for (int i = 0; i < 64; ++i) {
			// write random buffer to block address
			Ram.writeBlock(i * BUFFER_SIZE, BUFFER_SIZE, set_buffer);
			// read  block address
			Ram.readBlock(i * BUFFER_SIZE, BUFFER_SIZE, get_buffer);
			// compare write buffer and read buffer
			
			//Serial.print(i);
			//Serial.print(F(" test block: >"));
			for (size_t j = 0; j < BUFFER_SIZE; ++j) {
				//Serial.print(get_buffer[j]);
				if (get_buffer[j] != set_buffer[j]) {
 #ifdef SERIAL_OUT_VERBOSE
					Serial.println(F("FAILED: Ram test"));
 #endif
 #ifdef _DO_HW_CHECK
					failed_ram = true;
 #endif
				} 
			}
			//Serial.println(F("<"));
		}
		
		//size_t ret = 
 #ifdef SERIAL_OUT
		if (!failed_ram) Serial.println(F("RAM block read/write passed"));
 #endif
	} else {
 #ifdef SERIAL_OUT
		Serial.println(F("FAILED: RAM byte read/write failed"));
 #endif
 #ifdef _DO_HW_CHECK
		failed_ram = true;
 #endif
	}
}

#endif // _RAM

#ifdef _SPI
void setup_spi() {
 #ifdef SERIAL_OUT_VERBOSE
	Serial.println(F("Setting up SPI bus"));
 #endif // SERIAL_OUT_VERBOSE

 #ifdef _MMC
	// SD card interface
	setup_mmc();
 #endif // _MMC

 #ifdef _RAM
	// RAM
	//pinMode(SS_RAM, INPUT_PULLUP);
	//digitalWrite(SS_RAM, HIGH);
	setup_ram();
 #else
 #endif // _RAM
 
 #ifdef SERIAL_OUT
	Serial.println(F("Done with SPI bus"));
 #endif // SERIAL_OUT
}
#endif // _SPI

void setup() {
#ifdef SERIAL_OUT
	Serial.begin(230400);
	Serial.println(F("Setup Start"));
#endif // SERIAL_OUT

#ifdef _SPI
	setup_spi();
#endif

#ifdef _I2C
	setup_i2c();
#endif

#ifdef _NUT
	setup_steppers();
#endif

#ifdef _SEN
	setup_sensors();
#endif

#ifdef _PWM
	setup_pwm_power();
#endif

#ifdef _VAL
	setup_valves();
#endif

#ifdef _FLO
	setup_flow_rate();
#endif

#ifdef SERIAL_OUT
	Serial.println(F("Setup Done"));
#endif

#ifdef _MMC	
	read_config_files();
#endif

#ifdef SERIAL_OUT
	Serial.println(F("Config Done"));
#endif

}

bool need_to_run = true;
bool need_h2o = false;
bool need_nut = false;

#ifdef _RTC
time_t time_now;
time_t time_sta;
time_t time_h2o;
time_t time_nut;

char times[3][20] = { "1970/01/01 00:00:00", "1970/01/01 00:00:00", "1970/01/01 00:00:00" };

 #if defined(_NUT) || defined(_FLO)
int check_schedule(int zone) {
	//bool do_run = need_to_run;
	//need_to_run = false;
	
  #ifdef _MMC
	read_zone_file("zone", zone, times);
  #endif // _MMC
	
	// read zone file to variables
	time_now = getDateTime(dttm_str);
	time_sta = getDateTime(times[0]);
	time_h2o = getDateTime(times[1]);
	time_nut = getDateTime(times[2]);
	
	// Compute which scheduled week to run, verify in range
	int week = (int) ((time_now - time_sta) / SECS_PER_WEEK);
	if (week < 0 || week >= WEEKS) {
  #ifdef SERIAL_OUT
		sprintf(ser_output, "Zone: %d - Week computed as: %d\n", zone, week);
		Serial.println(ser_output);
  #endif // SERIAL_OUT
		return -1; // TODO set alarm
	}

	// check if enough time has passed to water
	time_t time_since_h2o = time_now - time_h2o;
	need_h2o = time_since_h2o >= interv_h2o;
	
	// check if enough time has passed to feed
	time_t time_since_nut = time_now - time_nut;
	need_nut = time_since_nut >= interv_nut;

	// check if enough time has passed since low flow rate
	// TODO
	
  #ifdef SERIAL_OUT
	Serial.print(F("Weeks since zone start : "));
	Serial.println(week);
	
	Serial.print(F("Seconds since last h2o : "));
	Serial.println(time_since_h2o);
	
	Serial.print(F("Seconds since last nut : "));
	Serial.println(time_since_nut);
  #endif // SERIAL_OUT
	if (need_h2o) {
		return week;
	} else {
		return -1;
	}
}
 #endif // defined(_NUT) || defined(_FLO)
#endif // _RTC

char sch_display[6][22] = { };

unsigned long millis_run_loop = 0;
unsigned long interv_run_loop = 300;

#ifdef _RTC
 #if defined(_NUT) || defined(_FLO)
bool execute_schedule(int zone, int week) {
  #ifdef SERIAL_OUT
	sprintf(ser_output, "Starting zone %d week %d", zone, week);
	Serial.println(ser_output);
  #endif // SERIAL_OUT

  #ifdef _SEN
	// Check alarms, do nothing if any are set
	if (check_alarms()) return false;
  #endif // _SEN
	
  #if defined(_MMC) && defined(_LOG)
	// Record start
	loop_rtc();
	sprintf(log_entry, "\n%s START\n", dttm_str);
	write_zone_log_file("zone", zone, log_entry);
  #endif // defined(_MMC) && defined(_LOG)

  #ifdef _PWM
	// Enable mixers
	enable_mixers();
	// Allow mixers time to spin up and mix nutrients
	// TODO
  #endif // _PWM

	//PumpStepper h2o = stepper[0];

	// Calculate required h2o and nutrient volume based on total h2o delivery
	float h2o_amt = (float) opt_h2o_amt[week];
	Serial.print(F("H2O "));
	Serial.print(print_float(h2o_amt));
	Serial.println(" mL");
	// Calculate maximum h2o and nutrient pump rates, determine limiting rate
	// For now, assume h2o is limiting
	float h2o_time = 120; //h2o.getTimeToDispense(h2o_amt); // FIXME?
	float h2o_rate = (float) h2o_amt / (float) h2o_time;
	
	// Setup continuous pump dicharge parameters
	//stepper[0].setCurrentPosition(0);
	stepper[0].setVolumeTime(h2o_amt, h2o_time * 0.82);
   #ifdef SERIAL_OUT
	Serial.print(F("H2O "));
	Serial.print(print_float(h2o_amt / (float) h2o_time));
	Serial.println(" mL/sec");
   #endif
   
	float nut_amt[NUTRIENTS] = {};
	for (int n = 0; n < NUTRIENTS; ++n) {
		nut_amt[n] = h2o_amt * opt_nut_ratio[n][week] / ML_L;
   #ifdef SERIAL_OUT
		Serial.print(F("Nutrient "));
		Serial.print(n);
		Serial.print(" ");
		Serial.print(print_float(nut_amt[n]));
		Serial.println(" mL");
   #endif
	}	

	// Calculate fill rate for each syringe pump and fill it in minimum time.
	for (int n = 0; n < NUTRIENTS; ++n) {
		float n_time = nut[n].getTimeToDispense(nut_amt[n]);
		nut[n].setVolumeTime(-(nut_amt[n]), n_time);	// Negative volume loads syringe pump
		
   #ifdef SERIAL_OUT
		Serial.print(F("Fill Nutrient "));
		Serial.print(n);
		Serial.print(" ");
		Serial.print(print_float(nut_amt[n] / n_time));
		Serial.println(" mL/sec");
   #endif
	}
	
	unsigned long time_fill = millis();
	// Fill syringe pumps and prepare to dispense
	bool loading;
  #ifdef SERIAL_OUT_VERBOSE
	int count = 0;
  #endif
	do {
		loading = false;
		for (int n = 0; n < NUTRIENTS; ++n) {
			loading = nut[n].runSpeedToPositionToStop() | loading;
		}
		
  #ifdef SERIAL_OUT_VERBOSE
		if (count % 1000 == 0) {
			for (int n = 0; n < NUTRIENTS; n++) {
				Serial.print(" ");
				float nut_amt_dis = nut[n].getDispensedVolume();
				long nut_currentPosition = (long) nut[n].currentPosition();
				Serial.print("| ");
				char nut_currentPositionStr[6] = {};
				sprintf(nut_currentPositionStr, "%5ld", nut_currentPosition);
				Serial.print(nut_currentPositionStr);
				Serial.print(" ");
				Serial.print(nut_amt_dis);
			}
			Serial.println("");
		}
		++count;
  #endif
		/*
		for (int n = 0; n < NUTRIENTS; ++n) {
			Serial.print(" ");
			Serial.print(nut[n].currentPosition());
			Serial.print(" ");
			Serial.print(nut[n].getDispensedVolume());
		}
		Serial.println();
		*/
	} while (loading);
	
  #ifdef SERIAL_OUT
	Serial.print("Fill took ");
	Serial.print(millis() - time_fill);
	Serial.print(" millis.. ");
  #endif
	
   #ifdef _PWM
	disable_mixers();
   #endif // _PWM
	
	Serial.println("Done Loading");
	// Setup syringe dicharge parameters	
	// Calculate volumes for phase 1, 2, and 3
		// phase 1, just water, 1/5 of water volume
		// phase ., nuts water, 3/5 of water volume, all nutrients
		// phase 5, just water, 1/5 of water volume, flush lines of nutrients
	int nut_time = floor(h2o_time * (3.0 / 5.0));
	float s_rate[NUTRIENTS] = {};
	for (int n = 0; n < NUTRIENTS; ++n) {
		nut[n].setVolumeTime(nut_amt[n], nut_time);	// Positive volume empties syringe pump
		// TODO check that max discharge rate is not exceeded and if it is, deal with it
		
   #ifdef SERIAL_OUT
		Serial.print(F("Discharge Nutrient "));
		Serial.print(n);
		Serial.print(" ");
		Serial.print(print_float(nut_amt[n] / (float) nut_time));
		Serial.println(" mL/sec");
   #endif
	}
	
  #ifdef SERIAL_OUT
	Serial.println("Setup Discharge");
  #endif
	
  #ifdef _SEN
	//bool alarm = false;
	//int alarm_code = 0;
  #endif // _SEN

	bool running_h2o;
	float h2o_amt_dis = 0;

	// open valve
	open_valve(zone);

	float nut_amt_dis[NUTRIENTS] = {0};
  #ifdef SERIAL_OUT_VERBOSE
	long nut_currentPosition[NUTRIENTS] = {0};
  #endif
	
	unsigned long time_spill = millis();
	do {

		// TODO measure pH and adjust rate for final nutrient (pH-Up)

		// TODO Check alarms

		// TODO Log output
		
		// TODO measure pH, TDS, EC, dOx, 
		
		// Run continuous stepper pump (h2o)
		running_h2o = stepper[0].runSpeedToPositionToStop();
		h2o_amt_dis = stepper[0].getDispensedVolume();

		// determine phase
		int phase = ceil(( h2o_amt_dis / h2o_amt ) * 5.0);

		// Run syringe stepper pumps (nutrients) until empty (~ beginning last phase)
		if (phase > 1) {
			for (int n = 0; n < NUTRIENTS; n++) {
		  		nut[n].runSpeedToPositionToStop();
			}
		}
		
  #ifdef _DIS
		if (millis_display_loop + interv_display_loop < millis()) {
			for (int n = 0; n < NUTRIENTS; n++) {
   #ifdef SERIAL_OUT_VERBOSE
				nut_currentPosition[n] = (long) nut[n].currentPosition();
   #endif
				nut_amt_dis[n] = nut[n].getDispensedVolume();
			}
  
   #ifdef SERIAL_OUT_VERBOSE
			// Do less frequent Serial updates
			long currentPosition = (long) stepper[0].currentPosition();
			
			if (currentPosition % 200 == 0) {
				Serial.print(" ");
				char currentPositionStr[6] = {};
				for (int n = 0; n < NUTRIENTS; n++) {
					Serial.print("| ");
					char nut_currentPositionStr[6] = {};
					sprintf(nut_currentPositionStr, "%5ld", nut_currentPosition[n]);
					Serial.print(nut_currentPositionStr);
					Serial.print(" ");
					Serial.print(nut_amt_dis[n]);
				}
				Serial.print(" | ");
				sprintf(currentPositionStr, "%5ld", currentPosition);
				Serial.print(currentPositionStr);
				Serial.print(" ");
				Serial.print(h2o_amt_dis);
				Serial.println();
			}
   #endif
		}

		// TODO record measurements and h2o_rate, h2o_amt, nut_n_rate, nut_n_amt
		//float h2o_rate = (float) h2o_amt / (float) h2o_time;
		float h2o_rate_real = (float) h2o_amt_dis / ((float) (millis() - time_spill) / 1000.0);
		float h2o_rem_amt = h2o_amt - h2o_amt_dis;
		
		// Display to i2c
		if (millis_display_loop + interv_display_loop < millis()) {
			millis_display_loop = millis();
			water_loop(nut_amt_dis, 
						sch_display,
						h2o_rate_real,
						h2o_rate,
						h2o_rem_amt,
						h2o_amt,
						phase, 
						s_rate,
						week,
						zone);
		}
						
		// TODO log sensor readouts to MMC
		
  #endif
	} while (running_h2o);

  #ifdef SERIAL_OUT
	Serial.print("Spill took ");
	Serial.print(millis() - time_spill);
	Serial.print(" millis.. ");
	
	Serial.println("Done Discharging");
  #endif
	// ensure closed valves
	open_valve(-1);

  #if defined(_MMC) && defined(_LOG)
	// Record stop
	loop_rtc();
	sprintf(log_entry, "%s STOP\n", dttm_str);
	write_zone_log_file("zone", zone, log_entry);
	
	char new_times[3][20] = { "1970/01/01 00:00:00", "1970/01/01 00:00:00", "1970/01/01 00:00:00" };
	sprintf(new_times[0], "%s", times[0]);
	for (int i = 1; i < 3; ++i) sprintf(new_times[i], "%s", dttm_str);
	for (int i = 0; i < 3; ++i) new_times[i][19] = '\0';
	write_zone_file("zone", zone, new_times);
  #endif // defined(_MMC) && defined(_LOG)

  #ifdef _NUT
	// Update nutrient amounts
	for (int n = 0; n < NUTRIENTS; ++n) {
		// Calculate dispensed nutrient amounts and write to file
		opt_nut_rem[n] -= nut[n].currentPosition() / PULSE_PER_ML;
		
   #if defined(_MMC) && defined(_LOG)
		write_nut_rem_file(n);
   #endif // defined(_MMC) && defined(_LOG)
	}

	disable_steppers();
  #endif // _NUT
   
	return true;
}
 #endif // defined(_NUT) || defined(_FLO)
#endif // _RTC

void loop() {

#ifdef _RTC
	loop_rtc();
#endif // _RTC

#ifdef _SEN
	// check inputs, alarms, and environmental sensors
	check_alarms();
	check_inputs();
#endif // _SEN

#ifdef _BME
	sensor_loop();
#endif // _BME

#if defined (_DIS) && ( defined(_RTC) || defined(_SEN) || defined(_BME) )
	// display sensor readouts
	display_loop();
#endif // defined (_DIS) && ( defined(_RTC) || defined(_SEN) || defined(_BME) )

	// check key (and see if it is interrupt)
#ifdef _MENU
 #ifdef _KEY
	key_loop();
 #endif // _KEY
#endif // _MENU

#if defined(_MMC) && defined(_LOG)
	// record_environment
	record_log();
#endif // defined(_MMC) && defined(_LOG)
	
#ifdef _RTC
	if (millis_sch + millis_interv_sch < millis()) {
		millis_sch = millis();
 #ifdef SERIAL_OUT
		Serial.println(F("Checking schedule"));
 #endif // SERIAL_OUT
		for (int zone = 0; zone < ZONES; ++zone) {
 #if defined(_NUT) || defined(_FLO)
			// check for and execute appropriate schedule
			int week = check_schedule(zone);	// Return -1 for not scheduled or error
			if (week != -1) execute_schedule(zone, week);
 #endif // defined(_NUT) || defined(_FLO)
		}
	}
#endif // _RTC
} // void loop()
