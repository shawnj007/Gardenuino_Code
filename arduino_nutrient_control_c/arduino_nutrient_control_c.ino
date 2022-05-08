#include "arduino_nutrient_control_c.h"
#include "arduino_nutrient_control_globals.h"

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
	 nc	11	D11	PWM
	 nc	12	D12	PWM 
	 	13	D13	PWM	-> D47
	 nc	14	D14
	 nc	15	D15
	 nc	16	D16
	 nc	17	D17
INT2 nc	19	D19				(KEY interrupt)
INT1	20	D20		SDA		I2C Data
INT0	21	D21		SCL		I2C Clock
	 nc	22	D22		
		23	D23		SS2		SPI MMC chip select / SPI Ram chip select
	 nc	24	D24
	 nc	25	D25
	 nc	26	D26
	 nc	27	D27
	 nc	28	D28		
	 nc	29	D29		
	 	30	D30		J1-3	Motor 0 -3
		31	D31		J1-4	Motor 0 -4
		32	D32		J5-1	Motor 4 -1
		33	D33		J5-2	Motor 4 -2
		34	D34		J5-3	Motor 4 -3
		35	D35		J5-4	Motor 4 -4
		36	D36		J4-1	Motor 3 -1
		37	D37		J4-2	Motor 3 -2
		38	D38		J4-3	Motor 3 -3
		39	D39		J4-4	Motor 3 -4
		40	D40		J3-1	Motor 2 -1
		41	D41		J3-2	Motor 2 -2
		42	D42		J3-3	Motor 2 -3
		43	D43		J3-4	Motor 2 -4
		44	D44	PWM	J2-1	Motor 1 -1
		45	D45	PWM	J2-2	Motor 1 -2
		46	D46	PWM	J2-3	Motor 1 -3
		47	D47	D13	J2-4	Motor 1 -4
		48	D48		J1-2	Motor 0 -2
		49	D49		J1-1	Motor 0 -1
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
		66	A12		J6-1	Motor 5 -1
		67	A13		J6-2	Motor 5 -2
		68	A14		J6-3	Motor 5 -3
		69	A15		J6-4	Motor 5 -4
 */

 /* Screen layout
  *     012345678901234567890
  *    +---------------------+
  * 0  |water CH:CM:CS  HH:MM|
  * 1  |                Mo/Da|
  * 2  |     temp wet1 soil1 |
  * 3  |    humid    2     2 |
  * 4  | pressure    3     3 |
  * 5  |     flow    4     4 |
  * 6  | activity    5     5 |
  * 7  |    ALARM    6     6 |
  *    +---------------------+
  */

// TODO System leak detection
// TODO Low flow detection

#ifdef _DIS
void setup_display() {
	if(display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
		display.display();
 #ifdef SERIAL_OUT
		Serial.println(F("SSD1306 allocation success"));
 #endif
	} else {
 #ifdef _DO_HW_CHECK
		failed_dis = true;
 #endif
 #ifdef SERIAL_OUT
		Serial.println(F("FAILED: SSD1306 allocation failed!"));
 #endif
	}
}
#endif // _DIS

#ifdef _RTC
time_t getDateTime(const char *str) {
	int Year, Month, Day, Hour, Min, Sec;
	tmElements_t dttm;
	if (sscanf(str, "%d/%d/%d %d:%d:%d", &Year, &Month, &Day, &Hour, &Min, &Sec) != 6) {
 #ifdef SERIAL_OUT
		sprintf(ser_output, "Failed to parse: %s\n", str);
		Serial.print(ser_output);
 #endif
		return false;
	}
	dttm.Year = CalendarYrToTm(Year);
	dttm.Month = Month;
	dttm.Day = Day;
	dttm.Hour = Hour;
	dttm.Minute = Min;
	dttm.Second = Sec;
	time_t time = makeTime(dttm);
 #ifdef SERIAL_OUT
	sprintf(ser_output, "%s = %ld\n", str, (unsigned long) time);
	Serial.print(ser_output);
 #endif
	return time;
}

// call back for file timestamps
 #ifdef _MMC
void dateTime(uint16_t* date, uint16_t* time) {
	read_rtc();
	
	// return date using FAT_DATE macro to format fields
	*date = FAT_DATE(tm.Year - 78, tm.Month, tm.Day);

	// return time using FAT_TIME macro to format fields
	*time = FAT_TIME(tm.Hour, tm.Minute, tm.Second);
}
 #endif

void loop_rtc() {
	if (millis_rtc + millis_interv_rtc < millis()) {
		millis_rtc = millis();
		read_rtc();
		print_rtc();
	}
}

bool getTime(const char *str) {
	int Hour, Min, Sec;
	if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
	tm.Hour = Hour;
	tm.Minute = Min;
	tm.Second = Sec;
	return true;
}

