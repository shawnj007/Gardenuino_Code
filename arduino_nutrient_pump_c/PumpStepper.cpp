#include "PumpStepper.h"


PumpStepper::PumpStepper(	uint8_t interface,
							uint8_t pin1,
							uint8_t pin2,
							uint8_t pin3,
							uint8_t pin4,
							bool enable) {
	
	uint8_t type = PumpType::CONTINUOUS;
	float   h = 52;
	float	v = 3;
	uint8_t stop_f = 0;
	uint8_t stop_b = 0;
	
	PumpStepper(type, h, v, interface, pin1, pin2, pin3, pin4, stop_f, stop_b, enable);
}

PumpStepper::PumpStepper(	uint8_t type,
							float   h,
							float   v,
							uint8_t interface,
							uint8_t pin1,
							uint8_t pin2,
							uint8_t pin3,
							uint8_t pin4,
							uint8_t stop_f,
							uint8_t stop_b,
							bool enable) {
	
	_as = AccelStepper(interface, pin1, pin2, pin3, pin4, enable);

	_type = type;
	
	_steps_per_millimeter = ( _type == PumpType::SYRINGE ? SYRINGE_STEPS_PER_MILLIMETER : CONTINUOUS_STEPS_PER_MILLIMETER );

	_steps_per_millimeter *= (interface == AccelStepper::FULL4WIRE ? 0.5 : 1.0);
	
	_h  = h;
	_v  = v;
	_a1 = h/v; // units of mm / mL
	
	_steps_per_milliliter = (_steps_per_millimeter * _a1);
	
	_max_position = h * _steps_per_millimeter;
	
	_target_position = 0;
	
	pinMode(stop_f, INPUT_PULLUP);
	pinMode(stop_b, INPUT_PULLUP);
	
	_pin_stop[DIRECTION_FORWARD]  = stop_f;
	_pin_stop[DIRECTION_BACKWARD] = stop_b;
	
}

void PumpStepper::setMaxRate(float rate) { // rate in mL / seconds
	float speed = _steps_per_millimeter * _a1 * rate; // steps / second
	//Serial.print("speed ");
	//Serial.println(speed);
	_as.setMaxSpeed(speed);
	_as.setSpeed(speed);
}

void PumpStepper::setVolumeTime(float milliliters, int seconds) {
	_target_position = _steps_per_milliliter * milliliters;
	float rate = milliliters / seconds;
	_direction = (rate >= 0 ? DIRECTION_FORWARD : DIRECTION_BACKWARD );
	//Serial.print("rate ");
	//Serial.print(rate);
	//Serial.print("mL/sec & steps relative_position ");
	//Serial.println(_target_position);
	_as.moveTo(_target_position);
	setMaxRate(rate);
}

bool PumpStepper::check_stop() {
	long currentPosition = _as.currentPosition();
	//Serial.print((_direction == DIRECTION_FORWARD ? "FORWARD" : "BACKWARD"));
	//Serial.print(" currentPosition ");
	//Serial.print(currentPosition);
	//Serial.print(" _target_position ");
	//Serial.print(_target_position);
	if ((_direction == DIRECTION_FORWARD  && currentPosition >= _target_position)
	 || (_direction == DIRECTION_BACKWARD && currentPosition <= _target_position)) {
	 	return false;
	}
	//Serial.print(" ");
	//Serial.println(digitalRead(_pin_stop[_direction]));
	return ((_type == PumpType::CONTINUOUS) || (digitalRead(_pin_stop[_direction])));  // 0 is closed (grounded) circuit, stop in this direction, 1 is open, keep running
}

void PumpStepper::stop() {
	//Serial.println(" STOP");
	_as.setCurrentPosition(0);
	_as.disableOutputs();
}

bool PumpStepper::runToStop() {
	// Check for endstop signal
	if (check_stop()) {
		_as.run();
		return true;
	} else {
		stop();
		return false;
	}
}

bool PumpStepper::runSpeedToStop() {
	// Check for endstop signal
	if (check_stop()) {
		_as.runSpeed();
		return true;
	} else {
		stop();
		return false;
	}
}

bool PumpStepper::runSpeedToPositionToStop() {
	
	// Check for endstop signal
	if (check_stop()) {
		_as.runSpeedToPosition();
		return true;
	} else {
		stop();
		return false;
	}
}

long PumpStepper::setMaxPosition(long max_position) {
	_max_position = MIN(70.0 * STEPS_PER_MILLIMETER, max_position);
	return _max_position;
}

long PumpStepper::calibrateSyringePump() {
	if (_type == PumpType::SYRINGE) {
		// Count revolutions to:
		
		// Run pump to endstop
		_as.moveTo(SYRINGE_MAX_POSITION);
		_as.setSpeed(100);
		while (_as.isRunning())
			runSpeedToStop();
		_as.setCurrentPosition(0);
		
		long max_pos = 0;
		
		// Run pump to other endstop
		_as.moveTo(-(SYRINGE_MAX_POSITION));
		_as.setSpeed(-100);
		while (_as.isRunning()) {
			runSpeedToStop();
			max_pos = -(_as.currentPosition());
		}
		_as.setCurrentPosition(0);

		// Set the calibrated position of end stop
		return max_pos;
	}
	
	return 0;
}

long PumpStepper::calibrateSyringePumpMaxPosition() {
	return setMaxPosition(calibrateSyringePump());
}


