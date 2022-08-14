
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

void h2o_tick_in() {
	++h2o_ticks[0];
}

void h2o_tick_out() {
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
float calculate_n_rate(float n_ratio, float h2o_n_rate) { //, float h2o_amt, float h2o_time) {
	// determine how much nutrient remains to be dispensed
	float h2o_n_lps = h2o_n_rate / 1000.0;
	float n_rate = h2o_n_lps * n_ratio;
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
unsigned long set_h2o_rate(uint8_t s, float h2o_n_rate) { // rate in mL / sec
	float h2o_sps = min(1000, calculate_n_speed(h2o_n_rate));
 #ifdef SERIAL_OUT_VERBOSE
	Serial.print(F("set h2o sps rate: "));
	Serial.println(h2o_sps);
 #endif
	stepper[s].setSpeed(h2o_sps);
	return (unsigned long) h2o_sps;
}
#endif // _H2O

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

#if defined(_NUT) || defined(_H2O)
void disable_steppers() {
	unsigned long c_time = millis();
	while (c_time + 1000 >= millis());
	for (uint8_t s = 0; s < COUNT_STEPPERS; ++s) {
		stepper[s].stop();
		//stepper[s].setCurrentPosition(0);
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


