// Copyright (c) 2023 Oleg Kalachev <okalachev@gmail.com>
// Repository: https://github.com/okalachev/flix

// Main firmware file

#include "vector.h"
#include "quaternion.h"
#include "util.h"
#include "config.h"

#define WIFI_ENABLED 1

float t = NAN; // current step time, s
float dt; // time delta from previous step, s
float controlRoll, controlPitch, controlYaw, controlThrottle; // pilot's inputs, range [-1, 1]
float controlMode = NAN;
Vector gyro; // gyroscope data
Vector acc; // accelerometer data, m/s/s
Vector rates; // filtered angular rates, rad/s
Quaternion attitude; // estimated attitude
bool landed; // are we landed and stationary
float motors[4]; // normalized motors thrust in range [0..1]

void setup() {
	setupLED();
	setLED(2);
	Serial.begin(115200);
	print("Initializing flix\n");
	disableBrownOut();
	setupParameters();
	#if BATTERY_ENABLED
		setupBattery();
	#endif
	#if BAROMETER_ENABLED
		setupBarometer();
	#endif
	setupMotors();
	#if WIFI_ENABLED
		setupWiFi();
	#endif
	setupCompass();
	setupLocator();
	setupIMU();
	setupRC();
	setLED(4);
	print("Initializing complete\n");
}

void loop() {
	readIMU();
	loopCompass();
	step();
	readRC();
	estimate();
	control();
	#if BATTERY_ENABLED
		loopBattery();
	#endif
	#if BAROMETER_ENABLED
		loopBarometer();
	#endif
		sendMotors();
		handleInput();
	#if WIFI_ENABLED
		processMavlink();
	#endif
	logData();
	syncParameters();
}
