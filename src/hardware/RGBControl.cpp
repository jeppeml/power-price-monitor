// RGBControl.cpp
#include "RGBControl.hpp"

CRGB leds[NUM_LEDS];  // Instantiate RGB LED

void setupLED() {
    FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS);  // Initialize RGB LED
}

void setRGBColor(const uint8_t r,const uint8_t g,const uint8_t b) {
    leds[0] = CRGB(r, g, b);
    FastLED.show();
}

void setNeopixelColorRGB(const uint8_t* colorRGB) {
    setRGBColor(colorRGB[0],colorRGB[1],colorRGB[2]);
}

