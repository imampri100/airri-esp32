#include "domain/service/decision_engine.h"

namespace irrigation
{

DecisionResult DecisionEngine::evaluate(
    const SensorReading& sensorReading,
    const TriggerSetting& triggerSetting,
    const RestrictionSetting& restrictionSetting
) const
{
    DecisionResult result;

    ReasonCode reason = ReasonCode::TriggerNotMet;

    if (!evaluateTrigger(sensorReading, triggerSetting))
    {
        result.shouldRunPump = false;
        result.reason = ReasonCode::TriggerNotMet;

        return result;
    }

    if (!evaluateRestriction(sensorReading, restrictionSetting, reason))
    {
        result.shouldRunPump = false;
        result.reason = reason;

        return result;
    }

    result.shouldRunPump = true;
    result.reason = ReasonCode::PumpCanRun;

    return result;
}

bool DecisionEngine::compare(
    float sensorValue,
    const String& comparisonOperator,
    float thresholdValue
) const
{
    if (comparisonOperator == "<")
    {
        return sensorValue < thresholdValue;
    }

    if (comparisonOperator == "<=")
    {
        return sensorValue <= thresholdValue;
    }

    if (comparisonOperator == ">")
    {
        return sensorValue > thresholdValue;
    }

    if (comparisonOperator == ">=")
    {
        return sensorValue >= thresholdValue;
    }

    return false;
}

bool DecisionEngine::evaluateTrigger(
    const SensorReading& sensorReading,
    const TriggerSetting& triggerSetting
) const
{
    return compare(
        sensorReading.soilMoisture,
        triggerSetting.soilMoistureOperator,
        triggerSetting.soilMoistureValue
    );
}

bool DecisionEngine::evaluateRestriction(
    const SensorReading& sensorReading,
    const RestrictionSetting& restrictionSetting,
    ReasonCode& reason
) const
{
    if (restrictionSetting.airHumidityEnabled)
    {
        if (compare(
                sensorReading.airHumidity,
                restrictionSetting.airHumidityOperator,
                restrictionSetting.airHumidityValue))
        {
            reason = ReasonCode::RestrictedByHumidity;
            return false;
        }
    }

    if (restrictionSetting.airTemperatureEnabled)
    {
        if (compare(
                sensorReading.airTemperature,
                restrictionSetting.airTemperatureOperator,
                restrictionSetting.airTemperatureValue))
        {
            reason = ReasonCode::RestrictedByTemperature;
            return false;
        }
    }

    if (restrictionSetting.lightIntensityEnabled)
    {
        if (compare(
                sensorReading.lightIntensity,
                restrictionSetting.lightIntensityOperator,
                restrictionSetting.lightIntensityValue))
        {
            reason = ReasonCode::RestrictedByLight;
            return false;
        }
    }

    return true;
}

}