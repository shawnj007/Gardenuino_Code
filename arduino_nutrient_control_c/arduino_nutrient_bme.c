
#ifdef _BME
void setup_bme() {
	if (bme.begin(0x76)) {
 #ifdef SER_OUT
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
 #ifdef SER_OUT
		Serial.println(F("FAILED: Could not find a valid BME280 sensor, check wiring!"));
 #endif
 #ifdef _DO_HW_CHECK
		failed_bme = true;
 #endif
	}
}

void loop_bme() {
	t = bme.readTemperature();
	p = bme.readPressure() / 100.0F;
	h = bme.readHumidity();
}
#endif // _BME
