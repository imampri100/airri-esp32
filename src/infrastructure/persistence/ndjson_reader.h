#ifndef IRRIGATION_NDJSON_READER_H
#define IRRIGATION_NDJSON_READER_H

#include <Arduino.h>
#include <functional>

#include "infrastructure/persistence/persistence_storage_manager.h"

namespace irrigation
{

    class NdjsonReader
    {
    public:
        explicit NdjsonReader(
            PersistenceStorageManager &storage);

        bool readEach(
            const String &path,
            const std::function<bool(const String &)> &callback);

    private:
        PersistenceStorageManager &storage;
    };

}

#endif