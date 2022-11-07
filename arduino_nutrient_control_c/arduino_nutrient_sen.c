#ifdef _SEN
void setup_sensors() {
 #ifdef SER_OUT_VERBOSE
	Serial.println(F("Setting up sensors"));
 #endif	 // SER_OUT_VERBOSE
	// 12 analog sensors
	for (uint8_t s = 0; s < COUNT_SENSORS; ++s) {
		pinMode(SENSORS[s], INPUT);
		analogWrite(SENSORS[s], LOW);
 #ifdef SER_OUT_VERBOSE
		Serial.print(F("Setup Sensor "));
		Serial.println(s);
 #endif	 // SER_OUT_VERBOSE
	}
 #ifdef SER_OUT
	Serial.println(F("Sensors done"));
 #endif	 // SER_OUT
}

bool check_alarm_flood(int zone) {
	// TODO later design/build sensor
 #ifdef SER_OUT
	if (alarm_flood) Serial.println(F("ALARM\nALARM: flood\nALARM"));
 #endif	 // SER_OUT
	return alarm_flood;
}

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
 #ifdef SER_OUT
	if (alarm_nut) Serial.println(F("ALARM\nALARM: nutrient\nALARM"));
 #endif	 // SER_OUT
	return alarm_nut;
}

bool check_alarm_humid(int zone) {
	// TODO
	return false;
}

bool check_alarm_soil(int zone) {
	int value = analogRead(PLANTS_SENSE[zone]);
	if (value < SOIL_MOIST_MIN) alarm_soil = -1;
	if (value > SOIL_MOIST_MAX) alarm_soil = 1;
	if (alarm_soil) Serial.println(F("ALARM\nALARM: soil\nALARM"));
	return (alarm_soil != 0);
}

bool check_alarm_pan(int zone) {
	// TODO
	return false;
}

bool check_alarm_env() {
	// TODO later
	// Check environmental limits
	// Check temperature limits
	// Check humidity limits
 #ifdef SER_OUT
	if (alarm_env) Serial.println(F("ALARM\nALARM: environment\nALARM"));
 #endif	 // SER_OUT
	return alarm_env;
}

bool check_alarm_int() {
	// TODO later design/build interrupt
	// Check for interrupt
 #ifdef SER_OUT
	if (alarm_int) Serial.println(F("ALARM\nALARM: interrupt\nALARM"));
 #endif	 // SER_OUT
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

bool check_alarms() {
	return false
 #ifdef _SEN
		   || check_alarm_flood(0)
		   || check_alarm_flood(1)

		   || check_alarm_humid(0)
		   || check_alarm_humid(1)

		   || check_alarm_soil(0)
		   || check_alarm_soil(1)
		   || check_alarm_soil(2)
		   || check_alarm_soil(3)

		   || check_alarm_pan(0)
		   || check_alarm_pan(1)
		   || check_alarm_pan(2)
		   || check_alarm_pan(3)

		   || check_alarm_env()
		   || check_alarm_int()

  #ifdef _NUT
		   || check_alarm_nut()
  #endif  // _NUT

 #endif	 // _SEN
 #ifdef _FLO
		//|| check_alarm_flow() 			// FIXME
 #endif	 // _FLO
		;
}

bool check_inputs() {
	// TODO later
	// Check soil moisture sensors
	// Check humidifier tank sensors



	return false;
}

#endif	// _SEN

/*
*/

#ifdef _FLO
bool check_alarm_flow() {
 #ifdef _H2O
	if ((h2o_rate < FLOW_RATE_MIN) || (h2o_rate > FLOW_RATE_MAX)) alarm_flow = true;
 #endif
 #ifdef SER_OUT
	if (alarm_flow) Serial.println(F("ALARM\nALARM: flow\nALARM"));
 #endif	 // SER_OUT
	return alarm_flow;
}
#endif	// _FLO