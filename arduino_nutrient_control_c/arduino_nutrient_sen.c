
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

bool check_alarm_flood() {
	// TODO later design/build sensor
 #ifdef SERIAL_OUT
	if (alarm_flood) Serial.println(F("ALARM\nALARM: flood\nALARM"));
 #endif // SERIAL_OUT
	return alarm_flood;
}

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

bool check_alarms() {
	return false;
}

#endif // _SEN
/*
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
*/

#ifdef _FLO
bool check_alarm_flow() {
	if ((h2o_rate < FLOW_RATE_MIN) || (h2o_rate > FLOW_RATE_MAX)) alarm_flow = true;
 #ifdef SERIAL_OUT
	if (alarm_flow) Serial.println(F("ALARM\nALARM: flow\nALARM"));
 #endif // SERIAL_OUT
	return alarm_flow;
}
#endif // _FLO
