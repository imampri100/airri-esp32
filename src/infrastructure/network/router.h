#ifndef IRRIGATION_ROUTER_H
#define IRRIGATION_ROUTER_H

#include "infrastructure/network/handler/irrigation_log_handler.h"
#include "infrastructure/network/handler/maintenance_handler.h"
#include "infrastructure/network/handler/pump_handler.h"
#include "infrastructure/network/handler/sensor_log_handler.h"
#include "infrastructure/network/handler/setting_handler.h"
#include "infrastructure/network/handler/status_handler.h"
#include "infrastructure/network/handler/sync_handler.h"
#include "infrastructure/network/http_server.h"

namespace irrigation
{

    // Mendaftarkan semua endpoint REST API ke HttpServer, lalu
    // mendelegasikan tiap request ke handler yang sesuai.
    //
    // Ringkasan API (dikonsumsi mobile app Flutter):
    //   GET    /api/status
    //   GET    /api/logs/sensor?lastId=&limit=
    //   GET    /api/logs/irrigation?lastId=&limit=
    //   DELETE /api/logs
    //   GET    /api/sync
    //   GET    /api/settings/trigger
    //   PUT    /api/settings/trigger
    //   GET    /api/settings/restriction
    //   PUT    /api/settings/restriction
    //   POST   /api/pump/start
    //   POST   /api/pump/stop
    //   POST   /api/pump/test
    //   POST   /api/maintenance/factory-reset
    class Router
    {
    public:
        Router(
            HttpServer &httpServer,
            StatusHandler &statusHandler,
            SensorLogHandler &sensorLogHandler,
            IrrigationLogHandler &irrigationLogHandler,
            SyncHandler &syncHandler,
            SettingHandler &settingHandler,
            PumpHandler &pumpHandler,
            MaintenanceHandler &maintenanceHandler);

        void registerRoutes();

    private:
        HttpServer &httpServer;

        StatusHandler &statusHandler;

        SensorLogHandler &sensorLogHandler;

        IrrigationLogHandler &irrigationLogHandler;

        SyncHandler &syncHandler;

        SettingHandler &settingHandler;

        PumpHandler &pumpHandler;

        MaintenanceHandler &maintenanceHandler;
    };

}

#endif
