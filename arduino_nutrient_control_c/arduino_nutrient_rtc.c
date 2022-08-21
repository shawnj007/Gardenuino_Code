
#ifdef _RTC
time_t getDateTime(const char *str) {
	int Year, Month, Day, Hour, Min, Sec;
	tmElements_t dttm;
	if (sscanf(str, "%d/%d/%d %d:%d:%d", &Year, &Month, &Day, &Hour, &Min, &Sec) != 6) {
 #ifdef SER_OUT
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
 #ifdef SER_OUT
	sprintf(ser_output, "%s = %ld\n", str, (unsigned long) time);
	Serial.print(ser_output);
 #endif
	return time;
}

 #if defined(SER_OUT) || defined (SER_OUT_VERBOSE)
void print2digits(int number) {
	if (number >= 0 && number < 10) {
		Serial.write("0");
	}
	Serial.print(number);
}
 #endif

void read_rtc() {
	RTC.read(tm);
 #ifdef SER_OUT_VERBOSE
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
 
void print_rtc() {
	sprintf(dt_str, "%02d/%02d/%02d", tmYearToCalendar(tm.Year)%100, tm.Month, tm.Day);
	sprintf(tm_str, "%2d:%02d:%02d", tm.Hour, tm.Minute, tm.Second);
	sprintf(dttm_str, "%04d/%02d/%02d %2d:%02d:%02d", tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second);
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
 #ifdef SER_OUT
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
 #ifdef SER_OUT
				Serial.print(F("RTC time is: "));
				Serial.println(rtc_time_out);
 #endif
			}
		}
 #ifdef SER_OUT
		Serial.println(F("FAILED: Could not set RTC!"));
 #endif
 #ifdef _DO_HW_CHECK
		failed_rtc = true;
 #endif
	}
}

#endif // _RTC
