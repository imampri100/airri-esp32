#ifndef IRRIGATION_SYNC_REPOSITORY_H
#define IRRIGATION_SYNC_REPOSITORY_H

#include "domain/entity/sync_metadata.h"

namespace irrigation
{

class SyncRepository
{
public:

    virtual ~SyncRepository() = default;

    virtual SyncMetadata getMetadata() = 0;

    virtual void saveMetadata(
        const SyncMetadata& metadata
    ) = 0;
};

}

#endif