const char *monthName[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

bool getDate(const char *str) {
	char Month[12];
	int Day, Year;
	uint8_t monthIndex;
	if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
	for (monthIndex = 0; monthIndex < 12; monthIndex++) {
		if (strcmp(Month, monthName[monthIndex]) == 0) break;
	}
	if (monthIndex >= 12) return false;
	tm.Day = Day;
	tm.Month = monthIndex + 1;
	tm.Year = CalendarYrToTm(Year);
	return true;
}

void setup_rtc() {
	//bool parse=false;
	//bool config=false;

	if (RTC.chipPresent() && RTC.isRunning() && SET_RTC == true) {
		// get the date and time the compiler was run
		if (getDate(__DATE__) && getTime(__TIME__)) {
			//parse = true;
			// and configure the RTC with this info
			if (RTC.write(tm)) {
				//config = true;
			}
		}
 #ifdef SERIAL_OUT
		Serial.println(F("Set RTC."));
 #endif
	} else {
		tmElements_t tm_rtc;
		if (getDate(__DATE__) && getTime(__TIME__)) {
			RTC.read(tm_rtc);
			if ((tm_rtc.Year >= tm.Year) && (tm_rtc.Month >= tm.Month) && (tm_rtc.Day >= tm.Day) && (tm_rtc.Hour >= tm.Hour) && (tm_rtc.Minute >= tm.Minute)) {
				char rtc_time_out[128];
				RTC.read(tm);
				sprintf(rtc_time_out, "%02d/%02d/%02d %2d:%02d:%02d", tmYearToCalendar(tm.Year)%100, tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second);
 #ifdef SERIAL_OUT
				Serial.print(F("RTC time is: "));
				Serial.println(rtc_time_out);
 #endif
			}
		}
 #ifdef SERIAL_OUT
		Serial.println(F("FAILED: Could not set RTC!"));
 #endif
 #ifdef _DO_HW_CHECK
		failed_rtc = true;
 #endif
	}
}

 #if defined(SERIAL_OUT) || defined (SERIAL_OUT_VERBOSE)
void print2digits(int number) {
	if (number >= 0 && number < 10) {
		Serial.write("0");
	}
	Serial.print(number);
}
 #endif

void read_rtc() {
	RTC.read(tm);
 #ifdef SERIAL_OUT_VERBOSE
	Serial.print(F("Ok, Time = "));
	print2digits(tm.Hour);
	Serial.write(':');
	print2digits(tm.Minute);
	Serial.write(':');
	print2digits(tm.Second);
	Serial.print(F(", Date (D/M/Y) = "));
	Serial.print(tm.Day);
	Serial.write('/');
	Serial.print(tm.Month);
	Serial.write('/');
	Serial.print(tmYearToCalendar(tm.Year));
	Serial.println();
 #endif
}
#endif // _RTC

#ifdef _BME
void setup_thp() {
	if (bme.begin(0x76)) {
 #ifdef SERIAL_OUT
		Serial.print(F("Temperature = "));
		Serial.print(bme.readTemperature());
		Serial.println(F(" °C"));
		
		Serial.print(F("Pressure = "));
		Serial.print(bme.readPressure() / 100.0F);
		Serial.println(F(" hP"));
		
		Serial.print(F("Approx. Altitude = "));
		Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
		Serial.println(F(" m"));
		
		Serial.print(F("Humidity = "));
		Serial.print(bme.readHumidity());
		Serial.println(F(" %"));
 #endif
	} else {
 #ifdef SERIAL_OUT
		Serial.println(F("FAILED: Could not find a valid BME280 sensor, check wiring!"));
 #endif
 #ifdef _DO_HW_CHECK
		failed_bme = true;
 #endif
	}
}
#endif // _BME

#ifdef _KEY
void setup_key() {
	for (int i = 0; i < 4; ++i) {
		keys.pinMode(ROW[i], OUTPUT);
		keys.pinMode(COL[i], INPUT);
	}
	
	if (keys.begin()){
 #ifdef SERIAL_OUT
		Serial.println(F("Keys passed!"));
 #endif
	} else {
 #ifdef SERIAL_OUT
		Serial.println(F("FAILED: Keys failed!"));
 #endif
 #ifdef _DO_HW_CHECK
		failed_key = true;
 #endif
	}
}
#endif // _KEY

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

#ifdef _MMC
void setup_mmc() {
 #ifdef SERIAL_OUT
	Serial.println(F("Setting up MMC"));
 #endif
	if (card.init(SPI_HALF_SPEED, SS_MMC)) {
		// print the type of card
 #ifdef SERIAL_OUT_VERBOSE
		Serial.println();
		Serial.print(F("Card type:         "));
		switch (card.type()) {
			case SD_CARD_TYPE_SD1:
				Serial.println(F("SD1"));
			break;
			case SD_CARD_TYPE_SD2:
				Serial.println(F("SD2"));
			break;
			case SD_CARD_TYPE_SDHC:
				Serial.println(F("SDHC"));
			break;
			default:
				Serial.println(F("Unknown"));
		}
 #endif
	} else {
 #ifdef SERIAL_OUT
		Serial.println(F("FAILED: SD card initialization failed."));
 #endif
 #ifdef SERIAL_OUT_VERBOSE
		Serial.println(F("Things to check:"));
		Serial.println(F("* is a card inserted?"));
		Serial.println(F("* is your wiring correct?"));
		Serial.println(F("* did you change the chipSelect pin to match your shield or module?"));
 #endif
 #ifdef _DO_HW_CHECK
		failed_mmc = true;
 #endif
	}
 #ifdef _DO_HW_CHECK
	if (!failed_mmc && volume.init(card)) {
 #ifdef SERIAL_OUT_VERBOSE
		Serial.print(F("Clusters:          "));
		Serial.println(volume.clusterCount());
		Serial.print(F("Blocks x Cluster:  "));
		Serial.println(volume.blocksPerCluster());
		
		Serial.print(F("Total Blocks:      "));
		Serial.println(volume.blocksPerCluster() * volume.clusterCount());
		Serial.println();
 #endif
		// print the type and size of the first FAT-type volume
		uint32_t volumesize;
 #ifdef SERIAL_OUT_VERBOSE
		Serial.print(F("Volume type is:    FAT"));
		Serial.println(volume.fatType(), DEC);
 #endif
		volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
		volumesize *= volume.clusterCount();       // we"ll have a lot of clusters
		volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
 #ifdef SERIAL_OUT_VERBOSE
		Serial.print(F("Volume size (Kb):  "));
		Serial.println(volumesize);
		Serial.print(F("Volume size (Mb):  "));
 #endif
		volumesize /= 1024;
 #ifdef SERIAL_OUT_VERBOSE
		Serial.println(volumesize);
		Serial.print(F("Volume size (Gb):  "));
		Serial.println((float)volumesize / 1024.0);
		Serial.println(F("\nFiles found on the card (name, date and size in bytes): "));
 #endif
		root.openRoot(volume);
		
		// list all files in the card with date and size
		root.ls(LS_R | LS_DATE | LS_SIZE);
		if (!SD.begin(SS_MMC)) {
 #ifdef SERIAL_OUT
			Serial.println(F("FAILED: MMC initialization failed!"));
 #endif
 #ifdef _DO_HW_CHECK
			failed_mmc = true;
 #endif // _DO_HW_CHECK
		}
	} else 
 #endif // _DO_HW_CHECK
	{
 #ifdef SERIAL_OUT_VERBOSE
		Serial.println(F("FAILED: Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card"));
 #endif // SERIAL_OUT_VERBOSE
 #ifdef _DO_HW_CHECK
		failed_mmc = true;
 #endif // _DO_HW_CHECK
	}
 #ifdef _RTC
	SdFile::dateTimeCallback(dateTime);
 #endif
 #ifdef SERIAL_OUT
	Serial.println(F("MMC done"));
 #endif // SERIAL_OUT
}
#endif // _MMC

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

#ifdef _NUT
void setup_steppers() {
 #ifdef SERIAL_OUT_VERBOSE
		Serial.println(F("Setting up steppers"));
 #endif // SERIAL_OUT_VERBOSE
	// 6 steppers, 4 wires each
	uint8_t s;
	for (s = 0; s < COUNT_STEPPERS - NUTRIENTS; ++s) {
		stepper[s] = PumpStepper(PumpStepper::HALF4WIRE, STEPPER_WIRES[s][0], STEPPER_WIRES[s][1], STEPPER_WIRES[s][2], STEPPER_WIRES[s][3]);
		stepper[s].setMaxSpeed(MAXSPEED);
		steppers.addStepper(stepper[s]);
 #ifdef SERIAL_OUT
		Serial.print(s);
		Serial.println(F(" Stepper CONTINUOUS"));
 #endif // SERIAL_OUT_VERBOSE
	}
	
	for ( ; s < COUNT_STEPPERS - (NUTRIENTS - 3); ++s) {
		stepper[s] = PumpStepper(PumpStepper::SYRINGE, 69.0, 20.0, PumpStepper::HALF4WIRE, STEPPER_WIRES[s][0], STEPPER_WIRES[s][1], STEPPER_WIRES[s][2], STEPPER_WIRES[s][3], 0, 0, true);
		stepper[s].setMaxSpeed(MAXSPEED);
		steppers.addStepper(stepper[s]);
 #ifdef SERIAL_OUT
		Serial.print(s);
		Serial.println(F(" Stepper SYRINGE H = 69.00 V = 20.0"));
 #endif // SERIAL_OUT_VERBOSE
	}
	
	for ( ; s < COUNT_STEPPERS; ++s) {
		stepper[s] = PumpStepper(PumpStepper::SYRINGE, 47.75, 3.0, PumpStepper::HALF4WIRE, STEPPER_WIRES[s][0], STEPPER_WIRES[s][1], STEPPER_WIRES[s][2], STEPPER_WIRES[s][3], 0, 0, true);
		stepper[s].setMaxSpeed(MAXSPEED);
		steppers.addStepper(stepper[s]);
 #ifdef SERIAL_OUT
		Serial.print(s);
		Serial.println(F(" Stepper SYRINGE H = 47.75 V =  3.0"));
 #endif // SERIAL_OUT_VERBOSE
	}
	
	for (uint8_t n = 0; n < NUTRIENTS; ++n) {
		nut[n] = stepper[COUNT_STEPPERS - NUTRIENTS + n];
		nuts.addStepper(nut[n]);
	}
	
 #ifdef SERIAL_OUT
	Serial.println(F("Steppers done"));
 #endif // SERIAL_OUT
}
#endif // _NUT

#ifdef _SEN
void setup_sensors() {
 #ifdef SERIAL_OUT_VERBOSE
	Serial.println(F("Setting up sensors"));
 #endif // SERIAL_OUT_VERBOSE
	// 12 analog sensors
	for (uint8_t s = 0; s < COUNT_SENSORS; ++s) {
		pinMode(SENSORS[s], INPUT);
		analogWrite(SENSORS[s], LOW);
 #ifdef SERIAL_OUT_VERBOSE
		Serial.print(F("Setup Sensor "));
		Serial.println(s);
 #endif // SERIAL_OUT_VERBOSE
	}
 #ifdef SERIAL_OUT
	Serial.println(F("Sensors done"));
 #endif // SERIAL_OUT
}
#endif // _SEN

#ifdef _PWM
void setup_pwm_power() {
 #ifdef SERIAL_OUT_VERBOSE
	Serial.println(F("Setting up PWM"));
 #endif // SERIAL_OUT_VERBOSE
	// 7 outputs
	for (uint8_t s = 0; s < COUNT_PWM; ++s) {
		pinMode(PWM[s], OUTPUT);
		analogWrite(PWM[s], 0);
 #ifdef SERIAL_OUT_VERBOSE
		Serial.print(F("Setup PWM "));
		Serial.println(s);
 #endif // SERIAL_OUT_VERBOSE
	}
 #ifdef SERIAL_OUT
	Serial.println(F("PWM done"));
 #endif // SERIAL_OUT
}
#endif // _PWM

#ifdef _VAL
void setup_valves() {
 #ifdef SERIAL_OUT_VERBOSE
	Serial.println(F("Setting up valves"));
 #endif // SERIAL_OUT_VERBOSE
	// 7 outputs
	for (uint8_t s = 0; s < COUNT_VALVES; ++s) {
		pinMode(VALVES[s], OUTPUT);
		digitalWrite(VALVES[s], LOW);
 #ifdef SERIAL_OUT_VERBOSE
		Serial.print(F("Setup VALVE "));
		Serial.println(s);
 #endif // SERIAL_OUT_VERBOSE
	}
 #ifdef SERIAL_OUT
	Serial.println(F("Valves done"));
 #endif // SERIAL_OUT
}
#endif // _VAL

#ifdef _PWM
void disable_mixers() {
 #ifdef SERIAL_OUT
	Serial.println(F("Stopping mixers"));
 #endif // SERIAL_OUT
	analogWrite(PWM[0], (uint8_t) (0));
}

void enable_mixers() {
	disable_mixers();
 #ifdef SERIAL_OUT
	Serial.println(F("Starting mixers"));
 #endif // SERIAL_OUT
	delay(1000);
	
	float speed_x = 0.85;
	
	analogWrite(PWM[0], (uint8_t) (speed_x * 0.9 * 255));
	delay(200);
	
	analogWrite(PWM[0], (uint8_t) (speed_x * 0.6 * 255));
	delay(400);
	
	for (float r = .3; r <= .5 ; r += .01) {
		analogWrite(PWM[0], speed_x * r * 255);
		delay(100);
	}
}
#endif // _PWM

#ifdef _FLO
void setup_flow_rate() {
 #ifdef SERIAL_OUT
	Serial.println(F("Setup flow rate sensor"));
 #endif // SERIAL_OUT
	pinMode(FLOW_RATE[0], INPUT);
	digitalWrite(FLOW_RATE[0], LOW);
}

void h2o_tick_in () {
	++h2o_ticks[0];
}

void h2o_tick_out () {
	++h2o_ticks[1];
}

void enable_flow_rate(bool enable) {
 #ifdef SERIAL_OUT
	Serial.println(F("Configuring flow rate sensor"));
 #endif // SERIAL_OUT
	if (enable) {
		attachInterrupt(digitalPinToInterrupt(FLOW_RATE[0]), h2o_tick_in, CHANGE);
 #ifdef SERIAL_OUT
		Serial.println(F("Enabled flow rate pin interrupt"));
 #endif // SERIAL_OUT
	} else {
		detachInterrupt(digitalPinToInterrupt(FLOW_RATE[0]));
 #ifdef SERIAL_OUT
		Serial.println(F("Disabled flow rate pin interrupt"));
 #endif // SERIAL_OUT
	}
}
#endif // _FLO

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

void load_options() {
	// check if SD card is valid and working
		// check if options already exist on SD card
			// check if options are valid
				// load options
	// .. any fails, load fallback options
}

void save_options() {
	// check if SD card is valid and working
		// check if options already exist on SD card
			// check if options are valid
				// save options
}


#ifdef _KEY
uint8_t check_key() {
	// get key from I2C key library
	// check for key change
	// return key value (if changed)
	// else return 0 = no change, -1 = no key pressed
 #ifdef SERIAL_OUT_VERBOSE
	Serial.println(F("checking key"));
 #endif // SERIAL_OUT_VERBOSE
	keys.readBuffer();
	int row, col;
	for (row = 0; row < 4; ++row) {
		keys.digitalWrite(ROW[row], HIGH);
		for (col = 0; col < 4; ++col) {
			if (keys.digitalRead(COL[col]) == HIGH) {
				uint8_t key_out = key_[row][col];
				keys.digitalWrite(ROW[row], LOW);
				if (key_out != last_key) {
 #ifdef SERIAL_OUT
					Serial.print(F("Key pressed:"));
					Serial.println(key_out);
 #endif // SERIAL_OUT
					last_key = key_out;
					return key_out;
				} else {
					return KEY_NO_CHANGE;
				}
			}
		}
		keys.digitalWrite(ROW[row], LOW);
	}
	last_key = KEY_NO_PRESS;
	return KEY_NO_PRESS;
}

int select_menu(int menu, int menu_pos) {
	// Select the right menu...

	switch (menu) {
		case MENU_MAIN:
			switch (menu_pos) {
				case 0:
					return MENU_MANUAL;
			}
	}
	
	return 0;
}

 #ifdef _MENU
int display_menu(int menu, float menu_pos) {
	// Display the menu...

	switch (menu) {
		case MENU_MAIN:
			
		
			break;
	}
	
	return 0;
}

bool menu_loop(int menu) {

	int menu_pos = 0;
  #ifdef _KEY
	//check key (and respond in context)
	int key = check_key();

	switch (key) {
		// Enter menu
		case KEY_START:
			// Change to appropriate child-menu
			int menu = select_menu(menu, menu_pos);
			while (menu_loop(menu));
			break;
		case KEY_STOP:
			// Change to appropriate parent-menu
			return false;
			break;
		case KEY_UP:
			menu_pos -= 1;
			break;
		case KEY_LT:
			break;
		case KEY_DN:
			menu_pos += 1;
			break;
		case KEY_RT:
			//menu_pos += 0.01;
			break;
	}
	
	display_menu(menu, menu_pos);

	return true;
  #endif // _KEY
}

bool start_loop() {
	// Start watering, executing a "pre-defined" program

	// Get RTC current time

	// For each sink (humidifier/plant)
	
		// Calculate stage
	
		// Execute stage
	
	return false;
}

bool stop_loop() {
	// Stop all activity, close all valves, verify no flow
	return false;
}

bool f1_loop() {
	return menu_loop(MENU_MAIN);
}

bool f2_loop() {
	return false;
}

bool f3_loop() {
	return false;
}

bool f4_loop() {
	return false;
}
 #endif // _MENU
#endif // _KEY

#ifdef _SEN
bool check_alarm_flood() {
	// TODO later design/build sensor
 #ifdef SERIAL_OUT
	if (alarm_flood) Serial.println(F("ALARM\nALARM: flood\nALARM"));
 #endif // SERIAL_OUT
	return alarm_flood;
}
#endif // _SEN

float h2o_rate = 0;
unsigned long h2o_millis = 0;

#ifdef _FLO
bool check_alarm_flow() {
	if ((h2o_rate < FLOW_RATE_MIN) || (h2o_rate > FLOW_RATE_MAX)) alarm_flow = true;
 #ifdef SERIAL_OUT
	if (alarm_flow) Serial.println(F("ALARM\nALARM: flow\nALARM"));
 #endif // SERIAL_OUT
	return alarm_flow;
}
#endif // _FLO

#ifdef _NUT
bool check_alarm_nut() {
	// TODO later design/build sensor
	// Check pH value
	// Check Electrical Conductivity
	// Check Dissolved Oxygen
	// Check Total Dissolved Solids
	
	// Check if enough nutrients remain
	for (int s = 0; s < NUTRIENTS; ++s) {
		if (opt_nut_rem[s] <= 0) alarm_nut = true;
	}
 #ifdef SERIAL_OUT
	if (alarm_nut) Serial.println(F("ALARM\nALARM: nutrient\nALARM"));
 #endif // SERIAL_OUT
	return alarm_nut;
}
#endif // _NUT

#ifdef _SEN
bool check_alarm_soil(int zone) {
	int value = PLANTS_SENSE[zone];
	if (value < SOIL_MOIST_MIN) alarm_soil = -1;
	if (value > SOIL_MOIST_MAX) alarm_soil =  1;
	if (alarm_soil) Serial.println(F("ALARM\nALARM: soil\nALARM"));
	return (alarm_soil != 0);
}

bool check_alarm_env() {
	// TODO later
	// Check environmental limits
	// Check temperature limits
	// Check humidity limits
 #ifdef SERIAL_OUT
	if (alarm_env) Serial.println(F("ALARM\nALARM: environment\nALARM"));
 #endif // SERIAL_OUT
	return alarm_env;
}

bool check_alarm_int() {
	// TODO later design/build interrupt
	// Check for interrupt
 #ifdef SERIAL_OUT
	if (alarm_int) Serial.println(F("ALARM\nALARM: interrupt\nALARM"));
 #endif // SERIAL_OUT
	return alarm_int;
}
#endif // _SEN

bool check_alarms() {
	return false
#ifdef _SEN
		|| check_alarm_flood()
 #ifdef _NUT
		//|| check_alarm_nut() 				// FIXME
 #endif // _NUT
		//|| check_alarm_soil(0) 			// FIXME
		|| check_alarm_env()
		|| check_alarm_int()
#endif // _SEN
#ifdef _FLO
		//|| check_alarm_flow() 			// FIXME
#endif // _FLO
		;
}

#ifdef _SEN
bool print_alarms() {
	sprintf(alarm_state, "%5s%4s%3s%4s%3s", 
		alarm_flood ? "FLOOD" : "",
		
 #ifdef _FLO
		alarm_flow  ? "FLOW"  : "",
 #else
		"noFL",
 #endif

 #ifdef _NUT
		alarm_nut   ? "NUT"   : "",
 #else
		"noN",
 #endif
		alarm_soil  ? "SOIL"  : "",
		alarm_env   ? "ENV"   : "");
	return false;
}
/*
bool check_sensor(int pin, int threshold) {
	int sum = 0;
	for(int i = 0; i < SAMPLE_COUNT; ++i) {
		sum += analogRead(pin);
	}	
}
*/
bool check_inputs() {
	// TODO later
	// Check soil moisture sensors
	// Check humidifier tank sensors
	return false;
}

bool print_inputs() {
	// TODO later 
	// prepare humidifier sensors
	// prepare soil sensors
	return false;
}
#endif // _SEN

#ifdef _RTC
void print_rtc() {
	sprintf(dt_str, "%02d/%02d/%02d", tmYearToCalendar(tm.Year)%100, tm.Month, tm.Day);
	sprintf(tm_str, "%2d:%02d:%02d", tm.Hour, tm.Minute, tm.Second);
	sprintf(dttm_str, "%04d/%02d/%02d %2d:%02d:%02d", tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second);
}
#endif // _RTC

char f_str[12] = { };

char *print_float(float f) {
	sprintf(f_str, "%5ld.%03ld",  (long)f, (long)(f*1000)%1000);
 #ifdef SERIAL_OUT_VERBOSE
	Serial.print(F("Converting "));
	Serial.print(f);
	Serial.print(F(" to "));
	Serial.println(f_str);
 #endif // SERIAL_OUT_VERBOSE
	return f_str;
}

#ifdef _BME
void senssor_loop() {
	t = bme.readTemperature();
	p = bme.readPressure() / 100.0F;
	h = bme.readHumidity();
}

void print_sensors() {

 #ifdef SERIAL_OUT_VERBOSE
	Serial.print(F("Temperature = "));
	Serial.print(t);
	Serial.println(F(" °C"));
	
	Serial.print(F("Pressure = "));
	Serial.print(p);
	Serial.println(F(" hP"));
	
	Serial.print(F("Humidity = "));
	Serial.print(h);
	Serial.println(F(" %"));
	
	Serial.println();
 #endif // SERIAL_OUT_VERBOSE
	//sprintf(tph_str, "%6.2f*C %6.2hP %6.2%%\n", t, p, h);
	sprintf(t_str, "%4d.%01doC",  (int)t, (int)(t*10)%10);
	sprintf(p_str, "%4d.%01dhP",  (int)p, (int)(p*10)%10);
	sprintf(h_str, "%4d.%01d%%",  (int)h, (int)(h*10)%10);
	
	sprintf(tph_str, "%s %s %s", t_str, p_str, h_str);
}
#endif // _BME

#if defined(_DIS) && ( defined(_SEN) || defined(_DIS) || defined(_BME) )
void display_loop() {
	// TODO after others
	
	if (millis_display_loop + interv_display_loop >= millis()) return;
	millis_display_loop = millis();
	// display time
	
	// format all sensor readouts to strings
 #ifdef _DIS
  #ifdef _SEN
	print_alarms();
	print_inputs();
  #endif
  #ifdef _BME
	print_sensors();
  #endif
	// display sensor readouts to screen
	display.clearDisplay();
	display.setTextSize(1);             // Normal 1:1 pixel scale
	display.setTextColor(SSD1306_WHITE);        // Draw white text
  #ifdef _RTC
	display.setCursor(0,0);             // Start at top-left corner
	display.print(dt_str);
	display.setCursor(0,8);             // Start at top-left corner
	display.print(tm_str);
  #endif
  #ifdef _BME
	display.setCursor(0,16);             // Start at top-left corner
	display.print(t_str);
	display.setCursor(0,24);             // Start at top-left corner
	display.print(p_str);
	display.setCursor(0,32);             // Start at top-left corner
	display.print(h_str);
	display.setCursor(0,56);             // Start at top-left corner
	//display.print(sys_status);
  #endif
	display.display();
 #endif // _DIS

	// log sensor readouts to MMC
}
#endif // defined(_DIS) && ( defined(_SEN) || defined(_DIS) || defined(_BME) )

#ifdef _MMC
void backup_file(char* filename) {
	if (SD.exists(filename)) {
 #ifdef SERIAL_OUT
		Serial.print(F("Backing up "));
		Serial.println(filename);
 #endif // SERIAL_OUT
		File file_old = SD.open(filename, FILE_READ);
		char file_new_name[13] = { };
		sprintf(file_new_name, "old_%s", filename);
 #ifdef _RTC
		char data_header[20] = { };
		sprintf(data_header, "\nd%04d%02d%02dt%02d%02d%02d\n", tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second);
 #endif // _RTC
		File file_new = SD.open(file_new_name, FILE_WRITE);
		size_t n;
		uint8_t buf[64];
 #ifdef _RTC
		file_new.write(data_header);
 #endif // _RTC
		while ((n = file_old.read(buf, sizeof(buf))) > 0) file_new.write(buf, n);
		file_old.flush();
		file_old.close();
		file_new.flush();
		file_new.close();
 #ifdef SERIAL_OUT
		Serial.println(F("Backup done"));
 #endif // SERIAL_OUT
	}
}

 #ifdef _NUT
bool write_nut_file() {
  #ifdef SERIAL_OUT
	Serial.println(F("Writing nutrient file."));
  #endif // SERIAL_OUT
	char filename[] = "nut.cfg";
	//backup_file(filename);
	File file;
	
	if (SD.exists(filename)) {
		file = SD.open(filename, O_WRITE);
		file.seek(0);
	} else {
		file = SD.open(filename, FILE_WRITE);
	}
	
	if (!file) {
  #ifdef SERIAL_OUT
		Serial.println(F("FAILED to open nut.cfg"));
  #endif // SERIAL_OUT
		return false;
	}

	// write rows (nutrient count)
	sprintf(fil_output, "%2d\n", NUTRIENTS);
	file.write(fil_output);
	
	// write cols (weeks count)
	sprintf(fil_output, "%2d\n", WEEKS);
	file.write(fil_output);
  #if defined(_FLO) || defined(_H2O)
	// write array[cols] into h2o array
	for (int j = 0; j < WEEKS; ++j) {
		sprintf(fil_output, "%6d", opt_h2o_amt[j]);
		file.write(fil_output);
	}
	file.write("\n");
  #endif // defined(_FLO) || defined(_H2O)
	// write array[rows,cols] into nut amounts
	for (int i = 0; i < NUTRIENTS; ++i) {
		for (int j = 0; j < WEEKS; ++j) {
			sprintf(fil_output, "%4d.%01d", (int) opt_nut_ratio[i][j], (int) (opt_nut_ratio[i][j] * 10.0) % 10);
			file.write(fil_output);
		}
		file.write("\n");
	}
	file.write("\n");
	file.close();
  #ifdef SERIAL_OUT
	Serial.println(F("Done"));
  #endif //SERIAL_OUT
	return true;
}

bool read_nut_file() {
  #ifdef SERIAL_OUT
	Serial.println(F("Reading nutrient files"));
  #endif
	char filename[] = "nut.cfg";
	if (!SD.exists(filename)) write_nut_file();

	File file = SD.open(filename, FILE_READ);
	if (!file) return false;
	
	char val[7] = { };
	
	// read rows (nutrient count)
	file.read(val, 3);
	int nutrients = atoi(val);
	
	// read cols (weeks count)
	file.read(val, 3);
	int weeks = atoi(val);

	// verify that file matches compiled code parameters
	if (weeks != WEEKS) {
  #ifdef SERIAL_OUT
		Serial.println(F("Week count does not match."));
  #endif
		while (true);
	}
	if (nutrients != NUTRIENTS) {
  #ifdef SERIAL_OUT
		Serial.println(F("Nutrient count does not match."));
  #endif
		while (true);
	}


  #if defined(_FLO) || defined(_H2O)
	// read array[cols] into h2o array
	for (int j = 0; j < WEEKS; ++j) {
		file.read(val, 6);
		opt_h2o_amt[j] = atoi(val);
	}
	file.read(val, 1);
  #endif // defined(_FLO) || defined(_H2O)
	// read array[rows,cols] into nut amounts
	for (int i = 0; i < NUTRIENTS; ++i) {
		for (int j = 0; j < WEEKS; ++j) {
			file.read(val, 6);
			opt_nut_ratio[i][j] = atof(val);
		}
		file.read(val, 1);
	}
	file.read(val, 1);
	file.close();
  #ifdef SERIAL_OUT
	Serial.println(F("Done with nutrient files"));
  #endif // SERIAL_OUT
	return true;
}

void write_nut_rem_file(int s) {
	const char *type = "nut";
	char filename[13];
	sprintf(filename, "%s_%01d.rem", type, s);
	File file;
	if (SD.exists(filename)) {
		file = SD.open(filename, O_WRITE);
		file.seek(0);
	} else {
		file = SD.open(filename, FILE_WRITE);
	}
	sprintf(fil_output, "%10s", print_float(opt_nut_rem[s]));
  #ifdef SERIAL_OUT_VERBOSE
	Serial.print(F("Writing nutrient "));
	Serial.print(s);
	Serial.print(F(" remaining"));
	Serial.println(fil_output);
  #endif // SERIAL_OUT_VERBOSE
	file.write(fil_output);
	file.flush();
	file.close();
	return;
}

float read_nut_rem_file(int s) {
	const char *type = "nut";
	char filename[13];
	sprintf(filename, "%s_%01d.rem", type, s);
	if (!SD.exists(filename)) write_nut_rem_file(s);
	File file = SD.open(filename, FILE_READ);
	char val[11] = { };
	file.read(val, 10);
  #ifdef SERIAL_OUT_VERBOSE
	Serial.print(F("Reading nutrient "));
	Serial.print(s);
	Serial.print(F(" remaining"));
	Serial.println(val);
  #endif // SERIAL_OUT_VERBOSE
	file.close();
	return atof(val);
}
 #endif // _NUT

 #ifdef _RTC
bool write_zone_file(const char *type, int zone, char times[3][20]) {
	char filename[13];
	sprintf(filename, "%s_%01d.cfg", type, zone);
	//backup_file(filename);
	File file;
	if (SD.exists(filename)) {
		file = SD.open(filename, O_WRITE);
		file.seek(0);
	} else {
		file = SD.open(filename, FILE_WRITE);
	}
	if (!file) {
  #ifdef SERIAL_OUT
		Serial.print(F("Writing "));
		sprintf(ser_output, "%s", type);
		Serial.print(ser_output);
		Serial.print(F(" "));
		sprintf(ser_output, "%d", zone);
		Serial.print(ser_output);
		Serial.println(F(" failed"));
  #endif // SERIAL_OUT
		return false;
	}
  #ifdef SERIAL_OUT
	Serial.println();
  #endif // SERIAL_OUT
	// Write zone start date		YYYY/MM/DD HH:MM:SS AP
	if (times[0][0] == (char) 0) {
		// Write zone start date
		file.write(dttm_str);
		file.write("\n");
		// Write zone last h2o date
		file.write("1970/01/01 00:00:00\n");
		// Write zone last nut date
		file.write("1970/01/01 00:00:00\n");
	} else {
		for (int i = 0; i < 3; ++i) {
			file.write(times[i], 19);
			file.write('\n');
  #ifdef SERIAL_OUT_VERBOSE
			Serial.print(F("Wrote "));
			Serial.print(times[i]);
			Serial.print(F(" to file.\n"));
  #endif // SERIAL_OUT_VERBOSE
		}
	}
	file.flush();
	file.close();
  #ifdef SERIAL_OUT
	Serial.println(F("Done"));
  #endif // SERIAL_OUT
	return true;
}

bool read_zone_file(const char *type, int zone, char times[3][20]) {
  #ifdef SERIAL_OUT
	Serial.print(F("Reading "));
	Serial.print(type);
	Serial.print(F(" "));
	Serial.println(zone);
  #endif // SERIAL_OUT
	
	char filename[13];
	sprintf(filename, "%s_%01d.cfg", type, zone);
	
	if (!SD.exists(filename)) {
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 20; ++j)
				times[i][j] = (char) 0;
		write_zone_file(type, zone, times);
	}
	
	File file = SD.open(filename, FILE_READ);
	if (!file) {
  #ifdef SERIAL_OUT
		Serial.println(F(" failed"));
  #endif // SERIAL_OUT
		return false;
	}
	
	// Read zone dates
	for (int i = 0; i < 3; ++i) {
		file.read(times[i], 20);
		times[i][19] = '\0';
	}

	// TODO verify that file matches compiled code parameters (counts)
	
	file.close();
  #ifdef SERIAL_OUT
	Serial.println(F("Done"));
  #endif // SERIAL_OUT
	return true;
}
 #endif // _RTC

