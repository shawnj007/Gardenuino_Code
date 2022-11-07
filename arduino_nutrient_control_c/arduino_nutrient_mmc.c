#ifdef _MMC
void setup_mmc() {
 #ifdef SER_OUT
	Serial.println(F("Setting up MMC"));
 #endif
	if (card.init(SPI_HALF_SPEED, SS_MMC)) {
		// print the type of card
 #ifdef SER_OUT_VERBOSE
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
 #ifdef SER_OUT
		Serial.println(F("FAILED: SD card initialization failed."));
 #endif
 #ifdef SER_OUT_VERBOSE
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
  #ifdef SER_OUT_VERBOSE
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
  #ifdef SER_OUT_VERBOSE
		Serial.print(F("Volume type is:    FAT"));
		Serial.println(volume.fatType(), DEC);
  #endif
		volumesize = volume.blocksPerCluster();	 // clusters are collections of blocks
		volumesize *= volume.clusterCount();	 // we"ll have a lot of clusters
		volumesize /= 2;						 // SD card blocks are always 512 bytes (2 blocks are 1KB)
  #ifdef SER_OUT_VERBOSE
		Serial.print(F("Volume size (Kb):  "));
		Serial.println(volumesize);
		Serial.print(F("Volume size (Mb):  "));
  #endif
		volumesize /= 1024;
  #ifdef SER_OUT_VERBOSE
		Serial.println(volumesize);
		Serial.print(F("Volume size (Gb):  "));
		Serial.println((float)volumesize / 1024.0);
		Serial.println(F("\nFiles found on the card (name, date and size in bytes): "));
  #endif
		root.openRoot(volume);

		// list all files in the card with date and size
		root.ls(LS_R | LS_DATE | LS_SIZE);
		if (!SD.begin(SS_MMC)) {
  #ifdef SER_OUT
			Serial.println(F("FAILED: MMC initialization failed!"));
  #endif
  #ifdef _DO_HW_CHECK
			failed_mmc = true;
  #endif  // _DO_HW_CHECK
		}
	} else
 #endif	 // _DO_HW_CHECK
	{
 #ifdef SER_OUT_VERBOSE
		Serial.println(F("FAILED: Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card"));
 #endif	 // SER_OUT_VERBOSE
 #ifdef _DO_HW_CHECK
		failed_mmc = true;
 #endif	 // _DO_HW_CHECK
	}
 #ifdef _RTC
	SdFile::dateTimeCallback(dateTime);
 #endif
 #ifdef SER_OUT
	Serial.println(F("MMC done"));
 #endif	 // SER_OUT
}

void backup_file(char *filename) {
	if (SD.exists(filename)) {
 #ifdef SER_OUT
		Serial.print(F("Backing up "));
		Serial.println(filename);
 #endif	 // SER_OUT
		File file_old = SD.open(filename, FILE_READ);
		char file_new_name[13] = {};
		sprintf(file_new_name, "old_%s", filename);
 #ifdef _RTC
		char data_header[20] = {};
		sprintf(data_header, "\nd%04d%02d%02dt%02d%02d%02d\n", tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second);
 #endif	 // _RTC
		File file_new = SD.open(file_new_name, FILE_WRITE);
		size_t n;
		uint8_t buf[64];
 #ifdef _RTC
		file_new.write(data_header);
 #endif	 // _RTC
		while ((n = file_old.read(buf, sizeof(buf))) > 0) file_new.write(buf, n);
		file_old.flush();
		file_old.close();
		file_new.flush();
		file_new.close();
 #ifdef SER_OUT
		Serial.println(F("Backup done"));
 #endif	 // SER_OUT
	}
}

 #ifdef _NUT
bool write_nut_file() {
  #ifdef SER_OUT
	Serial.println(F("Writing nutrient file."));
  #endif  // SER_OUT
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
  #ifdef SER_OUT
		Serial.println(F("FAILED to open nut.cfg"));
  #endif  // SER_OUT
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
  #endif  // defined(_FLO) || defined(_H2O)
	// write array[rows,cols] into nut amounts
	for (int i = 0; i < NUTRIENTS; ++i) {
		for (int j = 0; j < WEEKS; ++j) {
			sprintf(fil_output, "%4d.%01d", (int)opt_nut_ratio[i][j], (int)(opt_nut_ratio[i][j] * 10.0) % 10);
			file.write(fil_output);
		}
		file.write("\n");
	}
	file.write("\n");
	file.close();
  #ifdef SER_OUT
	Serial.println(F("Done"));
  #endif  //SER_OUT
	return true;
}

