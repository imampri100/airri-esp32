#ifndef IRRIGATION_CLEAR_LOGS_USECASE_H
#define IRRIGATION_CLEAR_LOGS_USECASE_H

#include "domain/repository/irrigation_log_repository.h"
#include "domain/repository/sensor_log_repository.h"

namespace irrigation
{
    class ClearLogsUseCase
    {
    public:
        ClearLogsUseCase(
            SensorLogRepository &sensorLogRepository,
            IrrigationLogRepository &irrigationLogRepository);

        void execute();

    private:
        SensorLogRepository &sensorLogRepository;

        IrrigationLogRepository &irrigationLogRepository;
    };
}

#endif