bool write_zone_log_file(const char *type, int zone, char* log_entry) {
 #ifdef SERIAL_OUT_VERBOSE
	Serial.println(F("Writing zone log file."));
 #endif // SERIAL_OUT_VERBOSE
	char filename[13];
	sprintf(filename, "%s_%01d.log", type, zone);
	File file = SD.open(filename, FILE_WRITE);
	if (!file) return false;
	file.write(log_entry);
	if (log_entry[strlen(log_entry) - 1] != '\n') file.write('\n');
	file.flush();
	file.close();
	Serial.println(log_entry);
	return true;
}

/*
bool read_zone_log_file(char *type, int zone) {
	Serial.println(F("Reading zone log file."));
	char filename[13];
	sprintf(filename, "%s_%01d.log", type, zone);
	//backup_file(filename);
	File file = SD.open(filename, FILE_READ);
	
	// TODO maybe
	
	file.close();
	return true;
}
*/

bool write_limits_file() {
 #ifdef SERIAL_OUT
	Serial.println(F("Writing limits file."));
 #endif
	char filename[] = "limits.cfg";
	File file;
	if (SD.exists(filename)) {
		file = SD.open(filename, O_WRITE);
		file.seek(0);
	} else {
		file = SD.open(filename, FILE_WRITE);
	}
	
	// TODO ?
	return false;
}

