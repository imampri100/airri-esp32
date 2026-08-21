#ifndef IRRIGATION_SENSOR_LOG_MAPPER_H
#define IRRIGATION_SENSOR_LOG_MAPPER_H

#include <ArduinoJson.h>

#include "domain/entity/sensor_log.h"

namespace irrigation
{

    class SensorLogMapper
    {
    public:
        static void toDocument(
            const SensorLog &entity,
            JsonDocument &document);

        static SensorLog fromDocument(
            const JsonDocument &document);

        static SensorLog fromJsonLine(
            const String &line,
            bool &ok);
    };

}

#endif
