#include "application/usecase/get_sensor_logs_usecase.h"

namespace irrigation
{

    GetSensorLogsUseCase::GetSensorLogsUseCase(
        SensorLogRepository &sensorLogRepository)
        : sensorLogRepository(sensorLogRepository)
    {
    }

    std::vector<SensorLog> GetSensorLogsUseCase::execute(
        uint32_t lastId,
        uint32_t limit)
    {
        return sensorLogRepository.getAfterId(lastId, limit);
    }

}
