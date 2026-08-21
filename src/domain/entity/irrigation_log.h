#ifndef IRRIGATION_IRRIGATION_LOG_H
#define IRRIGATION_IRRIGATION_LOG_H

#include "domain/value_object/timestamp.h"

namespace irrigation
{

struct IrrigationLog
{
    uint32_t id = 0;

    Timestamp createdAt;

    Timestamp irrigationRunAt;

    Timestamp irrigationStopAt;

    uint32_t irrigationDurationSecond = 0;

    float irrigationMillilitre = 0.0f;

    bool isSynced = false;

    Timestamp syncedAt;
};

}

#endif