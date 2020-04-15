#ifndef effects_h
#define effects_h

#include "Arduino.h"
#include "FastLED.h"

#define LED_PIN 13

//CRGB strip[LED_COUNT];
void SnowSparkle(CRGB strip[], byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay);
void setAll(CRGB strip[], byte red, byte green, byte blue);
void setPixel(CRGB strip[], int Pixel, byte red, byte green, byte blue);
void showStrip();
void Fire(CRGB strip[], int Cooling, int Sparking, int SpeedDelay);
void setPixelHeatColor(CRGB strip[], int Pixel, byte temperature);
void rainbowCycle(CRGB strip[], int SpeedDelay, bool &isBreaked);
byte *Wheel(byte WheelPos);

#endif