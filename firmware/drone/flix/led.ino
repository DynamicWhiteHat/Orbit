// Copyright (c) 2023 Oleg Kalachev <okalachev@gmail.com>
// Repository: https://github.com/okalachev/flix

// Board's LED control
#include <Adafruit_NeoPixel.h>

#define BLINK_PERIOD 500000
#define STATUS_LED 37 // UPDATED: SK6812 status LEDs
#define LED_COUNT 3
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, STATUS_LED, NEO_RGBW + NEO_KHZ800);


void setupLED() {
	strip.begin();
  strip.show();
}

void setLED(int state) {
	if (state==0) {
		colorSet(strip.Color(255, 0, 0, 0), 0); // Error - Red
	} 
	else if (state==1){
		colorSet(strip.Color(0, 255, 0, 0), 0); // Ready - Green
	}
	else if (state==2) {
		colorSet(strip.Color(0, 0, 255, 0), 0); // Setup - Blue
	}
	else if (state==3) {
		colorSet(strip.Color(255, 255, 0, 0), 0); // Low Battery - Yellow
	}
	else if (state==4) {
		colorSet(strip.Color(0, 0, 0, 255), 0); // Disarmed - White
	}
}

void colorSet(uint32_t c, uint8_t wait) { // From NeoPixel Library
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
   strip.show();
   delay(wait);
}
