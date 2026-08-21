#ifndef IRRIGATION_NDJSON_WRITER_H
#define IRRIGATION_NDJSON_WRITER_H

#include <ArduinoJson.h>

#include "infrastructure/persistence/json_serializer.h"
#include "infrastructure/persistence/persistence_storage_manager.h"

namespace irrigation
{

class NdjsonWriter
{
public:

    NdjsonWriter(
        PersistenceStorageManager& storage,
        JsonSerializer& serializer
    );

    bool append(
        const String& path,
        const JsonDocument& document
    );

private:

    PersistenceStorageManager& storage;

    JsonSerializer& serializer;

};

}

#endif