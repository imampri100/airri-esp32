#include "infrastructure/mapper/sensor_log_mapper.h"

namespace irrigation
{

    void SensorLogMapper::toDocument(
        const SensorLog &entity,
        JsonDocument &document)
    {
        document.clear();

        document["id"] = entity.id;
        document["createdAt"] = entity.createdAt.value();

        document["soilMoisture"] = entity.soilMoisture;
        document["airHumidity"] = entity.airHumidity;
        document["airTemperature"] = entity.airTemperature;
        document["lightIntensity"] = entity.lightIntensity;

        document["isIrrigationRun"] = entity.isIrrigationRun;
        document["irrigationLogId"] = entity.irrigationLogId;
        document["irrigationRunAt"] = entity.irrigationRunAt.value();
        document["irrigationStopAt"] = entity.irrigationStopAt.value();
        document["irrigationDurationSecond"] = entity.irrigationDurationSecond;
        document["irrigationMillilitre"] = entity.irrigationMillilitre;

        document["isSynced"] = entity.isSynced;
        document["syncedAt"] = entity.syncedAt.value();
    }

    SensorLog SensorLogMapper::fromDocument(
        const JsonDocument &document)
    {
        SensorLog entity;

        entity.id = document["id"] | 0;
        entity.createdAt = Timestamp(document["createdAt"] | 0);

        entity.soilMoisture = document["soilMoisture"] | 0.0f;
        entity.airHumidity = document["airHumidity"] | 0.0f;
        entity.airTemperature = document["airTemperature"] | 0.0f;
        entity.lightIntensity = document["lightIntensity"] | 0.0f;

        entity.isIrrigationRun = document["isIrrigationRun"] | false;
        entity.irrigationLogId = document["irrigationLogId"] | 0;
        entity.irrigationRunAt = Timestamp(document["irrigationRunAt"] | 0);
        entity.irrigationStopAt = Timestamp(document["irrigationStopAt"] | 0);
        entity.irrigationDurationSecond = document["irrigationDurationSecond"] | 0;
        entity.irrigationMillilitre = document["irrigationMillilitre"] | 0.0f;

        entity.isSynced = document["isSynced"] | false;
        entity.syncedAt = Timestamp(document["syncedAt"] | 0);

        return entity;
    }

    SensorLog SensorLogMapper::fromJsonLine(
        const String &line,
        bool &ok)
    {
        StaticJsonDocument<512> document;

        DeserializationError error = deserializeJson(document, line);

        ok = !error;

        if (!ok)
        {
            return SensorLog();
        }

        return fromDocument(document);
    }

}
