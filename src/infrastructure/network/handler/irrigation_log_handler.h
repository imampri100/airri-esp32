#ifndef IRRIGATION_IRRIGATION_LOG_HANDLER_H
#define IRRIGATION_IRRIGATION_LOG_HANDLER_H

#include "application/usecase/get_irrigation_logs_usecase.h"

#include "infrastructure/network/http_server.h"

namespace irrigation
{

    // GET /api/logs/irrigation?lastId=0&limit=50
    class IrrigationLogHandler
    {
    public:
        IrrigationLogHandler(
            HttpServer &httpServer,
            GetIrrigationLogsUseCase &getIrrigationLogsUseCase);

        void handleGetIrrigationLogs();

    private:
        HttpServer &httpServer;

        GetIrrigationLogsUseCase &getIrrigationLogsUseCase;
    };

}

#endif
