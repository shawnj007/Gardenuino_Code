// I2C device headers
#include <Wire.h>

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#include <Adafruit_SSD1306.h>
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long h2o_ticks = 0;

void h2o_tick_in () {
	++h2o_ticks;
}

void setup_display() {
	if(display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
		display.clearDisplay();
		display.setTextColor(SSD1306_WHITE);
		display.print("passed");
		display.display();
		delay(1000);
		display.clearDisplay();
		display.display();
	}
}

void setup_valve() {
	pinMode(5, OUTPUT);
	digitalWrite(5, LOW);
}

void open_valve(bool enable) {
	digitalWrite(5, enable ? HIGH : LOW);
}

void setup_flow_rate() {
	pinMode(2, INPUT);
	digitalWrite(2, LOW);
	attachInterrupt(digitalPinToInterrupt(2), h2o_tick_in, RISING);
}

unsigned long loop_time_start = 0;
unsigned long loop_time = 10000;

void setup() {
	setup_display();
	setup_flow_rate();
	setup_valve();
	open_valve(true);
	loop_time_start = millis();
}

unsigned long millis_display_loop;
unsigned long interv_display_loop = 10;

void display_loop() {
	if (millis_display_loop + interv_display_loop >= millis()) return;
	millis_display_loop = millis();

	display.clearDisplay();
	display.setTextSize(2);
	display.setCursor(0,0);
	display.print(h2o_ticks);
	display.setCursor(0,16);
	display.print(millis() - loop_time_start);
	display.display();
}

void loop() {
	display_loop();
	
	if (millis() - loop_time_start > loop_time) {
		display_loop();
		open_valve(false);
		while(true);
	}
}
