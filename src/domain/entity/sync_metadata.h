#include <cstdint>
#ifndef IRRIGATION_SYNC_METADATA_H
#define IRRIGATION_SYNC_METADATA_H

namespace irrigation
{

struct SyncMetadata
{
    uint32_t storageId = 1;

    uint32_t sensorLogLastId = 0;

    uint32_t irrigationLogLastId = 0;
};

}

#endif