bool read_limits_file() {
 #ifdef SERIAL_OUT
	Serial.println(F("Reading limits file."));
 #endif
	char filename[] = "limits.cfg";
	File file = SD.open(filename, FILE_READ);
	if (!file) return false;

	// TODO ?
	return true;
}
/*
void write_config_files() {
#ifdef SERIAL_OUT
	Serial.println(F("Writing config files."));
#endif
	write_nut_file();
	// TODO
	for (int i = 0; i < COUNT_HUMID; ++i) {
//		write_zone_file("humid", i);
	}
	
	for (int i = 0; i < COUNT_PLANTS; ++i) {
//		write_zone_file("zone", i);
	}
}
*/

void read_config_files() {
 #ifdef SERIAL_OUT
	Serial.println(F("Reading config files."));
 #endif // SERIAL_OUT
 #ifdef _NUT
	read_nut_file();
 #endif // _NUT
	// TODO
 #ifdef _SEN
	for (int i = 0; i < COUNT_HUMID; ++i) {
//		read_zone_file("humid", i);
	}
	
	for (int i = 0; i < COUNT_PLANTS; ++i) {
//		read_zone_file("zone", i);
	}
 #endif // _SEN
}

 #if defined(_MMC) && defined(_LOG)
bool write_log_file() {
	bool ret = false;
	
	File log_file = SD.open("system.log", FILE_WRITE);
	
	// if the file is available, write to it:
	if (log_file) {
		for (int i = 0; i < LOG_CACHE_SIZE; i++) {
			int n = s + i;
			if (n >= LOG_CACHE_SIZE) n -= LOG_CACHE_SIZE;
			log_file.println(log_string[n]);
  #ifdef SERIAL_OUT
			Serial.println(log_string[n]);
  #endif // SERIAL_OUT
		}
		log_file.close();
		// print to the serial port too:
		ret = true;
	}
	return ret;
}

