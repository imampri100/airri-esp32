#ifndef IRRIGATION_JSON_SERIALIZER_H
#define IRRIGATION_JSON_SERIALIZER_H

#include <ArduinoJson.h>
#include <FS.h>

namespace irrigation
{

    class JsonSerializer
    {
    public:
        bool serialize(
            const JsonDocument &document,
            File &file) const;
    };

}

#endif