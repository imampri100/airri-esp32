#ifndef IRRIGATION_DECISION_ENGINE_H
#define IRRIGATION_DECISION_ENGINE_H

#include "domain/entity/decision_result.h"
#include "domain/entity/restriction_setting.h"
#include "domain/entity/sensor_reading.h"
#include "domain/entity/trigger_setting.h"

namespace irrigation
{

class DecisionEngine
{
public:
    DecisionResult evaluate(
        const SensorReading& sensorReading,
        const TriggerSetting& triggerSetting,
        const RestrictionSetting& restrictionSetting
    ) const;

private:
    bool compare(
        float sensorValue,
        const String& comparisonOperator,
        float thresholdValue
    ) const;

    bool evaluateTrigger(
        const SensorReading& sensorReading,
        const TriggerSetting& triggerSetting
    ) const;

    bool evaluateRestriction(
        const SensorReading& sensorReading,
        const RestrictionSetting& restrictionSetting,
        ReasonCode& reason
    ) const;
};

}

#endif