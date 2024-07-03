// RGBControl.hpp
#ifndef RGBCONTROL_HPP
#define RGBCONTROL_HPP

#include <FastLED.h>

#define NUM_LEDS 1         // Number of RGB LED beads
#define DATA_PIN D8        // The pin for controlling RGB LED
#define LED_TYPE NEOPIXEL  // RGB LED strip type

void setupLED();
void setRGBColor(uint8_t r, uint8_t g, uint8_t b);

#endif // RGBCONTROL_HPP
