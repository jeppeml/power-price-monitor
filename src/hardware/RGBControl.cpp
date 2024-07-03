// RGBControl.cpp
#include "RGBControl.hpp"

CRGB leds[NUM_LEDS];  // Instantiate RGB LED

void setupLED() {
    FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS);  // Initialize RGB LED
}

void setRGBColor(uint8_t r, uint8_t g, uint8_t b) {
    leds[0] = CRGB(r, g, b);
    FastLED.show();
}
