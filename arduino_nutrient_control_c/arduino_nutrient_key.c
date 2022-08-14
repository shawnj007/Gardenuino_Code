
#ifdef _KEY
int ROW[4] = {P0, P1, P2, P3};
int COL[4] = {P4, P5, P6, P7};

uint8_t key_[4][4] = {{ 1, 2,  3, 10}, 
                      { 4, 5,  6, 11},
                      { 7, 8,  9, 12},
                      {15, 0, 14, 13}};

#define KEY_NO_PRESS 0
#define KEY_NO_CHANGE 0

#define KEY_UL 1
#define KEY_UR 3
#define KEY_DL 7
#define KEY_DR 9

#define KEY_CR 5

#define KEY_UP 2
#define KEY_LT 4
#define KEY_RT 6
#define KEY_DN 8

#define KEY_F1 11
#define KEY_F2 12
#define KEY_F3 13
#define KEY_F4 14

#define KEY_STOP  15
#define KEY_START 16

#define MENU_MAIN 0
#define MENU_MANUAL 1

float last_key = KEY_NO_PRESS;
float menu_pos = 0; // fmt: XX.YY
char menu_buff[8];

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
 
void key_loop() {
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
}
 #endif // _MENU
#endif // _KEY
