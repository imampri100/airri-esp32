#ifndef IRRIGATION_SENSOR_LOG_HANDLER_H
#define IRRIGATION_SENSOR_LOG_HANDLER_H

#include "application/usecase/get_sensor_logs_usecase.h"

#include "infrastructure/network/http_server.h"

namespace irrigation
{

    // GET /api/logs/sensor?lastId=0&limit=50
    class SensorLogHandler
    {
    public:
        SensorLogHandler(
            HttpServer &httpServer,
            GetSensorLogsUseCase &getSensorLogsUseCase);

        void handleGetSensorLogs();

    private:
        HttpServer &httpServer;

        GetSensorLogsUseCase &getSensorLogsUseCase;
    };

}

#endif
