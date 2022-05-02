#include "PumpStepper.h"

#define _TYPE PumpStepper::SYRINGE
#define _H (float) 47.0
#define _V (float) 3.0

#define _INTERFACE AccelStepper::HALF4WIRE
#define _PIN1 48
#define _PIN2 49
#define _PIN3 30
#define _PIN4 31
#define _STOP_F A0
#define _STOP_B A1
#define _ENABLE true

SyringePumpStepper ps(	_TYPE,
						_H,
						_V,
						_INTERFACE,
						_PIN1,
						_PIN2,
						_PIN3,
						_PIN4,
						_STOP_F,
						_STOP_B,
						_ENABLE);

void setup() {
	//ps.calibratePumpMaxPosition();
	Serial.begin(230400);
	Serial.println("Pump setup");
	ps.setVolumeTime(0.1,10);
}

void loop() {
	while(ps.runSpeedToPositionToStop());
	Serial.println("Pump done");
	while(true);
}
