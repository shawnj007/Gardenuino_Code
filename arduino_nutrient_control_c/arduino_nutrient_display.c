
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

 /* Screen layout
  *     012345678901234567890
  *    +---------------------+
  * 0  |ZZOONNE i  WWEEEEK w |		Note: there are half-spaces
  * 1  |ZZOONNE i  WWEEEEK W |		Note: there are half-spaces
  * 2  |H2O  ml.sec  rem/totl|
  * 3  |Nut1 ml.sec  m.L r.em|
  * 4  |Nut2 ml.sec  m.L r.em|
  * 5  |Nut3 ml.sec  m.L r.em|
  * 6  |Nut4 ml.sec  m.L r.em|
  * 7  |Nut5 ml.sec  m.L r.em|
  *    +---------------------+
  */

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

 #if ( defined(_RTC) || defined(_SEN) || defined(_BME) )

void display_loop() {
	// TODO after others
	
 	if (millis_display_loop + interv_display_loop >= millis()) return;
	millis_display_loop = millis();

	// format all alarm, input, and sensor readouts to strings
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
	// display time
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
}

void water_loop(float *nut_amt_dis, 
				char sch_display[6][22],
				int h2o_rate_real,
				int h2o_rate,
				int h2o_rem_amt,
				int h2o_amt,
				int phase, 
				float *s_rate,
				int week,
				int zone) {
	
	float nut_rem_amt[NUTRIENTS] = {0};
	
	for (uint8_t n = 0; n < NUTRIENTS; ++n) {
		nut_rem_amt[n] = -(nut_amt_dis[n]);
	}
	
	sprintf(sch_display[0],
		"W %2d.%1d %2d.%1d %4d/%4d",
		(int) h2o_rate_real,
		((int) (h2o_rate_real * 10.0)) % 10,
		(int) h2o_rate,
		((int) (h2o_rate * 10.0)) % 10,
		(int) h2o_rem_amt,
		(int) h2o_amt);
	
	for (uint8_t n = 0; n < NUTRIENTS; ++n) {
		bool running = ((phase > 1) && (nut[n].targetPosition() != nut[n].currentPosition()));
		
		sprintf(sch_display[NUT_START + n],
				"Nut%1d %2d.%03d %2d.%01d %1d.%02d",
				NUT_START + n,
		        (running ? (int) s_rate[n] : 0),
		        (running ? (int) (s_rate[n] * 1000.0) % 1000 : 0),
		        (running ? (int) opt_nut_ratio[n][week] : 0),
		        (running ? (int) (opt_nut_ratio[n][week] * 10.0) % 10 : 0),
		        (int) (nut_rem_amt[n]),
		        (int) (nut_rem_amt[n] * 100.0) % 100);
	}
	
	// TODO, clear partial instead
	display.clearDisplay();
	display.setCursor(0, 0);
	display.setTextSize(2);
	display.print("Zone");
	
	display.setCursor(50, 0);
	display.setTextSize(1,2);
	display.print(zone);
	
	display.setCursor(68, 0);
	display.setTextSize(2);
	display.print("Week");
	
	display.setCursor(116, 0);
	display.setTextSize(1,2);
	display.print(week);
	
	display.setTextSize(1);
	for (int i = 0; i < 6; ++i) {
		display.setCursor(0, 16 + (i * 8));
		display.print(sch_display[i]);
	}
	display.display();
}
 #endif // ( defined(_SEN) || defined(_DIS) || defined(_BME) )

#endif // _DIS
