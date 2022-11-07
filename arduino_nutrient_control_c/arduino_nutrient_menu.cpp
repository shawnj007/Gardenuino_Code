#ifndef _MENU_H
#define _MENU_H

#include <Arduino.h>

#define I2C_ADDRESS 0x3C
// #include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>

#include <menu.h>
#include <menuIO/SSD1306AsciiOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIO.h>

//enable this include if using esp8266
// #include <menuIO/esp8266Out.h>
using namespace Menu;

extern bool running_h2o;
extern bool running_nut;
extern float opt_nut_ratio[5][13];

//Define your font here. Default font: lcd5x7
//#define menuFont X11fixed7x14
//#define fontW 7
//#define fontH 15

SSD1306AsciiWire oled;

// #define menuFont System5x7
#define menuFont lcd5x7
#define fontW 5
#define fontH 8

/*
MENU(system_status, "System Status", doNothing, noEvent, noStyle
     , EXIT("<Back")
    );
*/

bool run_h2o = false;
bool run_nut = false;
bool run_auto = false;

/*
result setRunning() {
	running_h2o = run_h2o;
	running_nut = run_nut;
}
      
TOGGLE(run_h2o, set_h2o, "H2O", doNothing, noEvent, noStyle
       , VALUE("On",true,setRunning,enterEvent)
       , VALUE("Off",false,setRunning,enterEvent)
      );
      
TOGGLE(run_nut, set_nut, "NUT", doNothing, noEvent, noStyle
       , VALUE("On",true,setRunning,enterEvent)
       , VALUE("Off",false,setRunning,enterEvent)
      );
      
MENU(system_control, "System Control", doNothing, noEvent, noStyle
     , SUBMENU(set_h2o)
     , SUBMENU(set_nut)
     , EXIT("<Back")
    );
*/
/*     
MENU(system_records, "System Records", doNothing, noEvent, noStyle
     , EXIT("<Back")
    );
     
MENU(h2o_nut_schedule, "H2O/Nut Schedule", doNothing, noEvent, noStyle
     , EXIT("<Back")
    );
float opt_nut_ratio_4=0;A

*/

unsigned int week_disp = 0;
unsigned int week_n = 1;

float opt_nut_ratio_0 = 0;
float opt_nut_ratio_1 = 0;
float opt_nut_ratio_2 = 0;
float opt_nut_ratio_3 = 0;
float opt_nut_ratio_4 = 0;

result updateValues() {
	// TODO: Read or store to the opt_nut_ratio variable and MMC
	if (week_n == week_disp) {
#ifdef _NUT
		// update stored from menu value, since week has changed
		if (opt_nut_ratio[0][week_n] != opt_nut_ratio_0) opt_nut_ratio[0][week_n] = opt_nut_ratio_0;
		if (opt_nut_ratio[1][week_n] != opt_nut_ratio_1) opt_nut_ratio[1][week_n] = opt_nut_ratio_1;
		if (opt_nut_ratio[2][week_n] != opt_nut_ratio_2) opt_nut_ratio[2][week_n] = opt_nut_ratio_2;
		if (opt_nut_ratio[3][week_n] != opt_nut_ratio_3) opt_nut_ratio[3][week_n] = opt_nut_ratio_3;
		if (opt_nut_ratio[4][week_n] != opt_nut_ratio_4) opt_nut_ratio[4][week_n] = opt_nut_ratio_4;
#endif
	} else {
#ifdef _NUT
		// update menu from stored values, since week has changed
		if (opt_nut_ratio[0][week_n] != opt_nut_ratio_0) opt_nut_ratio_0 = opt_nut_ratio[0][week_n];
		if (opt_nut_ratio[1][week_n] != opt_nut_ratio_1) opt_nut_ratio_1 = opt_nut_ratio[1][week_n];
		if (opt_nut_ratio[2][week_n] != opt_nut_ratio_2) opt_nut_ratio_2 = opt_nut_ratio[2][week_n];
		if (opt_nut_ratio[3][week_n] != opt_nut_ratio_3) opt_nut_ratio_3 = opt_nut_ratio[3][week_n];
		if (opt_nut_ratio[4][week_n] != opt_nut_ratio_4) opt_nut_ratio_4 = opt_nut_ratio[4][week_n];
#endif
		week_disp = week_n;
	}
	return proceed;
}

MENU(h2o_nut_ratios, "Nutrient Ratios", updateValues, anyEvent, noStyle, FIELD(week_n, "Week", "w", 1, 13, 1, 1, updateValues, anyEvent, noStyle)
#ifdef _NUT
																			 ,
	 FIELD(opt_nut_ratio_0, "Nut1", "mL/L", 0.0, 10.0, 1.0, 0.1, updateValues, anyEvent, noStyle), FIELD(opt_nut_ratio_1, "Nut2", "mL/L", 0.0, 10.0, 1.0, 0.1, updateValues, anyEvent, noStyle), FIELD(opt_nut_ratio_2, "Nut3", "mL/L", 0.0, 10.0, 1.0, 0.1, updateValues, anyEvent, noStyle), FIELD(opt_nut_ratio_3, "Nut4", "mL/L", 0.0, 10.0, 1.0, 0.1, updateValues, anyEvent, noStyle), FIELD(opt_nut_ratio_4, "Nut5", "mL/L", 0.0, 10.0, 1.0, 0.1, updateValues, anyEvent, noStyle)
#endif
																																																																																																 ,
	 EXIT("<Back"));

/*
MENU(h2o_nut_balance, "Nutrient Balance", doNothing, noEvent, noStyle
     , EXIT("<Back")
    );
     
MENU(h2o_nut_records, "Nutrient Records", doNothing, noEvent, noStyle
     , EXIT("<Back")
    );
*/

MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle, SUBMENU(h2o_nut_ratios)
	 //   , SUBMENU(system_control)
	 ,
	 EXIT("<Back"));
/*  
     , SUBMENU(system_status)
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

#define MAX_DEPTH 3
idx_t tops[MAX_DEPTH] = { 0, 0, 0 };  //store cursor positions for each level

const panel panels[] MEMMODE = { { 0, 0, 128 / fontW, 64 / fontH } };
navNode* nodes[sizeof(panels) / sizeof(panel)];	 //navNodes to store navigation status
panelsList pList(panels, nodes, 1);				 //a list of panels and nodes

SSD1306AsciiOut outOLED(&oled, tops, pList, 5, 1);	//oled output device menu driver

menuOut* constMEM outputs[] MEMMODE = { &outOLED };	 //list of output devices
outputsList out(outputs, 1);						 //outputs list

serialIn serial(Serial);
MENU_INPUTS(in, &serial);

NAVROOT(nav, mainMenu, MAX_DEPTH, in, out);

#endif