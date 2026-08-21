#ifndef IRRIGATION_SENSOR_LOG_H
#define IRRIGATION_SENSOR_LOG_H

#include <Arduino.h>

#include "domain/value_object/timestamp.h"

namespace irrigation
{

struct SensorLog
{
    uint32_t id = 0;

    Timestamp createdAt;

    float soilMoisture = 0.0f;

    float airHumidity = 0.0f;

    float airTemperature = 0.0f;

    float lightIntensity = 0.0f;

    bool isIrrigationRun = false;

    uint32_t irrigationLogId = 0;

    Timestamp irrigationRunAt;

    Timestamp irrigationStopAt;

    uint32_t irrigationDurationSecond = 0;

    float irrigationMillilitre = 0.0f;

    bool isSynced = false;

    Timestamp syncedAt;
};

}

#endif