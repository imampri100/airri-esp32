#ifndef IRRIGATION_GET_SYNC_USECASE_H
#define IRRIGATION_GET_SYNC_USECASE_H

#include "application/dto/sync_dto.h"

#include "domain/repository/irrigation_log_repository.h"
#include "domain/repository/sensor_log_repository.h"
#include "domain/repository/sync_repository.h"

namespace irrigation
{
    class GetSyncUseCase
    {
    public:
        GetSyncUseCase(
            SyncRepository &syncRepository,
            SensorLogRepository &sensorLogRepository,
            IrrigationLogRepository &irrigationLogRepository);

        SyncDto execute();

    private:
        SyncRepository &syncRepository;

        SensorLogRepository &sensorLogRepository;

        IrrigationLogRepository &irrigationLogRepository;
    };
}

#endif
