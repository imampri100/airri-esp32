#ifndef IRRIGATION_IRRIGATION_LOG_MAPPER_H
#define IRRIGATION_IRRIGATION_LOG_MAPPER_H

#include <ArduinoJson.h>

#include "domain/entity/irrigation_log.h"

namespace irrigation
{

    class IrrigationLogMapper
    {
    public:
        static void toDocument(
            const IrrigationLog &entity,
            JsonDocument &document);

        static IrrigationLog fromDocument(
            const JsonDocument &document);

        static IrrigationLog fromJsonLine(
            const String &line,
            bool &ok);
    };

}

#endif