/*
void write_output(char *output) {
	sprintf(log_string[s++], "%s", output);
	if (s == LOG_CACHE_SIZE) s = 0;
	write_log_file();
}
*/

void cache_log() {
	// prepare the log statment
	sprintf(log_string[s++], "%s %s",
  #ifdef _RTC
	dttm_str,
  #else 
	"",
  #endif // _RTC
  #ifdef _BME
	tph_str
  #else
	""
  #endif // _BME
	);
	if (s == LOG_CACHE_SIZE) s = 0;
}

void record_log() {
	if (millis_cache_log + millis_interv_cache_log < millis()) {
		millis_cache_log = millis();
		cache_log();
	}
	if (millis_log + millis_interv_log >= millis()) return;
	millis_log = millis();
	
  #ifdef _DO_HW_CHECK
	// check if MMC is valid
	if (failed_mmc) return;
  #endif // _DO_HW_CHECK

  #ifdef _DIS
	display.setCursor(0,56);             // Start at top-left corner
	display.print("WRITING");
	display.display();
  #endif // _DIS
	
	// check if log file exists
	// open/write/close file
	if (!write_log_file()) {	// if the file isn"t open, pop up an error:
  #ifdef SERIAL_OUT
		Serial.println(F("error writing to system.log"));
  #endif // SERIAL_OUT
	}
  #ifdef _DIS
	display.setCursor(40,56);             // Start at top-left corner
	display.print("..DONE.");
	display.display();
  #endif // _DIS
}
 #endif // defined(_MMC) && defined(_LOG)
