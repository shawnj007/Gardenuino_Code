// Constants
#define MAXSPEED 1500
#define MAXPOSITION (925 * 50)
#define COUNT_STEPPERS 6
#define WIRES 4
#define RATIO (3.0)

// Imports 
#include <AccelStepper.h>
AccelStepper stepper[COUNT_STEPPERS] = {};
#include <MultiStepper.h>
MultiStepper steppers;

// Stepper pins
const uint8_t STEPPER_WIRES[COUNT_STEPPERS][WIRES] = {{ 49, 48, 30, 31 },
                                                      { 44, 45, 46, 47 },
                                                      { 40, 41, 42, 43 },
                                                      { 36, 37, 38, 39 },
                                                      { 32, 33, 34, 35 },
                                                      { 66, 67, 68, 69 }};

char output[64] = {};

long positions[6] = { 0, 0, 0, 0, 0, 0 };

void setup() {
	// Setup stepper pins
	// Setup stepper rates
	for (uint8_t s = 0; s < COUNT_STEPPERS; ++s) {
		stepper[s] = AccelStepper(AccelStepper::FULL4WIRE, STEPPER_WIRES[s][0], STEPPER_WIRES[s][1], STEPPER_WIRES[s][2], STEPPER_WIRES[s][3]);
		//stepper[s].moveTo(MAXPOSITION/RATIO);
		stepper[s].setMaxSpeed(MAXSPEED/RATIO);
		//stepper[s].setSpeed(MAXSPEED/RATIO);
		steppers.addStepper(stepper[s]);
		positions[s] = MAXPOSITION/RATIO;
	}

	//stepper[0].moveTo((MAXPOSITION));
	stepper[0].setMaxSpeed((MAXSPEED));
	//stepper[0].setSpeed((MAXSPEED));
	
	positions[0] = MAXPOSITION;

	steppers.moveTo(positions);
	
	pinMode(5, OUTPUT);
	digitalWrite(5, HIGH);
		
	Serial.begin(230400);
	Serial.println("setup done");
}

void loop() {
	// Move steppers
	sprintf(output, "moving %6ld %6ld %6ld %6ld %6ld %6ld",
		stepper[0].currentPosition(),
		stepper[1].currentPosition(),
		stepper[2].currentPosition(),
		stepper[3].currentPosition(),
		stepper[4].currentPosition(),
		stepper[5].currentPosition());
	Serial.println(output);
	/*
	*/
	if (!steppers.run()) {
		digitalWrite(5, LOW);
	}
}
