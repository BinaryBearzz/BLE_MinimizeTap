#include <Arduino.h>
#include "EventVolume.h"
//author: worawit sayned(BinaryBearzz) 23/05/2023

onVolumeUp _Event_Up;
onVolumeDown _Event_Down;
onSW _Event_SW;

int EventVolume::readValue()
{
    if(_readType == Digital)
    {
        return digitalRead(pinData);
    }
    else if(_readType == Analog)
    {
        return analogRead(pinData);
    }
}

EventVolume::EventVolume(int pin, readType readType)
{
    pinData = pin;
    _readType = readType;
    _Event_Up = NULL;
    _Event_Down = NULL;
    _Event_SW = NULL;
    _inputType = sw;
    if(_inputType == sw)
        pinMode(pinData, INPUT_PULLUP);
    else
        pinMode(pinData, INPUT);
}

EventVolume::EventVolume(int pin, int inMin, int inMax, readType readType)
{
    pinData = pin;
    calibrate(inMin, inMax);
    _readType = readType;
    _Event_Up = NULL;
    _Event_Down = NULL;
    _Event_SW = NULL;
    
    if(_inputType == sw)
        pinMode(pinData, INPUT_PULLUP);
    else
        pinMode(pinData, INPUT);
}

void EventVolume::calibrate(int inMin, int inMax, int outMin, int outMax)
{
    _inputType = volume;
    _inMin = inMin;
    _inMax = inMax;
    _outMin = outMin;
    _outMax = outMax;
}
void EventVolume::setStepIndicator(int setValue)
{
    stepIndicator = setValue;
}

EventVolume::~EventVolume()
{

}

void EventVolume::handler()
{
    int input = readValue();

    if(_inputType == volume)
    {    
        _nml_value = map(input, _inMin, _inMax, _outMin , _outMax);
        // Serial.println("NML:"+ String(_nml_value));
        // Serial.println("previousValue:"+ String(previousValue));
        if(previousValue != _nml_value)
        {
            int difference = _nml_value - previousValue;
            // Serial.println("Difference: "+String(difference));
            if(difference < -stepIndicator)
            {
                _Event_Down();
            }
            else if (difference > stepIndicator)
            {
                _Event_Up();
            }    
            delay(50);
            previousValue = _nml_value;    
        } 
    }  
    else if (_inputType == sw)
    {
        if(pinData > 0)
        {
            currentStateSW = input;
            if (lastStateSW == HIGH && currentStateSW == LOW)
            {
                if(_Event_SW != NULL)
                {
                    _Event_SW(true);
                }
            }
            // Serial.println("The button is pressed");
            else if (lastStateSW == LOW && currentStateSW == HIGH)
            {
                if(_Event_SW != NULL)
                {
                    _Event_SW(false);
                }
            }
            // Serial.println("The button is released");
            lastStateSW = currentStateSW;
        }
    }
    
}

void EventVolume::onEventVolumeUp(onVolumeUp ovU)
{
    _Event_Up = ovU;
}

void EventVolume::onEventVolumeDown(onVolumeDown ovD)
{
    _Event_Down = ovD;
}



void EventVolume::onEventSW(onSW oSW)
{
    _Event_SW = oSW;
}