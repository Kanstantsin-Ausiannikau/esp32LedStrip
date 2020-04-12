
#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "effects.h"

#define LED_COUNT 89
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
void keyPressed();
void onTimer();

GenericFP effects[5] = {&effect1, &effect2, &effect3, &effect4, &effect5}; //create an array of 'GenericFP' function pointers. Notice the '&' operator

CRGB strip[LED_COUNT];

bool buttonState;
int debounceTimeCounter;
long startTimer;

bool isBreaked = false;
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

  buttonState = false;
  debounceTimeCounter = 0;

  attachInterrupt(BUTTON_PIN, keyPressed, HIGH);
}

void ICACHE_RAM_ATTR keyPressed()
{

  startTimer = millis();

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);

  // timer1_isr_init();
  // timer1_attachInterrupt(debounceTime);
  // timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
  // timer1_write(10000);
}

void IRAM_ATTR onTimer()
{
  portENTER_CRITICAL_ISR(&timerMux);
  if (digitalRead(BUTTON_PIN) == HIGH)
  {
    Serial.println(millis() - startTimer);
    //doBreaked = true;

    isBreaked = true;

    currentEffectIndex++;

    Serial.println("pressed");
    timerAlarmDisable(timer);
  }
  portEXIT_CRITICAL_ISR(&timerMux);
}

// void ICACHE_RAM_ATTR debounceTime()
// {
//   if (digitalRead(BUTTON_PIN) == HIGH)
//   {
//     Serial.println(millis() - startTimer);
//     //doBreaked = true;

//     isBreaked = true;
//     timer1_disable();
//     currentEffectIndex++;

//     Serial.println("pressed");
//   }
// }

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

  //Fire(strip, 55, 120, 15);
}

void effect1()
{
  rainbowCycle(strip, 20);
}

void effect2()
{
  Fire(strip, 55, 120, 15);
}

void effect3()
{
  SnowSparkle(strip, 0x10, 0x10, 0x10, 50, random(50, 500));
}

void effect4()
{
  setAll(strip, 0xff, 0xff, 0xff);
  delay(500);
}

void effect5()
{
  setAll(strip, 0, 0, 0);
}