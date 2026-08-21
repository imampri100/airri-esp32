#ifndef IRRIGATION_MAINTENANCE_HANDLER_H
#define IRRIGATION_MAINTENANCE_HANDLER_H

#include "application/usecase/clear_logs_usecase.h"
#include "application/usecase/factory_reset_usecase.h"

#include "infrastructure/network/http_server.h"

namespace irrigation
{

    // DELETE /api/logs, POST /api/maintenance/factory-reset
    class MaintenanceHandler
    {
    public:
        MaintenanceHandler(
            HttpServer &httpServer,
            ClearLogsUseCase &clearLogsUseCase,
            FactoryResetUseCase &factoryResetUseCase);

        void handleClearLogs();

        void handleFactoryReset();

    private:
        HttpServer &httpServer;

        ClearLogsUseCase &clearLogsUseCase;

        FactoryResetUseCase &factoryResetUseCase;
    };

}

#endif
