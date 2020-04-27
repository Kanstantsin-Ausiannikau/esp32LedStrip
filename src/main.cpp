
#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "effects.h"

#define LED_COUNT 178
#define STASSID "WifiNet4"
#define STAPSK "22349026"

// Указываем, к какому порту подключен вход ленты DIN.
#define LED_PIN 13
#define BUTTON_PIN 12

typedef void (*GenericFP)(); //function pointer prototype to a function which takes an 'int' an returns 'void'

void effect1();
void effect2();
void effect3();
void effect4();
void effect5();
void effect6();
void effect7();
void effect8();
void effect9();
void effect10();
void effect11();
void keyPressed();
void onTimer();

GenericFP effects[11] = {&effect1, &effect2, &effect3, &effect4, &effect5, &effect6, &effect7, &effect8, &effect9, &effect10, &effect11}; //create an array of 'GenericFP' function pointers. Notice the '&' operator

CRGB strip[LED_COUNT];

byte counter;

bool buttonState;
int debounceTimeCounter;
long startTimer;

bool isBreaked;
int currentEffectIndex = 0;

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void setup()
{

  delay(500);

  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA
      .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else // U_SPIFFS
          type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
      })
      .onEnd([]() {
        Serial.println("\nEnd");
      })
      .onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      })
      .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
          Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
          Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
          Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
          Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
          Serial.println("End Failed");
      });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  FastLED.addLeds<WS2812B, LED_PIN, RGB>(strip, LED_COUNT);

  randomSeed(analogRead(0));

  isBreaked = false;
  buttonState = false;
  debounceTimeCounter = 0;
  attachInterrupt(BUTTON_PIN, keyPressed, HIGH);

  counter = 0;
}

void ICACHE_RAM_ATTR keyPressed()
{
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 20000, true);
  timerAlarmEnable(timer);
}

void IRAM_ATTR onTimer()
{
  Serial.println("pressed");
  Serial.println(millis() - startTimer);
  portENTER_CRITICAL_ISR(&timerMux);

  if (digitalRead(BUTTON_PIN) == HIGH)
  {
    isBreaked = true;
    currentEffectIndex++;
  }
  timerAlarmDisable(timer);
  portEXIT_CRITICAL_ISR(&timerMux);
}

void loop()
{
  ArduinoOTA.handle();

  if (isBreaked)
  {
    Serial.println("Switch to new effect");

    if (currentEffectIndex == sizeof(effects) / sizeof(int))
    {
      currentEffectIndex = 0;
    }
    isBreaked = false;
  }

  effects[currentEffectIndex]();
}

void effect1()
{
  SnowSparkle(strip, 0x10, 0x10, 0x10, 50, random(50, 500));
}

void effect2()
{
  Fire(strip, 110, 180, 15);
}

void effect3()
{
  rainbowCycle(strip, 20, isBreaked);
}

void effect4()
{
  // setAll(strip, 0xff, 0xff, 0xff); - full brightness
  setAll(strip, 100, 100, 100);
  delay(500);
}

void effect5()
{
  //meteorRain(strip, 0xff,0xff,0xff,10, 64, true, 30);
  setAll(strip, 100, 50, 50);
}

void effect6()
{
  rainbowGayer(strip, counter);
  counter++;
}

void effect7()
{
  //OneByOne(strip, 255, 0, 0, LED_COUNT);   //красный
  //OneByOne(strip, 255, 100, 0, LED_COUNT); //оранжевый
  //OneByOne(strip, 255, 255, 0, LED_COUNT); // жёлтый
  //OneByOne(strip, 0, 255, 0, LED_COUNT);   //зелёный
  //OneByOne(strip, 0, 255, 255, LED_COUNT); //голубой
  //OneByOne(strip, 0, 0, 255, LED_COUNT);   //синий
  //OneByOne(strip, 127, 0, 255, LED_COUNT); //фиолетовый
  //OneByOne(strip, 0, 0, 255, LED_COUNT);   //синий
  //OneByOne(strip, 0, 255, 255, LED_COUNT); //голубой
  //OneByOne(strip, 0, 255, 0, LED_COUNT);   //зелёный
  //OneByOne(strip, 255, 255, 0, LED_COUNT); // жёлтый
  //OneByOne(strip, 255, 100, 0, LED_COUNT); //оранжевый
  Gradient(strip, LED_COUNT);
}

void effect8()
{
  FadingAway(strip, 255, 0, 0, LED_COUNT);
  FadingAway(strip, 0, 255, 0, LED_COUNT);
  FadingAway(strip, 0, 0, 255, LED_COUNT);
}

void effect9()
{
  byte red = random(256);
  byte green = random(256);
  byte blue = random(256);
  OneByOne(strip, red, green, blue, LED_COUNT);
}

void effect10()
{
  ByOneRandom(strip, 100, 100, 100, LED_COUNT);
  ByOneRandom(strip, 100, 0, 0, LED_COUNT);
  ByOneRandom(strip, 0, 100, 0, LED_COUNT);
  ByOneRandom(strip, 0, 0, 100, LED_COUNT);
}
void effect11()
{
  setAll(strip, 0, 0, 0);
}