bool read_nut_file() {
  #ifdef SER_OUT
	Serial.println(F("Reading nutrient files"));
  #endif
	char filename[] = "nut.cfg";
	if (!SD.exists(filename)) write_nut_file();

	File file = SD.open(filename, FILE_READ);
	if (!file) return false;

	char val[7] = {};

	// read rows (nutrient count)
	file.read(val, 3);
	int nutrients = atoi(val);

	// read cols (weeks count)
	file.read(val, 3);
	int weeks = atoi(val);

	// verify that file matches compiled code parameters
	if (weeks != WEEKS) {
  #ifdef SER_OUT
		Serial.println(F("Week count does not match."));
  #endif
		while (true)
			;
	}
	if (nutrients != NUTRIENTS) {
  #ifdef SER_OUT
		Serial.println(F("Nutrient count does not match."));
  #endif
		while (true)
			;
	}


  #if defined(_FLO) || defined(_H2O)
	// read array[cols] into h2o array
	for (int j = 0; j < WEEKS; ++j) {
		file.read(val, 6);
		opt_h2o_amt[j] = atoi(val);
	}
	file.read(val, 1);
  #endif  // defined(_FLO) || defined(_H2O)
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
  #ifdef SER_OUT
	Serial.println(F("Done with nutrient files"));
  #endif  // SER_OUT
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
  #ifdef SER_OUT_VERBOSE
	Serial.print(F("Writing nutrient "));
	Serial.print(s);
	Serial.print(F(" remaining"));
	Serial.println(fil_output);
  #endif  // SER_OUT_VERBOSE
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
	char val[11] = {};
	file.read(val, 10);
  #ifdef SER_OUT_VERBOSE
	Serial.print(F("Reading nutrient "));
	Serial.print(s);
	Serial.print(F(" remaining"));
	Serial.println(val);
  #endif  // SER_OUT_VERBOSE
	file.close();
	return atof(val);
}
 #endif	 // _NUT

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
  #ifdef SER_OUT
		Serial.print(F("Writing "));
		sprintf(ser_output, "%s", type);
		Serial.print(ser_output);
		Serial.print(F(" "));
		sprintf(ser_output, "%d", zone);
		Serial.print(ser_output);
		Serial.println(F(" failed"));
  #endif  // SER_OUT
		return false;
	}
  #ifdef SER_OUT
	Serial.println();
  #endif  // SER_OUT
	// Write zone start date		YYYY/MM/DD HH:MM:SS AP
	if (times[0][0] == (char)0) {
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
  #ifdef SER_OUT_VERBOSE
			Serial.print(F("Wrote "));
			Serial.print(times[i]);
			Serial.print(F(" to file.\n"));
  #endif  // SER_OUT_VERBOSE
		}
	}
	file.flush();
	file.close();
  #ifdef SER_OUT
	Serial.println(F("Done"));
  #endif  // SER_OUT
	return true;
}

bool read_zone_file(const char *type, int zone, char times[3][20]) {
  #ifdef SER_OUT
	Serial.print(F("Reading "));
	Serial.print(type);
	Serial.print(F(" "));
	Serial.println(zone);
  #endif  // SER_OUT

	char filename[13];
	sprintf(filename, "%s_%01d.cfg", type, zone);

	if (!SD.exists(filename)) {
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 20; ++j)
				times[i][j] = (char)0;
		write_zone_file(type, zone, times);
	}

	File file = SD.open(filename, FILE_READ);
	if (!file) {
  #ifdef SER_OUT
		Serial.println(F(" failed"));
  #endif  // SER_OUT
		return false;
	}

	// Read zone dates
	for (int i = 0; i < 3; ++i) {
		file.read(times[i], 20);
		times[i][19] = '\0';
	}

	// TODO verify that file matches compiled code parameters (counts)

	file.close();
  #ifdef SER_OUT
	Serial.println(F("Done"));
  #endif  // SER_OUT
	return true;
}
 #endif	 // _RTC

bool write_zone_log_file(const char *type, int zone, char *log_entry) {
 #ifdef SER_OUT_VERBOSE
	Serial.println(F("Writing zone log file."));
 #endif	 // SER_OUT_VERBOSE
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
 #ifdef SER_OUT
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
 #ifdef SER_OUT
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
#ifdef SER_OUT
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
 #ifdef SER_OUT
	Serial.println(F("Reading config files."));
 #endif	 // SER_OUT
 #ifdef _NUT
	read_nut_file();
 #endif	 // _NUT
	// TODO
 #ifdef _SEN
	for (int i = 0; i < COUNT_HUMID; ++i) {
		//		read_zone_file("humid", i);
	}

	for (int i = 0; i < COUNT_PLANTS; ++i) {
		//		read_zone_file("zone", i);
	}
 #endif	 // _SEN
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
  #ifdef SER_OUT
			Serial.println(log_string[n]);
  #endif  // SER_OUT
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
  #endif  // _RTC
  #ifdef _BME
			tph_str
  #else
			""
  #endif  // _BME
	);
	if (s == LOG_CACHE_SIZE) s = 0;
}

void loop_log() {
	if (millis_cache_log + millis_interv_cache_log < millis()) {
		millis_cache_log = millis();
		cache_log();
	}
	if (millis_log + millis_interv_log >= millis()) return;
	millis_log = millis();

  #ifdef _DO_HW_CHECK
	// check if MMC is valid
	if (failed_mmc) return;
  #endif  // _DO_HW_CHECK

  #ifdef _DIS
	display.setCursor(0, 56);  // Start at top-left corner
	display.print("WRITING");
	display.display();
  #endif  // _DIS

	// check if log file exists
	// open/write/close file
	if (!write_log_file()) {  // if the file isn"t open, pop up an error:
  #ifdef SER_OUT
		Serial.println(F("error writing to system.log"));
  #endif  // SER_OUT
	}
  #ifdef _DIS
	display.setCursor(40, 56);	// Start at top-left corner
	display.print("..DONE.");
	display.display();
  #endif  // _DIS
}
 #endif	 // defined(_MMC) && defined(_LOG)
#endif	 // _MMC