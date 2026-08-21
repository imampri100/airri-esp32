#ifndef IRRIGATION_FACTORY_RESET_USECASE_H
#define IRRIGATION_FACTORY_RESET_USECASE_H

#include "domain/repository/irrigation_log_repository.h"
#include "domain/repository/sensor_log_repository.h"
#include "domain/repository/setting_repository.h"
#include "domain/repository/sync_repository.h"

namespace irrigation
{
    class FactoryResetUseCase
    {
    public:
        FactoryResetUseCase(
            SettingRepository &settingRepository,
            SyncRepository &syncRepository,
            SensorLogRepository &sensorLogRepository,
            IrrigationLogRepository &irrigationLogRepository);

        // Mengembalikan trigger/restriction ke default, hapus semua log,
        // dan reset metadata sinkronisasi. TIDAK menghapus kredensial WiFi.
        void execute();

    private:
        SettingRepository &settingRepository;

        SyncRepository &syncRepository;

        SensorLogRepository &sensorLogRepository;

        IrrigationLogRepository &irrigationLogRepository;
    };
}

#endif
