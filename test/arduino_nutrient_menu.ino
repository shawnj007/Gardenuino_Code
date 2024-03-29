/*
System status
	Idle screen
	Water screen (auto)

System control
	Water On/Off
	Nutrient On/Off
	Auto pH On/Off
	Log On/Off

System records
	pH, TDS, EC, dOx
	Temp, Pressure, Humidity
	Flood, Pans, Plants

Water / Nutrient Schedule
	Field - Minimum H2O interval
	Field - Maximum H2O interval

Water / Nutrient Ratio Control
	Field - Week
		Field - Water Amt			mL			opt_h2o_amt
		Field - Nutratio_1			mL / L		opt_nut_ratio
		.
		Field - Nutratio_n
		
Water / Nutrient Balance Control
	Field - Week
		Field - TDS
		Field - pH
		Field - EC
		Field - dOx
	
Water / Nutrient Records
	Field - Zone
		Field - Week
			Field - Water Amt		mL
			Field - Nutratio_1		mL / L
			.
			Field - Nutratio_n
			pH, TDS, EC, dOx

Nutrient Amount Remaining
	Field - Nutrient_1				mL			opt_nut_rem
	.
	Field - Nutrient_n
	
*/


#ifndef _MENU_H
#define _MENU_H


#include <Arduino.h>

#define I2C_ADDRESS 0x3C
// #include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#include <menu.h>
#include <menuIO/SSD1306AsciiOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIO.h>

//enable this include if using esp8266
// #include <menuIO/esp8266Out.h>
using namespace Menu;

//Define your font here. Default font: lcd5x7
#define menuFont X11fixed7x14
#define fontW 7
#define fontH 15

#define MAX_DEPTH 1

SSD1306AsciiWire oled;

// #define menuFont System5x7
// #define menuFont lcd5x7
// #define fontW 5
// #define fontH 8

/*
MENU(system_status, "System Status", doNothing, noEvent, noStyle
     , EXIT("<Back")
    );
     
MENU(system_control, "System Control", doNothing, noEvent, noStyle
     , EXIT("<Back")
    );
     
MENU(system_records, "System Records", doNothing, noEvent, noStyle
     , EXIT("<Back")
    );
     
MENU(h2o_nut_schedule, "H2O/Nut Schedule", doNothing, noEvent, noStyle
     , EXIT("<Back")
    );
*/

unsigned int week_n=1;
float opt_nut_ratio_0=0;

MENU(h2o_nut_ratios, "Nutrient Ratios", doNothing, noEvent, noStyle
     , FIELD(week_n, "Week", "w", 1, 13, 1, 1, doNothing, noEvent, noStyle)
     , FIELD(opt_nut_ratio_0, "Nut1", "mL/L", 0.0, 10.0, 0.1, 0.1, doNothing, noEvent, wrapStyle)
     , EXIT("<Back")
    );
     //, FIELD(opt_nut_ratio[1][week_n], "Nut2", "mL/L", 0.0, 10.0, 0.1, 0.1, doNothing, noEvent, wrapStyle)
     //, FIELD(opt_nut_ratio[2][week_n], "Nut3", "mL/L", 0.0, 10.0, 0.1, 0.1, doNothing, noEvent, wrapStyle)
     //, FIELD(opt_nut_ratio[3][week_n], "Nut4", "mL/L", 0.0, 10.0, 0.1, 0.1, doNothing, noEvent, wrapStyle)
     //, FIELD(opt_nut_ratio[4][week_n], "Nut5", "mL/L", 0.0, 10.0, 0.1, 0.1, doNothing, noEvent, wrapStyle)
/*
MENU(h2o_nut_balance, "Nutrient Balance", doNothing, noEvent, noStyle
     , EXIT("<Back")
    );
     
MENU(h2o_nut_records, "Nutrient Records", doNothing, noEvent, noStyle
     , EXIT("<Back")
    );
*/

MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle
     , SUBMENU(h2o_nut_ratios)	
     , EXIT("<Back")
    );
/*  
     , SUBMENU(system_status)
     , SUBMENU(system_control)
     , SUBMENU(system_records)
     , SUBMENU(h2o_nut_schedule)
     , SUBMENU(h2o_nut_balance)
     , SUBMENU(h2o_nut_records)
*/
     /*
     , OP("Op1", action1, anyEvent)
     , OP("Op2", action2, enterEvent)
     , SUBMENU(subMenu1)
     , SUBMENU(setLed)
     , SUBMENU(selMenu)
     , SUBMENU(chooseMenu)
     , OP("LED On", internalLedOn, enterEvent) // will turn on built-in LED
     , OP("LED Off", internalLedOff, enterEvent)// will turn off built-in LED
     , OP("Alert test", doAlert, enterEvent)
     , EXIT("<Back")
     */
     /* FIELD Parameters :
        Action Name(function name), Action Heading, Action Heading Unit,
        range_lowest, range_highest, range_increment_step,
        range_decrement_step
     */
     //, FIELD(brightnessValue, "Brightness", "%", 0, 100, 5, 5, adjustBrightness, enterEvent, wrapStyle)

/*
*/
#define MAX_DEPTH 1

idx_t tops[MAX_DEPTH] = {0}; //, 0, 0}; //store cursor positions for each level

const panel panels[] MEMMODE = {{0, 0, 128 / fontW, 64 / fontH}};
navNode* nodes[sizeof(panels) / sizeof(panel)]; //navNodes to store navigation status
panelsList pList(panels, nodes, 1); //a list of panels and nodes

SSD1306AsciiOut outOLED(&oled, tops, pList, 5, 1); //oled output device menu driver

menuOut* constMEM outputs[]  MEMMODE  = {&outOLED}; //list of output devices
outputsList out(outputs, 1); //outputs list

serialIn serial(Serial);
MENU_INPUTS(in,&serial);

NAVROOT(nav, mainMenu, MAX_DEPTH, in, out);

#endif

void setup() {
	while(1);
}

void loop() {
	while(1);
}
