#include "FastLED.h"
#include <Arduino.h>
#include "settings.h"

void SnowSparkle(CRGB strip[], byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay);
void setAll(CRGB strip[], byte red, byte green, byte blue);
void setPixel(CRGB strip[], int Pixel, byte red, byte green, byte blue);
void showStrip();
void Fire(CRGB strip[], int Cooling, int Sparking, int SpeedDelay);
void setPixelHeatColor(CRGB strip[], int Pixel, byte temperature);
void rainbowCycle(CRGB strip[], int SpeedDelay);
byte *Wheel(byte WheelPos);

void SnowSparkle(CRGB strip[], byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay)
{
  setAll(strip, red, green, blue);

  int Pixel = random(FastLED.size());
  Serial.println(Pixel);

  setPixel(strip, Pixel, 0xff, 0xff, 0xff);
  showStrip();
  delay(SparkleDelay);
  setPixel(strip, Pixel, red, green, blue);
  showStrip();
  delay(SpeedDelay);
}

void setAll(CRGB strip[], byte red, byte green, byte blue)
{
  for (int i = 0; i < (FastLED.size()); i++)
  {
    setPixel(strip, i, red, green, blue);
  }
  showStrip();
}

void setPixel(CRGB strip[], int Pixel, byte red, byte green, byte blue)
{
  strip[Pixel].r = green;
  strip[Pixel].g = red;
  strip[Pixel].b = blue;
}

void showStrip()
{
  FastLED.show();
}

void Fire(CRGB strip[], int Cooling, int Sparking, int SpeedDelay)
{

  static byte heat[180];
  int cooldown;

  // Step 1.  Cool down every cell a little
  for (int i = 0; i < FastLED.size(); i++)
  {
    cooldown = random(0, ((Cooling * 10) / FastLED.size()) + 2);

    if (cooldown > heat[i])
    {
      heat[i] = 0;
    }
    else
    {
      heat[i] = heat[i] - cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for (int k = FastLED.size() - 1; k >= 2; k--)
  {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if (random(255) < Sparking)
  {
    int y = random(7);
    heat[y] = heat[y] + random(160, 255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for (int j = 0; j < FastLED.size(); j++)
  {
    setPixelHeatColor(strip, j, heat[j]);
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor(CRGB strip[], int Pixel, byte temperature)
{
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature / 255.0) * 191);

  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2;              // scale up to 0..252

  // figure out which third of the spectrum we're in:
  if (t192 > 0x80)
  { // hottest
    setPixel(strip, Pixel, 255, 255, heatramp);
  }
  else if (t192 > 0x40)
  { // middle
    setPixel(strip, Pixel, 255, heatramp, 0);
  }
  else
  { // coolest
    setPixel(strip, Pixel, heatramp, 0, 0);
  }
}

void rainbowCycle(CRGB strip[], int SpeedDelay)
{
  byte *c;
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++)
  { // 5 cycles of all colors on wheel
    for (i = 0; i < FastLED.size(); i++)
    {
      if (isBreaked)
      {
        Serial.println("returning");
        //currentEffectIndex++;
        setAll(strip, 0, 0, 0);
        
        return;
      }
      c = Wheel(((i * 256 / FastLED.size()) + j) & 255);
      setPixel(strip, i, *c, *(c + 1), *(c + 2));
    }
    showStrip();
    delay(SpeedDelay);
  }
}

byte *Wheel(byte WheelPos)
{
  static byte c[3];

  if (WheelPos < 85)
  {
    c[0] = WheelPos * 3;
    c[1] = 255 - WheelPos * 3;
    c[2] = 0;
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    c[0] = 255 - WheelPos * 3;
    c[1] = 0;
    c[2] = WheelPos * 3;
  }
  else
  {
    WheelPos -= 170;
    c[0] = 0;
    c[1] = WheelPos * 3;
    c[2] = 255 - WheelPos * 3;
  }

  return c;
}
