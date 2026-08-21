#ifndef IRRIGATION_SYNC_DTO_H
#define IRRIGATION_SYNC_DTO_H

#include "domain/entity/sync_metadata.h"

namespace irrigation
{

struct SyncDto
{
    SyncMetadata metadata;

    uint32_t sensorLogCount = 0;

    uint32_t irrigationLogCount = 0;

    // ID tertua yang masih tersedia (bisa berubah kalau ada rotasi/purge
    // log lama). Dipakai mobile app untuk deteksi gap: kalau
    // lastSyncedId + 1 < FirstId, berarti ada data yang sudah hilang.
    uint32_t sensorLogFirstId = 0;

    uint32_t irrigationLogFirstId = 0;
};

}

#endif