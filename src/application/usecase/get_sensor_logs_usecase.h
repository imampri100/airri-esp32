#ifndef IRRIGATION_GET_SENSOR_LOGS_USECASE_H
#define IRRIGATION_GET_SENSOR_LOGS_USECASE_H

#include <vector>

#include "domain/entity/sensor_log.h"
#include "domain/repository/sensor_log_repository.h"

namespace irrigation
{
    class GetSensorLogsUseCase
    {
    public:
        explicit GetSensorLogsUseCase(
            SensorLogRepository &sensorLogRepository);

        std::vector<SensorLog> execute(
            uint32_t lastId,
            uint32_t limit);

    private:
        SensorLogRepository &sensorLogRepository;
    };
}

#endif
