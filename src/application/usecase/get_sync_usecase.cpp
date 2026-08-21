#include "application/usecase/get_sync_usecase.h"

namespace irrigation
{

    GetSyncUseCase::GetSyncUseCase(
        SyncRepository &syncRepository,
        SensorLogRepository &sensorLogRepository,
        IrrigationLogRepository &irrigationLogRepository)
        : syncRepository(syncRepository),
          sensorLogRepository(sensorLogRepository),
          irrigationLogRepository(irrigationLogRepository)
    {
    }

    SyncDto GetSyncUseCase::execute()
    {
        SyncDto dto;

        dto.metadata = syncRepository.getMetadata();

        // sensorLogLastId/irrigationLogLastId di metadata tersimpan cuma
        // di-set ulang saat factory reset (lihat FactoryResetUseCase), jadi
        // kalau dibiarkan apa adanya nilainya basi begitu ada log baru
        // ditambahkan. Timpa dengan nilai live dari repository supaya
        // /api/sync selalu mencerminkan kondisi terkini.
        dto.metadata.sensorLogLastId = sensorLogRepository.getLastId();
        dto.metadata.irrigationLogLastId = irrigationLogRepository.getLastId();

        dto.sensorLogCount = sensorLogRepository.count();
        dto.irrigationLogCount = irrigationLogRepository.count();

        dto.sensorLogFirstId = sensorLogRepository.getFirstId();
        dto.irrigationLogFirstId = irrigationLogRepository.getFirstId();

        return dto;
    }

}
