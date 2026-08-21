#ifndef IRRIGATION_RESTRICTION_SETTING_H
#define IRRIGATION_RESTRICTION_SETTING_H

#include <Arduino.h>

namespace irrigation
{

struct RestrictionSetting
{
    bool airHumidityEnabled = false;

    String airHumidityOperator = ">=";

    float airHumidityValue = 80.0f;



    bool airTemperatureEnabled = false;

    String airTemperatureOperator = ">=";

    float airTemperatureValue = 35.0f;



    bool lightIntensityEnabled = false;

    String lightIntensityOperator = "<=";

    float lightIntensityValue = 100.0f;



    // Safety net: batas maksimal pompa nyala terus-menerus tanpa henti
    // (detik), buat jaga-jaga kalau soil sensor rusak/putus sehingga
    // decision engine terus menyuruh pompa ON. Dulu hardcoded 15 detik
    // di MonitorEnvironmentTask, sekarang bisa diatur lewat
    // GET/PUT /api/settings/restriction.
    uint32_t maxPumpRuntimeSecond = 15;
};

}

#endif