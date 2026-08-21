#include "infrastructure/persistence/json_serializer.h"

namespace irrigation
{

    bool JsonSerializer::serialize(
        const JsonDocument &document,
        File &file) const
    {
        return serializeJson(
                   document,
                   file) > 0;
    }

}