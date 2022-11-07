/* Idling Screen layout
  *     012345678901234567890
  *    +---------------------+	
  *	0  |YY/MM/DD  W_day hh:mm|
  *	1  |hh:mm:ss  N_day hh:mm|
  *	2  |  23.4oC  Humi1 Flor1|	
  *	3  |  23.4%       2     2|
  *	4  | 123.4hP  Soil1  Pan1|
  * 5  |H2O SUPPL     2     2|
  * 6  |FLOW SOIL     3     3|
  * 7  |FLOOD ENV     4     4|
  *    +---------------------+
  */

/* Watering Screen layout
  *     012345678901234567890
  *    +---------------------+
  * 0  |ZONE i WEEK W  hh:mm |
  * 1  |pH 7.0 DS_xxx EC_xxx |
  * 2  |H2O  ml.sec  rem/totl|
  * 3  |Nut1 ml.sec  m.L r.em|
  * 4  |Nut2 ml.sec  m.L r.em|
  * 5  |Nut3 ml.sec  m.L r.em|
  * 6  |CaMg ml.sec  m.L r.em|
  * 7  |pH+  ml.sec  m.L r.em|
  *    +---------------------+
  *
  *    +---------------------+
  * 0  |ZZOONNE i  WWEEEEK W |		Note: there are half-spaces
  * 1  |ZZOONNE i  WWEEEEK W |		Note: there are half-spaces
  *   
  */

/*
  *    +---------------------+
  * 0  |                     |
  * 1  |                     |
  * 2  |                     |
  * 3  |                     |
  * 4  |                     |
  * 5  |H2O SUPPL            |
  * 6  |FLOW SOIL            |
  * 7  |FLOOD ENV            |
  *    +---------------------+
  *
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

#ifdef _DIS
void setup_display() {
	if (display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
		display.display();
 #ifdef SER_OUT
		Serial.println(F("SSD1306 allocation success"));
 #endif
	} else {
 #ifdef _DO_HW_CHECK
		failed_dis = true;
 #endif
 #ifdef SER_OUT
		Serial.println(F("FAILED: SSD1306 allocation failed!"));
 #endif
	}
}

 #if (defined(_RTC) || defined(_SEN) || defined(_BME))

void loop_display_idle() {
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
	display.setTextSize(1);				  // Normal 1:1 pixel scale
	display.setTextColor(SSD1306_WHITE);  // Draw white text

  #ifdef _RTC
	// display date time

	// 0  |YY/MM/DD             |
	display.setCursor(0, 0);
	display.print(dt_str);

	// 1  |hh:mm:ss             |
	display.setCursor(0, 8);
	display.print(tm_str);
  #endif

  #ifdef _BME
	// display environmental sensors

	// 2  |  21.0oC             |
	display.setCursor(0, 16);
	display.print(t_str);
	// 3  |  54.3%              |
	display.setCursor(0, 24);
	display.print(h_str);
	// 4  | 987.6hP             |
	display.setCursor(0, 32);
	display.print(p_str);
  #endif

  #ifdef _SEN
	// display humidifier, soil, floor, and pan sensors

	//    +---------------------+
	// 2  |         Humi1       |
	display.setCursor(60, 16);
	//display.print();

	// 3  |         Humi2       |
	display.setCursor(60, 24);
	//display.print();

	// 4  |         Soil1       |
	display.setCursor(60, 32);
	//display.print();

	// 5  |         Soil2       |
	display.setCursor(60, 40);
	//display.print();

	// 6  |         Soil3       |
	display.setCursor(60, 48);
	//display.print();

	// 7  |         Soil4       |
	display.setCursor(60, 56);
	//display.print();

	// 2  |               Flor1 |
	display.setCursor(96, 16);
	//display.print();

	// 3  |               Flor1 |
	display.setCursor(96, 24);
	//display.print();

	// 4  |                Pan1 |
	display.setCursor(96, 32);
	//display.print();

	// 5  |                Pan2 |
	display.setCursor(96, 40);
	//display.print();

	// 6  |                Pan3 |
	display.setCursor(96, 48);
	//display.print();

	// 7  |                Pan4 |
	display.setCursor(96, 56);
	//display.print();

	// 5  |FLOOD ENV            |
	display.setCursor(0, 40);
	display.print(alarm_str[0]);

	// 6  |FLOW SOIL            |
	display.setCursor(0, 48);
	display.print(alarm_str[1]);

	// 7  |NUT SUPPL            |
	display.setCursor(0, 56);
	display.print(alarm_str[2]);

			//display.print(sys_status);

  #endif

	display.display();
}

void loop_display_water(float *nut_amt_dis,
						char sch_display[6][22],
						int h2o_rate_real,
						int h2o_rate,
						int h2o_rem_amt,
						int h2o_amt,
						int phase,
						float *s_rate,
						int week,
						int zone) {

  #ifdef _NUT
	float nut_rem_amt[NUTRIENTS] = { 0 };

	for (uint8_t n = 0; n < NUTRIENTS; ++n) {
		nut_rem_amt[n] = -(nut_amt_dis[n]);
	}
  #endif

	sprintf(sch_display[0],
			"W %2d.%1d %2d.%1d %4d/%4d",
			(int)h2o_rate_real,
			((int)(h2o_rate_real * 10.0)) % 10,
			(int)h2o_rate,
			((int)(h2o_rate * 10.0)) % 10,
			(int)h2o_rem_amt,
			(int)h2o_amt);

  #ifdef _NUT
	for (uint8_t n = 0; n < NUTRIENTS; ++n) {
		bool running = ((phase > 1) && (nut[n].targetPosition() != nut[n].currentPosition()));

		sprintf(sch_display[NUT_START + n],
				"Nut%1d %2d.%03d %2d.%01d %1d.%02d",
				NUT_START + n,
				(running ? (int)s_rate[n] : 0),
				(running ? (int)(s_rate[n] * 1000.0) % 1000 : 0),
				(running ? (int)opt_nut_ratio[n][week] : 0),
				(running ? (int)(opt_nut_ratio[n][week] * 10.0) % 10 : 0),
				(int)(nut_rem_amt[n]),
				(int)(nut_rem_amt[n] * 100.0) % 100);
	}
  #endif

	// TODO, clear partial instead
	display.clearDisplay();
	display.setCursor(0, 0);
	display.setTextSize(2);
	display.print("Zone");

	display.setCursor(50, 0);
	display.setTextSize(1, 2);
	display.print(zone);

	display.setCursor(68, 0);
	display.setTextSize(2);
	display.print("Week");

	display.setCursor(116, 0);
	display.setTextSize(1, 2);
	display.print(week);

	display.setTextSize(1);
	for (int i = 0; i < 6; ++i) {
		display.setCursor(0, 16 + (i * 8));
		display.print(sch_display[i]);
	}
	display.display();
}

 #endif	 // ( defined(_RTC) || defined(_SEN) || defined(_BME) )

#endif	// _DIS