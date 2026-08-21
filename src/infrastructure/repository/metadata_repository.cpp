#include "infrastructure/repository/metadata_repository.h"

#include <ArduinoJson.h>

namespace irrigation
{

    SdMetadataRepository::SdMetadataRepository(
        PersistenceStorageManager &storage,
        JsonSerializer &serializer,
        const String &path)
        : storage(storage),
          serializer(serializer),
          path(path)
    {
    }

    SyncMetadata SdMetadataRepository::getMetadata()
    {
        SyncMetadata metadata;

        File file = storage.openRead(path);
        if (!file)
        {
            return metadata;
        }

        StaticJsonDocument<256> document;
        DeserializationError error = deserializeJson(document, file);
        file.close();

        if (error)
        {
            return metadata;
        }

        metadata.storageId = document["storageId"] | metadata.storageId;
        metadata.sensorLogLastId = document["sensorLogLastId"] | metadata.sensorLogLastId;
        metadata.irrigationLogLastId = document["irrigationLogLastId"] | metadata.irrigationLogLastId;

        return metadata;
    }

    void SdMetadataRepository::saveMetadata(const SyncMetadata &metadata)
    {
        StaticJsonDocument<256> document;

        document["storageId"] = metadata.storageId;
        document["sensorLogLastId"] = metadata.sensorLogLastId;
        document["irrigationLogLastId"] = metadata.irrigationLogLastId;

        storage.remove(path);
        File file = storage.openWrite(path);
        if (!file)
        {
            return;
        }

        serializer.serialize(document, file);
        file.close();
    }

}
