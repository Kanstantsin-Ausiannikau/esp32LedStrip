#include "FastLED.h"
#include <Arduino.h>
#include "settings.h"

void SnowSparkle(CRGB strip[], byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay);
void setAll(CRGB strip[], byte red, byte green, byte blue);
void setPixel(CRGB strip[], int Pixel, byte red, byte green, byte blue);
void showStrip();
void Fire(CRGB strip[], int Cooling, int Sparking, int SpeedDelay);
void setPixelHeatColor(CRGB strip[], int Pixel, byte temperature);
void rainbowCycle(CRGB strip[], int SpeedDelay, bool &isBreaked);
byte *Wheel(byte WheelPos);
void meteorRain(CRGB strip[], byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay);
void fadeToBlack(CRGB strip[], int ledNo, byte fadeValue);
void rainbowGayer(CRGB strip [], byte counter);
void OneByOne(CRGB strip[], byte red, byte green, byte blue, int StripLength);
void ByOneRandom(CRGB strip[], byte red, byte green, byte blue, int StripLength);
void Gradient(CRGB strip[], int StripLength);
void FadingAway(CRGB strip[], byte red, byte green, byte blue, int StripLength);

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

void rainbowCycle(CRGB strip[], int SpeedDelay, bool &isBreaked)
{
  byte *c;
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++)
  { // 5 cycles of all colors on wheel
    for (i = 0; i < FastLED.size(); i++)
    {
      if (isBreaked)
      {
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

void meteorRain(CRGB strip[], byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) 
{  
  setAll(strip, 0,0,0);
 
  for(int i = 0; i < FastLED.size()*2; i++) 
  {
   
      // if (isBreaked)
      // {
      //   setAll(strip, 0, 0, 0);
      //   return;
      // }
 
    // fade brightness all LEDs one step
    for(int j=0; j<FastLED.size(); j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        fadeToBlack(strip, j, meteorTrailDecay );        
      }
    }
   
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <FastLED.size()) && (i-j>=0) ) {
        setPixel(strip, i-j, red, green, blue);
      }
    }
   
    showStrip();
    delay(SpeedDelay);
  }
}

void fadeToBlack(CRGB strip[], int ledNo, byte fadeValue) 
{
   strip[ledNo].fadeToBlackBy( fadeValue );
}

void rainbowGayer(CRGB strip [], byte counter)
{
  for (int i = 0; i < FastLED.size(); i++ ) {         // от 0 до первой трети
    strip[i] = CHSV(counter + i * 2, 255, 255);  // HSV. Увеличивать HUE (цвет)
    // умножение i уменьшает шаг радуги
  }
  FastLED.show();
  delay(15);         // скорость движения радуги
}

void OneByOne(CRGB strip[], byte red, byte green, byte blue, int StripLength)
{
  for(int i = 0; i < StripLength/2; i++)
  {
    setPixel(strip, i, red, green, blue);
    setPixel(strip, StripLength-i, red, green, blue);
    showStrip();
    delay(25);    
  }
}

void ByOneRandom(CRGB strip[], byte red, byte green, byte blue, int StripLength)
{  
  delay(5000);
  //создаем массив из неподсвеченных светодиодов
  int unflashed[StripLength];
  for(int i = 0; i < StripLength; i++)
  {
    unflashed[i] = i;
  }
  
  //зажигаем светодиоды по одному StripLength раз
  int unflashedCount = StripLength;
  for(int i = 0; i < StripLength; i++)
  {
    //выбираем светодиод из неподсвеченных
    int j = random(unflashedCount+1);

    //зажигаем его
    setPixel(strip, unflashed[j], red, green, blue);
    showStrip();
    delay(100);
    unflashedCount = unflashedCount - 1;

    //смещаем весь массив на 1 влево и заменяем на "-1" загоревшийся светодиод
    int last = StripLength;
    for(int i = j; i < unflashedCount; i++)
    {
      unflashed[i] = unflashed[i+1];
    }
    unflashed[last] = -1;
    last = last - 1;
  }

}

void Gradient(CRGB strip[], int StripLength)
{
  for(int i = 255, j = 0; i >= 0, j <= 255; i-=1, j+=1) //blue to red
  {
    setAll(strip, j, 0, i);
    showStrip();
    delay(10);
  }
  
  for(int i = 255, j = 0; i >= 0, j <= 255; i-=1, j+=1) //red to green
  {
    setAll(strip, i, j, 0);
    showStrip();
    delay(10);
  }
  for(int i = 255, j = 0; i >= 0, j <= 255; i-=1, j+=1) //green to blue
  {
    setAll(strip, 0, i, j);
    showStrip();
    delay(10);
  }
}
void FadingAway(CRGB strip[], byte red, byte green, byte blue, int StripLength)
{
  int i = 0;
  int j = 0;
  int k = 0;
  bool isBright = false;
  while(isBright == false) //повышаем яркость
  {
    setAll(strip, i, j, k);
    showStrip();
    delay(40);

    i+=3;
    j+=3;
    k+=3;

    if (i > red)
    {
      i = red;
    }
    if (j > green)
    {
      j = green;
    }
    if (k > blue)
    {
      k = blue;
    }

    if (i >= red && j >= green && k >= blue)
    {
      if (j >= green)
      {
        if (k >= blue)
        {
          isBright = true;
        }
      }
    }
  }

  
}


