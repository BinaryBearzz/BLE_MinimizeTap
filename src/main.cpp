#include <Arduino.h>
#include <BleKeyboard.h>
#include <EventChange.h>
#include "BLEClient.h"
#define TRIGGER_PIN 2
#define ECHO_PIN 4
#define DETECT_LED 32       // RED
#define READY_DETECT_LED 33 // BLUE

std::string nameDevice = "BLE-MinimizeTaps";
BleKeyboard BLE_KB(nameDevice);

EventChange ev_change;

unsigned long prv_time, prv_time_distance, prv_time_disable = 0;
int blink_interval = 500;

bool RDY_DETECT = true;
float measureDistance = 0;
float trigerDistanceValue = 25;

void blinkStatus();
void scanDistance();
void disableAfterDetect(int duration_ms);
void checkDistanceToMinimize(float sourceDistance, float trigerDistance);

void setup()
{
  Serial.begin(115200);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(DETECT_LED, OUTPUT);
  pinMode(READY_DETECT_LED, OUTPUT);

  digitalWrite(DETECT_LED, LOW);
  digitalWrite(READY_DETECT_LED, LOW);
  BLE_KB.begin();
  Serial.print(F("Starting BLE work!: "));
  Serial.println(nameDevice.c_str());

  ev_change.onChange([&](bool triger)
                     {
        if (BLE_KB.isConnected())
        {
            digitalWrite(DETECT_LED, triger);

            if(triger)
            {
              if(RDY_DETECT)
              {
                RDY_DETECT = LOW;
                digitalWrite(READY_DETECT_LED, RDY_DETECT);
                BLE_KB.press(KEY_LEFT_GUI);
                BLE_KB.print("m");
                delay(50);
                BLE_KB.releaseAll();   
                Serial.println(F("Minimize"));
              }
              prv_time_disable = millis(); //UPDATE time to disable
            }
        } });
  digitalWrite(READY_DETECT_LED, RDY_DETECT);
}

void loop()
{

  scanDistance();

  if (millis() - prv_time_distance > 100)
  {
    unsigned long pulseDuration = pulseIn(ECHO_PIN, HIGH);
    measureDistance = (pulseDuration / 2) / 29.1; // คำนวณเป็น centimeters
    Serial.print(F("Distance: "));
    Serial.print(measureDistance);
    Serial.println(F(" cm"));
    prv_time_distance = millis();
  }
  checkDistanceToMinimize(measureDistance, trigerDistanceValue);
  disableAfterDetect(5000);

  delay(10);
  blinkStatus();
}

#pragma region "Utility function"

void blinkStatus()
{
  if (!BLE_KB.isConnected())
  {
    if (millis() - prv_time > blink_interval)
    {
      digitalWrite(DETECT_LED, !digitalRead(DETECT_LED));
      digitalWrite(READY_DETECT_LED, LOW);
      prv_time = millis();
    }
  }
  else
  {
    if ((RDY_DETECT) && digitalRead(READY_DETECT_LED) == LOW)
    {
      digitalWrite(READY_DETECT_LED, HIGH);
    }
  }
}

void scanDistance()
{
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIGGER_PIN, LOW);
}

void disableAfterDetect(int duration_ms)
{
  if (!RDY_DETECT)
  {
    if (millis() - prv_time_disable > duration_ms)
    {
      RDY_DETECT = true;
      digitalWrite(READY_DETECT_LED, RDY_DETECT);
      prv_time_disable = millis();
    }
  }
}

void checkDistanceToMinimize(float sourceDistance, float trigerDistance)
{
  if (sourceDistance < trigerDistance && sourceDistance != 0)
  {
    ev_change.trigerCallback(true);
  }
  else
  {
    ev_change.trigerCallback(false);
  }
}

#pragma endregion "END Utility function"