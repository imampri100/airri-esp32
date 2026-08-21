#ifndef IRRIGATION_TRIGGER_SETTING_H
#define IRRIGATION_TRIGGER_SETTING_H

#include <Arduino.h>

namespace irrigation
{

struct TriggerSetting
{
    String soilMoistureOperator = "<=";

    float soilMoistureValue = 30.0f;
};

}

#endif