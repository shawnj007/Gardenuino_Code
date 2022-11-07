#include <stdio.h>

char f_str[12];

char *print_float(float f) {
	sprintf(f_str, "%5ld.%03ld", (long)f, (long)(f * 1000) % 1000);
#ifdef SER_OUT_VERBOSE
	Serial.print(F("Converting "));
	Serial.print(f);
	Serial.print(F(" to "));
	Serial.println(f_str);
#endif	// SER_OUT_VERBOSE
	return f_str;
}

#ifdef _SEN
bool print_alarms() {
	sprintf(alarm_state, "%5s%4s%3s%4s%3s",
			alarm_flood ? "FLOOD" : "",

 #ifdef _FLO
			alarm_flow ? "FLOW" : "",
 #else
			"noFL",
 #endif

 #ifdef _NUT
			alarm_nut ? "NUT" : "",
 #else
			"noN",
 #endif
			alarm_soil ? "SOIL" : "",
			alarm_env ? "ENV" : "");
	return false;
}
#endif

void print_inputs() {
	// TODO later
	// prepare humidifier sensors
	// prepare soil sensors
	//return false;
}

#ifdef _BME
void print_sensors() {

 #ifdef SER_OUT_VERBOSE
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
 #endif	 // SER_OUT_VERBOSE
	//sprintf(tph_str, "%6.2f*C %6.2hP %6.2%%\n", t, p, h);
	sprintf(t_str, "%4d.%01doC", (int)t, (int)(t * 10) % 10);
	sprintf(p_str, "%4d.%01dhP", (int)p, (int)(p * 10) % 10);
	sprintf(h_str, "%4d.%01d%%", (int)h, (int)(h * 10) % 10);

	sprintf(tph_str, "%s %s %s", t_str, p_str, h_str);
}
#endif	// _BME