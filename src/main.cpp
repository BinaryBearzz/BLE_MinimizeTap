#include <Arduino.h>
#include <BleKeyboard.h>
#include <EventChange.h>
#include "BLEClient.h"
#define TRIGGER_PIN 2
#define ECHO_PIN 4
#define LED 5
#include <HCSR04.h>

std::string nameDevice = "BLE-MinimizeTaps";
BleKeyboard BLE_KB(nameDevice, "Espressif", 100);

UltraSonicDistanceSensor distanceSensor(TRIGGER_PIN, ECHO_PIN); // Initialize sensor that uses digital pins 13 and 12.

EventChange ev_change;
unsigned long prv, prv_distance = 0;
int interval = 500;
float distance; // ประกาศตัวแปรเก็บค่าระยะ

void blinkStatus()
{
  if (!BLE_KB.isConnected())
  {
    if (millis() - prv > interval)
    {
      digitalWrite(LED, !digitalRead(LED));
      prv = millis();
    }
  }
  else
  {
    digitalWrite(LED, LOW);
  }
}

void echoDistance()
{
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIGGER_PIN, LOW);
}

void setup()
{
  Serial.begin(115200);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  Serial.println("Starting BLE work!");
  BLE_KB.begin();

  ev_change.onChange([&](bool triger)
                     {
        if (BLE_KB.isConnected())
        {
            if(triger)
            {
                BLE_KB.press(KEY_LEFT_GUI);
                BLE_KB.print("m");
                delay(50);
                BLE_KB.releaseAll();   
                Serial.println(F("Minimize"));
            }
            // else{
            //     Serial.println(F("not thing"));
            // } 
        } });
}

void loop()
{

  echoDistance();

  if (millis() - prv_distance > 100)
  {
    unsigned long pulseDuration = pulseIn(ECHO_PIN, HIGH);
    distance = (pulseDuration / 2) / 29.1; // คำนวณเป็น centimeters
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    prv_distance = millis();
  }
  if (distance < 25 && distance != 0)
  {
    ev_change.trigerCallback(true);
  }
  // else
  // {
  //   ev_change.trigerCallback(false);
  // }
  delay(10);
  blinkStatus();
}