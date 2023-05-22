#include <Arduino.h>

typedef std::function<void(boolean)> cb_onchange;

class EventChange
{
private:
    cb_onchange regis_cb_onchange;
    long prv_val_onchange = 0;
public:
    EventChange(/* args */);
    ~EventChange(){};
    void onChange(cb_onchange cb_isUp_onChange);
    void handler(int value);
    void trigerCallback(boolean execute);
};

EventChange::EventChange(/* args */)
{
    regis_cb_onchange = NULL;
}

void EventChange::onChange(cb_onchange cb_isUp_onChange)
{
    regis_cb_onchange = cb_isUp_onChange;
}

void EventChange::handler(int value)
{
    if(prv_val_onchange - value > 0)
    {
        if(regis_cb_onchange != NULL)
        {
            regis_cb_onchange(true);
        }
    }
    else if (prv_val_onchange - value < 0)
    {
        if(regis_cb_onchange != NULL)
        {
            regis_cb_onchange(false);
        }
    }
    prv_val_onchange = value;
}

void EventChange::trigerCallback(boolean execute)
{
     if(regis_cb_onchange != NULL)
     {
         regis_cb_onchange(execute);
     }
}