#endif // _MMC

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

#if defined(_NUT) || defined(_H2O)
void disable_steppers() {
	unsigned long c_time = millis();
	while (c_time + 1000 >= millis());
	for (uint8_t s = 0; s < COUNT_STEPPERS; ++s) {
		stepper[s].stop();
		stepper[s].setCurrentPosition(0);
		//stepper[s].disableOutputs();
	}
 #ifdef _H2O
	running_h2o = false;
 #endif
 #ifdef _NUT
	running_nut = false;
 #endif
 #ifdef SERIAL_OUT
	Serial.println(F("Stopped steppers"));
 #endif
}
#endif // defined(_NUT) || defined(_H2O)

float calculate_rate(unsigned long intv_time, unsigned long intv_count, float cal_factor) {

#ifdef SERIAL_OUT_VERBOSE
	Serial.print(intv_time);
	Serial.print(F(" "));
	Serial.print(intv_count);
	Serial.print(F(" "));
	Serial.print(cal_factor);
#endif
	
	if (intv_time == 0) return 0;
	
	float r = 1000.0 / (float) intv_time;	// interval ratio
	r = (float) intv_count * r;				// intv_count per second
	r = r / cal_factor;						// mL / sec
	
#ifdef SERIAL_OUT_VERBOSE
	Serial.print(F(" "));
	Serial.println(r);
#endif
	
	return r;
}

