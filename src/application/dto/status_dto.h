#ifndef IRRIGATION_STATUS_DTO_H
#define IRRIGATION_STATUS_DTO_H

#include "domain/entity/decision_result.h"
#include "domain/entity/restriction_setting.h"
#include "domain/entity/sensor_reading.h"
#include "domain/entity/trigger_setting.h"

namespace irrigation
{

struct StatusDto
{
    SensorReading sensorReading;

    TriggerSetting triggerSetting;

    RestrictionSetting restrictionSetting;

    DecisionResult decisionResult;

    bool pumpRunning = false;

    // false kalau RTC gagal DAN fallback NTP juga belum berhasil - dalam
    // kondisi ini createdAt di log sensor/irigasi bakal 0 dan rotasi/
    // retensi log skip total (lihat HybridTimeProvider, LogRetentionTask).
    bool timeSynchronized = false;
};

}

#endif