#include "infrastructure/mapper/irrigation_log_mapper.h"

namespace irrigation
{

    void IrrigationLogMapper::toDocument(
        const IrrigationLog &entity,
        JsonDocument &document)
    {
        document.clear();

        document["id"] = entity.id;
        document["createdAt"] = entity.createdAt.value();
        document["irrigationRunAt"] = entity.irrigationRunAt.value();
        document["irrigationStopAt"] = entity.irrigationStopAt.value();
        document["irrigationDurationSecond"] = entity.irrigationDurationSecond;
        document["irrigationMillilitre"] = entity.irrigationMillilitre;
        document["isSynced"] = entity.isSynced;
        document["syncedAt"] = entity.syncedAt.value();
    }

    IrrigationLog IrrigationLogMapper::fromDocument(
        const JsonDocument &document)
    {
        IrrigationLog entity;

        entity.id = document["id"] | 0;
        entity.createdAt = Timestamp(document["createdAt"] | 0);
        entity.irrigationRunAt = Timestamp(document["irrigationRunAt"] | 0);
        entity.irrigationStopAt = Timestamp(document["irrigationStopAt"] | 0);
        entity.irrigationDurationSecond = document["irrigationDurationSecond"] | 0;
        entity.irrigationMillilitre = document["irrigationMillilitre"] | 0.0f;
        entity.isSynced = document["isSynced"] | false;
        entity.syncedAt = Timestamp(document["syncedAt"] | 0);

        return entity;
    }

    IrrigationLog IrrigationLogMapper::fromJsonLine(
        const String &line,
        bool &ok)
    {
        StaticJsonDocument<384> document;

        DeserializationError error = deserializeJson(document, line);

        ok = !error;

        if (!ok)
        {
            return IrrigationLog();
        }

        return fromDocument(document);
    }

}
