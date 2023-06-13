#include <Arduino.h>
//author: worawit sayned(BinaryBearzz) 23/05/2023
#ifndef EVENTVOLUME
#define EVENTVOLUME

enum readType { Digital, Analog};
enum inputType { volume, sw};

typedef std::function<void()> onVolumeUp;
typedef std::function<void()> onVolumeDown;
typedef std::function<void(boolean state)> onSW;

class EventVolume
{
private:
    int pinData = -1;
    int previousValue = 0;
    int _inMin = 0;
    int _inMax = 0;
    int _outMin = 0;
    int _outMax = 0;
    int _nml_value = 0;
    int currentStateSW = 0;
    int lastStateSW = 0;
    int stepIndicator = 5;
    int readValue();
    readType _readType;
    inputType _inputType;
public:
    EventVolume(int pin, readType readType);
    EventVolume(int pin, int inMin, int inMax, readType readType);
    void calibrate(int inMin, int inMax, int outMin = 1, int outMax = 256);
    ~EventVolume();
    void handler();
        void setStepIndicator(int setValue);
        void onEventVolumeUp(onVolumeUp ovU);
        void onEventVolumeDown(onVolumeDown ovD);
        void onEventSW(onSW oSW);


};

#endif