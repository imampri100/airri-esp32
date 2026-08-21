#include "infrastructure/network/handler/maintenance_handler.h"

namespace irrigation
{

    MaintenanceHandler::MaintenanceHandler(
        HttpServer &httpServer,
        ClearLogsUseCase &clearLogsUseCase,
        FactoryResetUseCase &factoryResetUseCase)
        : httpServer(httpServer),
          clearLogsUseCase(clearLogsUseCase),
          factoryResetUseCase(factoryResetUseCase)
    {
    }

    void MaintenanceHandler::handleClearLogs()
    {
        clearLogsUseCase.execute();

        httpServer.sendJson(200, "{\"success\":true}");
    }

    void MaintenanceHandler::handleFactoryReset()
    {
        factoryResetUseCase.execute();

        httpServer.sendJson(200, "{\"success\":true}");
    }

}
