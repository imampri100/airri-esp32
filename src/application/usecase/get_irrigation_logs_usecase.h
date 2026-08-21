#ifndef IRRIGATION_GET_IRRIGATION_LOGS_USECASE_H
#define IRRIGATION_GET_IRRIGATION_LOGS_USECASE_H

#include <vector>

#include "domain/entity/irrigation_log.h"
#include "domain/repository/irrigation_log_repository.h"

namespace irrigation
{
    class GetIrrigationLogsUseCase
    {
    public:
        explicit GetIrrigationLogsUseCase(
            IrrigationLogRepository &irrigationLogRepository);

        std::vector<IrrigationLog> execute(
            uint32_t lastId,
            uint32_t limit);

    private:
        IrrigationLogRepository &irrigationLogRepository;
    };
}

#endif
