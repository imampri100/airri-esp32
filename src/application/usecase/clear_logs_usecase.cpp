#include "application/usecase/clear_logs_usecase.h"

namespace irrigation
{

    ClearLogsUseCase::ClearLogsUseCase(
        SensorLogRepository &sensorLogRepository,
        IrrigationLogRepository &irrigationLogRepository)
        : sensorLogRepository(sensorLogRepository),
          irrigationLogRepository(irrigationLogRepository)
    {
    }

    void ClearLogsUseCase::execute()
    {
        sensorLogRepository.clear();
        irrigationLogRepository.clear();
    }

}
