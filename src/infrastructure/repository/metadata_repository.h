#ifndef IRRIGATION_INFRA_METADATA_REPOSITORY_H
#define IRRIGATION_INFRA_METADATA_REPOSITORY_H

#include "domain/repository/sync_repository.h"

#include "infrastructure/persistence/json_serializer.h"
#include "infrastructure/persistence/persistence_storage_manager.h"

namespace irrigation
{

    // Implementasi SyncRepository (domain) -> metadata/sync.json di SD Card.
    class SdMetadataRepository : public SyncRepository
    {
    public:
        SdMetadataRepository(
            PersistenceStorageManager &storage,
            JsonSerializer &serializer,
            const String &path);

        SyncMetadata getMetadata() override;

        void saveMetadata(const SyncMetadata &metadata) override;

    private:
        PersistenceStorageManager &storage;

        JsonSerializer &serializer;

        String path;
    };

}

#endif
