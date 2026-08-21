#include "application/usecase/factory_reset_usecase.h"

#include "domain/entity/restriction_setting.h"
#include "domain/entity/sync_metadata.h"
#include "domain/entity/trigger_setting.h"

namespace irrigation
{

    FactoryResetUseCase::FactoryResetUseCase(
        SettingRepository &settingRepository,
        SyncRepository &syncRepository,
        SensorLogRepository &sensorLogRepository,
        IrrigationLogRepository &irrigationLogRepository)
        : settingRepository(settingRepository),
          syncRepository(syncRepository),
          sensorLogRepository(sensorLogRepository),
          irrigationLogRepository(irrigationLogRepository)
    {
    }

    void FactoryResetUseCase::execute()
    {
        settingRepository.saveTriggerSetting(TriggerSetting());
        settingRepository.saveRestrictionSetting(RestrictionSetting());

        sensorLogRepository.clear();
        irrigationLogRepository.clear();

        syncRepository.saveMetadata(SyncMetadata());
    }

}