#ifdef _FLO
void calculate_h2o_rate(uint8_t sensor) {
	// calculate and return h2o rate mL / s from sensor
	//delay(100);

	unsigned long intv_time = millis() - h2o_last_millis[sensor];
	h2o_last_millis[sensor] = millis();

	// h2o_ticks[sensor] = 3;

	h2o_ticks_total[sensor] += h2o_ticks[sensor];
	h2o_millis_total[sensor] += intv_time;

	h2o_rate = calculate_rate(intv_time, h2o_ticks[sensor], cal_factor[sensor]);
	
	//h2o_rate = 33.0; // FIXME DEBUG
	
	h2o_ticks[sensor] = 0;
	return;
}
#endif

#if defined(_NUT) || defined(_H2O)
float calculate_n_rate(float n_ratio, float h2o_rate) { //, float h2o_amt, float h2o_time) {
	// determine how much nutrient remains to be dispensed
	float h2o_lps = h2o_rate / 1000.0;
	float n_rate = h2o_lps * n_ratio;
	return n_rate; // return mL / s
}

float calculate_n_speed(float s_rate) {
	// calculate number of ticks needed, speed needed

	// FULL WIDTH PULSES
	// 20mL = 10800 pulses
	// 10mL =  5400 pulses
	//  1mL =   540 pulses
	
	// HALF WIDTH PULSES
	// 20mL = 21600 pulses
	// 10mL = 10800 pulses
	//  1mL =  1080 pulses
	
 #ifdef SERIAL_OUT_VERBOSE
	Serial.println(s_rate * PULSE_PER_ML);
 #endif
	
	return (s_rate * PULSE_PER_ML);
}
#endif // defined(_NUT) || defined(_H2O)

