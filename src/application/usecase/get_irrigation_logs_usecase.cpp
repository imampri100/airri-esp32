#include "application/usecase/get_irrigation_logs_usecase.h"

namespace irrigation
{

    GetIrrigationLogsUseCase::GetIrrigationLogsUseCase(
        IrrigationLogRepository &irrigationLogRepository)
        : irrigationLogRepository(irrigationLogRepository)
    {
    }

    std::vector<IrrigationLog> GetIrrigationLogsUseCase::execute(
        uint32_t lastId,
        uint32_t limit)
    {
        return irrigationLogRepository.getAfterId(lastId, limit);
    }

}