#ifdef _H2O
unsigned long set_h2o_rate(uint8_t s, float h2o_rate) { // rate in mL / sec
	float h2o_sps = min(1000, calculate_n_speed(h2o_rate));
 #ifdef SERIAL_OUT_VERBOSE
	Serial.print(F("set h2o sps rate: "));
	Serial.println(h2o_sps);
 #endif
	stepper[s].setSpeed(h2o_sps);
	return (unsigned long) h2o_sps;
}
#endif // _H2O

#ifdef _VAL
void open_valve(int valve) {
	if (valve == -1) {
		for (int v = 0; v < COUNT_VALVES; ++v) {
			digitalWrite(VALVES[v], LOW);
		}
 #ifdef _FLO
  #ifdef SERIAL_OUT
		Serial.println(F("Close valves"));
  #endif
		running_h2o = false;
 #endif // _FLO
	} else {
		open_valve(-1);
		digitalWrite(VALVES[valve], HIGH);
 #ifdef _FLO
  #ifdef SERIAL_OUT
		Serial.println(F("Open valve"));
  #endif
		running_h2o = true;
 #endif // _FLO
	}
}
#endif // _VAL

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
	float h2o_time = 120; //h2o.getTimeToDispense(h2o_amt);

	// Setup continuous pump dicharge parameters
	stepper[0].setCurrentPosition(0);
	stepper[0].setVolumeTime(h2o_amt, h2o_time);
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
	
	do {
		loading = false;
		for (int n = 0; n < NUTRIENTS; ++n) {
			loading = nut[n].runSpeedToPositionToStop() | loading;
		} 
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
	Serial.print("Fill took ");
	Serial.print(millis() - time_fill);
	Serial.print(" millis.. ");
	
   #ifdef _PWM
	disable_mixers();
   #endif // _PWM
	
	Serial.println("Done Loading");
	// Setup syringe dicharge parameters	
	// Calculate volumes for phase 1, 2, and 3
		// phase 1, just water, 1/5 of water volume
		// phase ., nuts water, 3/5 of water volume, all nutrients
		// phase 5, just water, 1/5 of water volume, flush lines of nutrients
	int nut_time = ceil(h2o_time * (3.0 / 5.0));
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
	Serial.println("Setup Discharge");
	
  #ifdef _SEN
	//bool alarm = false;
	//int alarm_code = 0;
  #endif // _SEN

	bool running_h2o;
	float h2o_amt_dis = 0;

	// open valve
	open_valve(zone);

	do {
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

		long currentPosition = (long) stepper[0].currentPosition();
		if (currentPosition % 1000 == 0) {
			Serial.print(" ");
			char currentPositionStr[6] = {};
			sprintf(currentPositionStr, "%5ld", currentPosition);
			Serial.print(currentPositionStr);
			Serial.print(" ");
			char h2o_amt_disStr[6] = {};
			sprintf(h2o_amt_disStr, "%6.2f", (double) h2o_amt_dis);
			Serial.print(h2o_amt_disStr);
			Serial.print(" ");
			for (int n = 0; n < NUTRIENTS; n++) {
				float nut_amt_dis = nut[n].getDispensedVolume();
				long nut_currentPosition = (long) nut[n].currentPosition();
				Serial.print("| ");
				char nut_currentPositionStr[6] = {};
				sprintf(nut_currentPositionStr, "%5ld", nut_currentPosition);
				Serial.print(nut_currentPositionStr);
				Serial.print(" ");
				char nut_amt_disStr[6] = {};
				sprintf(nut_amt_disStr, "%6.2f", (double) nut_amt_dis);
				Serial.print(nut_amt_disStr);
				Serial.print(" ");
			}
			Serial.println();
		}

		// TODO measure pH and adjust rate for final nutrient (pH-Down)

		// TODO Check alarms

		// TODO Log output
		
	} while (running_h2o);
	
	Serial.println("Done Discharging");
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
	senssor_loop();
#endif // _BME

#if defined (_DIS) && ( defined(_RTC) || defined(_SEN) || defined(_BME) )
	// display sensor readouts
	display_loop();
#endif // defined (_DIS) && ( defined(_RTC) || defined(_SEN) || defined(_BME) )

	// check key (and see if it is interrupt)
#ifdef _MENU
 #ifdef _KEY
	int key = check_key();

	switch (key) {
		// Enter menu
		//case KEY_NO_PRESS:
		case KEY_NO_CHANGE:
		  break;
		case KEY_START:
		  while (start_loop());
		  break;
		case KEY_STOP:
		  while (stop_loop());
		  break;
		case KEY_F1:
		  while (f1_loop());
		  break;
		case KEY_F2:
		  while (f2_loop());
		  break;
		case KEY_F3:
		  while (f3_loop());
		  break;
		case KEY_F4:
		  while (f4_loop());
		  break;
	}
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
